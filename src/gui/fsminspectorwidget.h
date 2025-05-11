/**
 * @author Adam Taha (xtahaa00)
 * @file fsminspectorwidget.h
 * @brief Declaration of FsmInspectorWidget, a UI component for inspecting and summarizing FSM structure.
 */

#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class QScrollArea;
class QVBoxLayout;
class AutomatonEditor;
class StateItem;

/**
 * @class FsmInspectorWidget
 * @brief A widget that provides a visual summary and interactive overview of an FSM.
 *
 * Displays state dependency information, global FSM metadata such as start state, delay,
 * and allowed input, and allows state selection via buttons.
 */
class FsmInspectorWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the FSM inspector.
     * @param editor Pointer to the AutomatonEditor managing the FSM.
     * @param parent Optional parent widget.
     */
    explicit FsmInspectorWidget(AutomatonEditor *editor, QWidget *parent = nullptr);

    /**
     * @brief Updates the inspector content based on the current FSM data.
     */
    void refresh();

signals:
    /**
     * @brief Emitted when the Save button is clicked.
     */
    void saveRequested();

private:
    /**
     * @brief Adds a new row representing a state and its incoming dependencies.
     * @param st Pointer to the StateItem to visualize.
     */
    void addRow(StateItem *st);

    AutomatonEditor *m_editor;     ///< Reference to the FSM editor.
    QScrollArea *m_scroll;         ///< Scroll area for state list.
    QVBoxLayout *m_listLay;        ///< Layout for listing state entries.

    // FSM summary widgets (bottom section)
    QLabel *delayLbl = nullptr;    ///< Label showing step delay.
    QLabel *startLbl = nullptr;    ///< Label showing start state.
    QLabel *activeLbl = nullptr;   ///< Label showing active state.
    QLabel *descrLbl = nullptr;    ///< Label showing FSM description.
    QLabel *allowedLbl = nullptr;  ///< Label showing allowed input characters.

    QPushButton *saveBtn = nullptr; ///< Button for triggering FSM save action.
};
