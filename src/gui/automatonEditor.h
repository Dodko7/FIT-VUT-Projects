#ifndef AUTOMATONEDITOR_H
#define AUTOMATONEDITOR_H

#include <QGraphicsView>
#include <QStack>
#include "transitionitem.h"

class QGraphicsScene;
class QGraphicsLineItem;
class StateItem;

/**
 * @author Adam Taha (xtahaa00)
 * @file automatonEditor.h
 * @brief Declaration of the AutomatonEditor class, a graphical canvas for editing finite state machines.
 */

/**
 * @class AutomatonEditor
 * @brief A custom QGraphicsView that provides a scalable and interactive editor
 *        for finite state machines (FSMs), supporting states, transitions,
 *        selection, and undo functionality.
 */
class AutomatonEditor : public QGraphicsView
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the AutomatonEditor.
     * @param parent Parent widget (optional).
     */
    explicit AutomatonEditor(QWidget *parent = nullptr);

    /**
     * @brief Opens dialog and adds a new state to the scene.
     * @param name Optional state name.
     * @param isEnd Whether the state is accepting.
     */
    void addState(const QString &name, bool isEnd);

    /**
     * @brief Inserts a state at a given position.
     * @param name State name.
     * @param isEnd Whether the state is an end state.
     * @param delayMs Delay in milliseconds.
     * @param pos Scene position to insert the state.
     * @return Pointer to created StateItem.
     */
    StateItem* insertState(const QString& name, bool isEnd, int delayMs, const QPointF& pos);

    /**
     * @brief Launches dialog to insert the start state.
     * @param scenePos Scene position where the state should be added.
     */
    void createStartState(const QPointF &scenePos);

    /**
     * @brief Creates a transition between two states with output and condition labels.
     * @param fromName Name of source state.
     * @param toName Name of destination state.
     * @param output Output symbol.
     * @param condition Condition string.
     */
    void createTransition(const QString &fromName,
                          const QString &toName,
                          const QString &output,
                          const QString &condition);

    /**
     * @brief Adds test states to the scene. (For development/debug)
     */
    void addTestStates();

    /**
     * @brief Adjusts the view to fit all scene items.
     */
    void fitScene();

    /**
     * @brief Clears all items from the scene and resets state.
     */
    void clearAutomaton();

    /**
     * @brief Returns all active states.
     * @return Const reference to vector of StateItem pointers.
     */
    const QVector<StateItem*>& states() const { return m_states; }

    /**
     * @brief Selects a state programmatically and centers the view on it.
     * @param s Pointer to StateItem to select.
     */
    void selectState(StateItem *s);

    /**
     * @brief Gets the FSM's designated start state.
     * @return Pointer to the start StateItem, or nullptr.
     */
    StateItem* startState() const;

    /**
     * @brief Gets the currently active state in the FSM.
     * @return Pointer to the active StateItem, or nullptr.
     */
    StateItem* activeState() const;

    /**
     * @brief Returns the step delay for the FSM simulation.
     * @return Delay in milliseconds.
     */
    int stepDelay() const { return m_stepDelay; }

    /**
     * @brief Returns the FSM's textual description.
     * @return Reference to the description string.
     */
    const QString& description() const { return m_description; }

    /**
     * @brief Sets the FSM step delay.
     * @param ms Delay in milliseconds.
     */
    void setStepDelay(int ms) { m_stepDelay = ms; }

    /**
     * @brief Sets the FSM's textual description.
     * @param txt New description.
     */
    void setDescription(QString txt) { m_description.swap(txt); }

    /**
     * @brief Sets allowed input symbols for the FSM.
     * @param s Input characters.
     */
    void setAllowedInput(const QString &s) { m_allowedInput = s; }

    /**
     * @brief Returns allowed input symbols for the FSM.
     * @return Input string.
     */
    QString allowedInput() const { return m_allowedInput; }

    /**
    * @brief Saves the specified state to backend.
    * @param state Pointer to the StateItem to save.
    */
    void saveState(StateItem* state);

    /**
     * @brief Saves the specified transition to backend.
     * @param transition Pointer to the TransitionItem to save.
     */
    void saveTransition(TransitionItem* transition);
    
    /**
     * @brief Saves the entire FSM structure to backend.
     */
    void saveFsmStructure();



signals:
    /**
     * @brief Emitted when a state is selected.
     * @param state Pointer to the selected StateItem.
     */
    void stateSelected(StateItem *state);

    /**
     * @brief Emitted when a transition is selected.
     * @param t Pointer to the selected TransitionItem.
     */
    void transitionSelected(TransitionItem *t);

    /**
     * @brief Emitted to request opening the transition creation dialog.
     */
    void requestTransitionDialog();

protected:
    /**
     * @brief Handles resize events and triggers auto-fit of the scene.
     * @param event Resize event.
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * @brief Handles key presses for state/transition operations.
     * @param event Key event.
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief Handles mouse press events for selection and creation.
     * @param event Mouse event.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Handles mouse movement over the scene.
     * @param event Mouse event.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief Handles mouse release events.
     * @param event Mouse event.
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    /**
     * @brief Enters transition creation mode.
     */
    void enterTransitionMode();

    /**
     * @brief Exits transition creation mode.
     * @param clearTemp Whether to clear temporary elements.
     */
    void exitTransitionMode(bool clearTemp = true);

    /**
     * @brief Checks if a transition already exists between two states.
     * @param from Source state.
     * @param to Destination state.
     * @return True if a transition exists, false otherwise.
     */
    bool transitionExists(StateItem* from, StateItem *to) const;

    QGraphicsScene *m_scene;                    ///< Graphics scene holding all items.
    QVector<StateItem*> m_states;               ///< List of active states.
    QStack<StateItem*> deletedStates;           ///< Stack for undoing deleted states.
    QStack<TransitionItem*> deletedTransitions; ///< Stack for undoing deleted transitions.

    int m_stepDelay {0};                        ///< Step delay in milliseconds.
    QString m_description;                      ///< FSM description.
    QString m_allowedInput;                     ///< Allowed input characters.

    bool m_placeState {false};                  ///< True if user is placing a new state.
};

#endif // AUTOMATONEDITOR_H
