/**
 * @author Adam Taha (xtahaa00)
 * @file transitioninspectorwidget.cpp
 * @brief Provides a GUI component for viewing and editing properties of
 *        a selected transition between two states in the automaton.
 */

#include "transitioninspectorwidget.h"
#include "transitionitem.h"
#include "stateitem.h"

#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

/**
 * @brief Constructs the TransitionInspectorWidget and initializes UI elements.
 * @param parent Optional parent widget.
 */
TransitionInspectorWidget::TransitionInspectorWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *form = new QFormLayout(this);

    fromLbl   = new QLabel("-", this);
    toLbl     = new QLabel("-", this);
    outputLbl = new QLabel("-", this);
    condLbl   = new QLabel("-", this);

    form->addRow(tr("From State:"), fromLbl);
    form->addRow(tr("To State:"),   toLbl);
    form->addRow(tr("Output:"),     outputLbl);
    form->addRow(tr("Condition:"),  condLbl);

    // Save button
    saveBtn = new QPushButton(tr("Save"), this);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(saveBtn);
    connect(saveBtn, &QPushButton::clicked, this, &TransitionInspectorWidget::saveRequested);
}

/**
 * @brief Clears all fields in the inspector to a default state.
 */
void TransitionInspectorWidget::clearFields()
{
    fromLbl  ->setText("-");
    toLbl    ->setText("-");
    outputLbl->setText("-");
    condLbl  ->setText("-");
}

/**
 * @brief Populates the inspector with details from the selected transition.
 * @param t Pointer to the TransitionItem being inspected. If null or invalid, fields are cleared.
 */
void TransitionInspectorWidget::setTransition(TransitionItem *t)
{
    if (!t || !t->source() || !t->dest()) {
        clearFields();
        return;
    }

    fromLbl  ->setText(t->source()->name());
    toLbl    ->setText(t->dest()->name());
    outputLbl->setText(t->output());
    condLbl  ->setText(t->condition());
}
