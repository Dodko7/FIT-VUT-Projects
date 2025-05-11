/**
 * @author Adam Taha (xtahaa00)
 * @file fsmcreationdialog.h
 * @brief Declaration of FsmCreationDialog, a dialog for creating a new finite state machine.
 */

#pragma once
#include <QDialog>

class QLineEdit;
class QSpinBox;

/**
 * @class FsmCreationDialog
 * @brief Dialog interface for gathering metadata and settings required to create a new FSM.
 *
 * Includes fields for name, description, delay, and allowed input characters.
 */
class FsmCreationDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the FSM creation dialog.
     * @param parent Optional parent widget.
     */
    explicit FsmCreationDialog(QWidget *parent = nullptr);

    /**
     * @brief Returns the FSM name entered by the user.
     * @return Name string.
     */
    QString name() const;

    /**
     * @brief Returns the FSM description entered by the user.
     * @return Description string.
     */
    QString description() const;

    /**
     * @brief Returns the delay between FSM steps.
     * @return Delay in milliseconds.
     */
    int stepDelay() const;

    /**
     * @brief Returns the allowed characters input by the user.
     * @return Allowed characters as a QString.
     */
    QString allowed() const;

private:
    QLineEdit *nameEdit;     ///< Input field for the FSM name.
    QLineEdit *descEdit;     ///< Input field for the FSM description.
    QLineEdit *allowedEdit;  ///< Input field for allowed characters.
    QSpinBox *delaySpin;     ///< Spin box for step delay in milliseconds.
};
