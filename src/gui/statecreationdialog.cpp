/**
 * @author Adam Taha (xtahaa00)
 * @file statecreationdialog.cpp
 * @brief General-purpose dialog for creating a new state. Allows specifying the state
 *        name, delay, and whether it's an end state.
 */

#include "statecreationdialog.h"

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>

/**
 * @brief Constructs the state creation dialog.
 * 
 * Sets up input fields for the state name, delay (ms), and a checkbox to mark it as an end state.
 * @param parent Optional parent widget.
 */
StateCreationDialog::StateCreationDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("State Creation"));
    setModal(true);
    setFixedWidth(250);

    nameEdit = new QLineEdit(this);
    delaySpin = new QSpinBox(this);
    delaySpin->setRange(0, 60000);
    delaySpin->setSuffix(" ms");
    delaySpin->setValue(0);

    endCheck = new QCheckBox(this);

    auto *form = new QFormLayout;
    form->addRow(tr("Name:"),    nameEdit);
    form->addRow(tr("State Delay:"), delaySpin);
    form->addRow(tr("End State"), endCheck);

    auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok |
                                        QDialogButtonBox::Cancel,
                                    Qt::Horizontal, this);
    connect(bb, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *vl = new QVBoxLayout(this);
    vl->addLayout(form);
    vl->addWidget(bb);
}

/**
 * @brief Retrieves the name entered for the new state.
 * @return State name as a QString.
 */
QString StateCreationDialog::name() const { return nameEdit->text(); }

/**
 * @brief Retrieves the delay value entered for the state.
 * @return Delay in milliseconds.
 */
int StateCreationDialog::delay() const { return delaySpin->value(); }

/**
 * @brief Indicates whether the state should be marked as an end state.
 * @return True if the checkbox is checked, false otherwise.
 */
bool StateCreationDialog::isEnd() const { return endCheck->isChecked(); }
