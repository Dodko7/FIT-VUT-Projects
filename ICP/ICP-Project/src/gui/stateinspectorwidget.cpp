/**
 * @author Adam Taha (xtahaa00)
 * @file stateinspectorwidget.cpp
 * @brief GUI widget for inspecting and modifying properties of a selected state,
 *        including its name, delay, transitions, and status flags.
 */

#include "stateinspectorwidget.h"
#include "stateitem.h"
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>

/**
 * @brief Constructs the StateInspectorWidget and initializes UI components.
 * @param parent Optional parent widget.
 */
StateInspectorWidget::StateInspectorWidget(QWidget *parent)
    : QWidget(parent)
    , m_nameEdit(new QLineEdit(this))
    , m_statusLbl(new QLabel(this))
    , m_fromLbl(new QLabel(this))
    , m_toLbl(new QLabel(this))
    , m_delayLbl(new QLabel(this))
    , m_delaySpin(new QSpinBox(this))
    , m_breakpointBtn(new QPushButton("Toggle Breakpoint", this))
    , m_endBtn(new QPushButton("Toggle End State", this))
{
    auto *layout = new QFormLayout(this);

    layout->addRow("Name:", m_nameEdit);
    layout->addRow("Status:", m_statusLbl);
    layout->addRow("Incoming Transitions:", m_fromLbl);
    layout->addRow("Outgoing Transitions:", m_toLbl);
    m_delaySpin->setRange(0, 60000); 
    m_delaySpin->setSuffix(" ms");
    layout->addRow("State Delay:", m_delaySpin);

    layout->addWidget(m_breakpointBtn);
    layout->addWidget(m_endBtn);

    connect(m_nameEdit, &QLineEdit::returnPressed, this, &StateInspectorWidget::renameState);
    connect(m_breakpointBtn, &QPushButton::clicked, this, &StateInspectorWidget::toggleBreakpoint);
    connect(m_endBtn, &QPushButton::clicked, this, &StateInspectorWidget::toggleEndState);
    connect(m_delaySpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int v){ if(m_state) m_state->setDelayMs(v); });

    // Save button
    saveBtn = new QPushButton(tr("Save"), this);
    layout->addWidget(saveBtn);
    connect(saveBtn, &QPushButton::clicked, this, &StateInspectorWidget::saveRequested);
}

/**
 * @brief Sets the state to be displayed and edited in the inspector.
 * @param state Pointer to the selected StateItem, or nullptr to clear the UI.
 */
void StateInspectorWidget::setState(StateItem *state)
{
    if (!state || !state->scene()) {
        m_state = nullptr;

        m_nameEdit->setEnabled(false);
        m_breakpointBtn->setEnabled(false);
        m_endBtn->setEnabled(false);
        m_delaySpin->setEnabled(false);

        m_nameEdit->setText("-");
        m_statusLbl->setText("-");
        m_fromLbl->setText("-");
        m_toLbl->setText("-");
        m_delaySpin->setValue(0);
        return;
    }

    m_state = state;

    m_nameEdit->setEnabled(true);
    m_breakpointBtn->setEnabled(true);
    m_endBtn->setEnabled(true);
    m_delaySpin->setEnabled(true);

    m_nameEdit->setText(state->name());
    m_statusLbl->setText(state->isActive() ? "Active" : "Idle");
    m_fromLbl->setText(QString::number(state->outgoingCount()));
    m_toLbl->setText(QString::number(state->incomingCount()));
    m_delaySpin->setValue(state->delayMs());
}

/**
 * @brief Updates the state's name based on the text entered in the name edit field.
 */
void StateInspectorWidget::renameState()
{
    if (m_state)
        m_state->setName(m_nameEdit->text());
}

/**
 * @brief Toggles the breakpoint flag for the selected state.
 */
void StateInspectorWidget::toggleBreakpoint()
{
    if (m_state)
        m_state->setBreakPoint(!m_state->isBreakPoint());
}

/**
 * @brief Toggles the end state status of the selected state.
 */
void StateInspectorWidget::toggleEndState()
{
    if (m_state)
        m_state->setEndState(!m_state->isEndState());
}
