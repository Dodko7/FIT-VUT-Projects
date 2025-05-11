/**
 * @author Adam Taha (xtahaa00)
 * @file fsminspectorwidget.cpp
 * @brief GUI component that summarizes and inspects the structure of the entire FSM.
 *        Displays state dependencies and global metadata such as delay, start/active states,
 *        allowed input, and description.
 */

#include "fsminspectorwidget.h"
#include "automatonEditor.h"
#include "stateitem.h"
#include "transitionitem.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>

/**
 * @brief Constructs the FSM inspector widget with a scrollable state list and summary section.
 * @param ed Pointer to the associated AutomatonEditor.
 * @param p Parent widget.
 */
FsmInspectorWidget::FsmInspectorWidget(AutomatonEditor *ed, QWidget *p)
    : QWidget(p), m_editor(ed)
{
    m_scroll = new QScrollArea(this);
    m_scroll->setWidgetResizable(true);

    auto *container = new QWidget(m_scroll);
    m_listLay = new QVBoxLayout(container);
    m_listLay->setAlignment(Qt::AlignTop);
    m_scroll->setWidget(container);

    auto *vl = new QVBoxLayout(this);
    vl->setContentsMargins(4,4,4,4);
    vl->addWidget(m_scroll);

    // Create summary section
    auto *summary = new QWidget(this);
    auto *form = new QFormLayout(summary);
    form->setContentsMargins(2,2,2,2);

    delayLbl = new QLabel("-", this);
    startLbl = new QLabel("-", this);
    activeLbl = new QLabel("-", this);
    allowedLbl = new QLabel("-", this);
    descrLbl = new QLabel("-", this);
    descrLbl->setWordWrap(true);

    form->addRow(tr("FSM step delay:"), delayLbl);
    form->addRow(tr("Start state:"), startLbl);
    form->addRow(tr("Active state:"), activeLbl);
    form->addRow(tr("Allowed input:"), allowedLbl);
    form->addRow(tr("Description:"), descrLbl);

    saveBtn = new QPushButton(tr("Save"), this);
    form->addRow("", saveBtn);
    connect(saveBtn, &QPushButton::clicked, this, &FsmInspectorWidget::saveRequested);

    auto *vl2 = qobject_cast<QVBoxLayout*>(layout()); // outer layout
    vl2->addWidget(summary, 0); // add summary at the bottom

    if (m_editor) refresh();
}

/**
 * @brief Refreshes the inspector contents based on the current FSM state.
 *        Updates summary labels and regenerates the list of states.
 */
void FsmInspectorWidget::refresh()
{
    if (!m_editor) return;

    // Summary info
    delayLbl->setText(QString::number(m_editor->stepDelay()) + " ms");

    if (auto *s = m_editor->startState())
        startLbl->setText(s->name());
    else
        startLbl->setText(tr("-"));

    if (auto *a = m_editor->activeState())
        activeLbl->setText(a->name());
    else
        activeLbl->setText(tr("-"));

    descrLbl->setText(m_editor->description());
    allowedLbl->setText(m_editor->allowedInput());

    // Clear and rebuild state list
    QLayoutItem *it;
    while ((it = m_listLay->takeAt(0))) {
        delete it->widget();
        delete it;
    }

    for (StateItem *st : m_editor->states())
        addRow(st);
}

/**
 * @brief Adds a row to the list for a given state, including dependency info and selection button.
 * @param st Pointer to the state to display.
 */
void FsmInspectorWidget::addRow(StateItem *st)
{
    // Build list of names of source states for incoming transitions
    QStringList deps;
    for (auto *tr : st->incoming())
        deps << tr->source()->name();
    const QString depText = deps.isEmpty() ? tr("None") : deps.join(", ");

    auto *btn = new QPushButton(st->name(), this);
    btn->setFlat(true);
    btn->setStyleSheet("text-align:left");

    auto *lbl = new QLabel(depText, this);
    lbl->setStyleSheet("color: gray");

    auto *row = new QWidget(this);
    auto *hl  = new QHBoxLayout(row); 
    hl->setContentsMargins(0,0,0,0);
    hl->addWidget(btn); 
    hl->addStretch(); 
    hl->addWidget(lbl);

    connect(btn, &QPushButton::clicked, this, [this, st] {
        m_editor->selectState(st); // switch back to State inspector
    });

    m_listLay->addWidget(row);
}
