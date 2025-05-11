/**
 * @author Adam Taha (xtahaa00)
 * @file startcreationdialog.h
 * @brief Declaration of StartStateCreationDialog, a specialized dialog for creating start states.
 */

#pragma once
#include "statecreationdialog.h"

/**
 * @class StartStateCreationDialog
 * @brief A dialog used specifically to create the initial (start) state in an FSM.
 *
 * Inherits from StateCreationDialog but disables the option to mark the state as an end state.
 */
class StartStateCreationDialog : public StateCreationDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the start state creation dialog.
     * @param parent Optional parent widget.
     */
    explicit StartStateCreationDialog(QWidget *parent = nullptr);
};
