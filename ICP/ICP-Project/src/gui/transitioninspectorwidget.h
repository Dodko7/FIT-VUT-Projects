/**
 * @author Adam Taha (xtahaa00)
 * @file transitioninspectorwidget.h
 * @brief Declaration of the TransitionInspectorWidget class for displaying and editing FSM transitions.
 */

#ifndef TRANSITIONINSPECTORWIDGET_H
#define TRANSITIONINSPECTORWIDGET_H

#include <QWidget>
#include <QPushButton>

class QLabel;
class StateItem;
class TransitionItem;

/**
 * @class TransitionInspectorWidget
 * @brief A GUI widget that displays and allows inspection of a selected FSM transition.
 *
 * Shows the source and destination state names, output, and condition of a transition.
 */
class TransitionInspectorWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the TransitionInspectorWidget.
     * @param parent Optional parent widget.
     */
    explicit TransitionInspectorWidget(QWidget *parent = nullptr);

    /**
     * @brief Sets the transition to be displayed in the inspector.
     * @param t Pointer to the TransitionItem to inspect.
     */
    void setTransition(TransitionItem *t);

signals:
    /**
     * @brief Emitted when the Save button is clicked by the user.
     */
    void saveRequested();

private:
    /**
     * @brief Clears all label fields to their default display.
     */
    void clearFields();

    QLabel *fromLbl{nullptr};     ///< Label showing the source state.
    QLabel *toLbl{nullptr};       ///< Label showing the destination state.
    QLabel *outputLbl{nullptr};   ///< Label showing the output string.
    QLabel *condLbl{nullptr};     ///< Label showing the condition.

    QPushButton *saveBtn = nullptr; ///< Save button widget.
};

#endif // TRANSITIONINSPECTORWIDGET_H
