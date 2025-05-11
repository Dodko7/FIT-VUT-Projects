/**
 * @author Adam Taha (xtahaa00)
 * @file fsmcreationdialog.cpp
 * @brief Dialog for specifying metadata when creating a new finite state machine.
 *        Includes fields for name, description, step delay, and allowed input symbols.
 */

#include "fsmcreationdialog.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>

/**
 * @brief Constructs a modal FSM creation dialog with fields for name, description, delay, and allowed input.
 * @param p Parent widget.
 */
FsmCreationDialog::FsmCreationDialog(QWidget *p) : QDialog(p)
{
    setWindowTitle(tr("FSM Creation"));
    setModal(true);
    setFixedWidth(280);

    nameEdit = new QLineEdit(this);
    descEdit = new QLineEdit(this);
    delaySpin = new QSpinBox(this);
    delaySpin->setRange(0, 60000);
    delaySpin->setSuffix(" ms");

    allowedEdit = new QLineEdit(this);

    auto *form = new QFormLayout;
    form->addRow(tr("Automaton Name:"), nameEdit);
    form->addRow(tr("Description:"), descEdit);
    form->addRow(tr("Step Delay:"), delaySpin);
    form->addRow(tr("Allowed Characters:"), allowedEdit);

    auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(bb, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *vl = new QVBoxLayout(this);
    vl->addLayout(form);
    vl->addWidget(bb);
}

/**
 * @brief Returns the name entered for the FSM.
 * @return FSM name as a QString.
 */
QString FsmCreationDialog::name() const
{
    return nameEdit->text();
}

/**
 * @brief Returns the description entered for the FSM.
 * @return FSM description as a QString.
 */
QString FsmCreationDialog::description() const
{
    return descEdit->text();
}

/**
 * @brief Returns the step delay (in milliseconds) entered by the user.
 * @return Integer value in milliseconds.
 */
int FsmCreationDialog::stepDelay() const
{
    return delaySpin->value();
}

/**
 * @brief Returns the allowed input characters for the FSM.
 * @return Input alphabet as a QString.
 */
QString FsmCreationDialog::allowed() const
{
    return allowedEdit->text();
}
