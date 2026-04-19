/**
 * @author Adam Taha (xtahaa00)
 * @file transitioncreationdialog.h
 * @brief Declaration of the TransitionCreationDialog class, a dialog used to define new transitions between FSM states.
 */

#ifndef TRANSITIONCREATIONDIALOG_H
#define TRANSITIONCREATIONDIALOG_H

#include <QDialog>

class QLineEdit;

/**
 * @class TransitionCreationDialog
 * @brief A dialog to gather information needed to create a new transition between two FSM states.
 *
 * Includes fields for source state name, destination state name, output symbol, and a condition.
 */
class TransitionCreationDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructs the transition creation dialog.
     * @param parent Optional parent widget.
     */
    explicit TransitionCreationDialog(QWidget *parent = nullptr);

    /**
     * @brief Returns the name of the source state.
     * @return QString representing the 'from' state name.
     */
    QString fromState() const;

    /**
     * @brief Returns the name of the destination state.
     * @return QString representing the 'to' state name.
     */
    QString toState() const;

    /**
     * @brief Returns the output associated with this transition.
     * @return Output string.
     */
    QString output() const;

    /**
     * @brief Returns the condition for triggering this transition.
     * @return Condition string.
     */
    QString condition() const;

private:
    QLineEdit *fromEdit;   ///< Input field for source state name.
    QLineEdit *toEdit;     ///< Input field for destination state name.
    QLineEdit *outputEdit; ///< Input field for output symbol.
    QLineEdit *condEdit;   ///< Input field for transition condition.
};

#endif // TRANSITIONCREATIONDIALOG_H
