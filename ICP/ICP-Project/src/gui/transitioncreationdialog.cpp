/**
 * @author Adam Taha (xtahaa00)
 * @file transitioncreationdialog.cpp
 * @brief Dialog for creating a new transition between two states. Collects input/output
 *        strings and condition logic.
 */

#include "transitioncreationdialog.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>

/**
 * @brief Constructs the transition creation dialog.
 * @param parent Optional parent widget.
 *
 * Sets up the form fields for selecting the source and destination states,
 * as well as defining the transition's output and condition.
 */
TransitionCreationDialog::TransitionCreationDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Transition Creation"));

    auto *form = new QFormLayout(this);

    fromEdit   = new QLineEdit(this);
    toEdit     = new QLineEdit(this);
    outputEdit = new QLineEdit(this);
    condEdit   = new QLineEdit(this);

    form->addRow(tr("From State:"), fromEdit);
    form->addRow(tr("To State:"),   toEdit);
    form->addRow(tr("Output:"),     outputEdit);
    form->addRow(tr("Condition:"),  condEdit);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    form->addWidget(buttons);
}

/**
 * @brief Gets the source state name entered by the user.
 * @return Name of the source state as a QString.
 */
QString TransitionCreationDialog::fromState() const
{
    return fromEdit->text();
}

/**
 * @brief Gets the destination state name entered by the user.
 * @return Name of the destination state as a QString.
 */
QString TransitionCreationDialog::toState() const
{
    return toEdit->text();
}

/**
 * @brief Gets the output string defined for the transition.
 * @return Output string as a QString.
 */
QString TransitionCreationDialog::output() const
{
    return outputEdit->text();
}

/**
 * @brief Gets the condition logic string defined for the transition.
 * @return Condition string as a QString.
 */
QString TransitionCreationDialog::condition() const
{
    return condEdit->text();
}
