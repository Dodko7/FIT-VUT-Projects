/**
 * @author Adam Taha (xtahaa00)
 * @file stateinspectorwidget.h
 * @brief Declaration of the StateInspectorWidget class, a widget used to inspect and modify FSM state properties.
 */

#ifndef STATEINSPECTORWIDGET_H
#define STATEINSPECTORWIDGET_H

#include <QWidget>
#include <QSpinBox>

class QLineEdit;
class QLabel;
class QPushButton;
class StateItem;

/**
 * @class StateInspectorWidget
 * @brief A GUI widget to inspect and modify properties of a selected FSM state.
 *
 * Allows the user to rename a state, toggle breakpoint and end state flags,
 * view transition counts, and adjust delay.
 */
class StateInspectorWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the StateInspectorWidget.
     * @param parent Optional parent widget.
     */
    explicit StateInspectorWidget(QWidget *parent = nullptr);

    /**
     * @brief Updates the inspector to reflect the currently selected state.
     * @param state Pointer to the selected StateItem. If nullptr, clears the display.
     */
    void setState(StateItem *state);

private slots:
    /**
     * @brief Renames the state based on user input.
     */
    void renameState();

    /**
     * @brief Toggles the breakpoint flag of the selected state.
     */
    void toggleBreakpoint();

    /**
     * @brief Toggles the end state status of the selected state.
     */
    void toggleEndState();

signals:
    /**
     * @brief Emitted when the Save button is clicked.
     */
    void saveRequested();

private:
    QLineEdit *m_nameEdit;         ///< Text input for state name.
    QLabel    *m_statusLbl;        ///< Label showing if state is active.
    QLabel    *m_fromLbl;          ///< Label showing number of outgoing transitions.
    QLabel    *m_toLbl;            ///< Label showing number of incoming transitions.
    QLabel    *m_delayLbl;         ///< Label for delay spinbox description.
    QSpinBox  *m_delaySpin;        ///< Spin box for setting delay in milliseconds.

    QPushButton *m_breakpointBtn;  ///< Button to toggle breakpoint.
    QPushButton *m_endBtn;         ///< Button to toggle end state flag.
    QPushButton *saveBtn = nullptr;///< Button to emit saveRequested signal.

    StateItem *m_state {nullptr};  ///< Currently inspected state.
};

#endif // STATEINSPECTORWIDGET_H
