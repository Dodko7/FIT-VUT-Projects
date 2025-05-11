/**
 * @author Adam Taha (xtahaa00)
 * @file statecreationdialog.h
 * @brief Declaration of the StateCreationDialog class, a dialog for creating a new state in the FSM.
 */

#pragma once
#include <QDialog>

class QLineEdit;
class QSpinBox;
class QCheckBox;

/**
 * @class StateCreationDialog
 * @brief A dialog that collects user input to create a new state in the finite state machine.
 *
 * Allows setting the state's name, delay (in ms), and whether it's an end state.
 */
class StateCreationDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the dialog.
     * @param parent Optional parent widget.
     */
    explicit StateCreationDialog(QWidget *parent = nullptr);

    /**
     * @brief Retrieves the name entered for the state.
     * @return State name as a QString.
     */
    QString name() const;

    /**
     * @brief Retrieves the delay for the state.
     * @return Delay in milliseconds.
     */
    int delay() const;

    /**
     * @brief Checks if the state should be marked as an end state.
     * @return True if checked, false otherwise.
     */
    bool isEnd() const;

private:
    QLineEdit *nameEdit;     ///< Text input for the state's name.
    QSpinBox *delaySpin;     ///< Spin box for state delay (ms).
    QCheckBox *endCheck;     ///< Checkbox to mark the state as an end state.
};
