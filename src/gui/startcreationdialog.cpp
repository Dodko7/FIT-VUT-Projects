/**
 * @author Adam Taha (xtahaa00)
 * @file startcreationdialog.cpp
 * @brief Specialized dialog for creating the start state. It inherits from
 *        StateCreationDialog but disables the end state checkbox.
 */

#include "startcreationdialog.h"

#include <QCheckBox>

/**
 * @brief Constructor for StartStateCreationDialog.
 *
 * Initializes the dialog and disables the "end state" checkbox to ensure that
 * the start state cannot simultaneously be marked as an end state.
 *
 * @param p Optional parent widget.
 */
StartStateCreationDialog::StartStateCreationDialog(QWidget *p)
    : StateCreationDialog(p)
{
    setWindowTitle(tr("Start State Creation"));

    if (auto box = findChild<QCheckBox*>())
    {
        box->setChecked(false);
        box->setEnabled(false);
    }
}
