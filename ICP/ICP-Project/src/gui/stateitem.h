/**
 * @author Adam Taha (xtahaa00)
 * @file stateitem.h
 * @brief Declaration of the StateItem class, which represents a visual state node in the FSM editor.
 */

#ifndef STATEITEM_H
#define STATEITEM_H

#include <QGraphicsObject>
#include <QVector>

class TransitionItem;

/**
 * @class StateItem
 * @brief Visual node representing a state in the automaton.
 *
 * Legend:
 * - Classic:            single black ring
 * - Breakpoint:         black ring + red dashed ring
 * - End:                black ring + <gap> + black ring
 * - End + Breakpoint:   black ring + <gap> + red dashed
 */
class StateItem : public QGraphicsObject
{
    Q_OBJECT

    // Geometry constants (device-independent px)
    static constexpr qreal BASE_RADIUS = 30.0;  //!< Inner ring radius
    static constexpr qreal RING_WIDTH  = 4.0;   //!< Stroke thickness
    static constexpr qreal END_GAP     = 3.0;   //!< Extra space for end states

    int m_delayMs {0};           ///< State delay in milliseconds (shown in dialog and state inspector)
    QPointF m_savedPos;          ///< Last stored GUI position (not displayed)

public:
    /**
     * @brief Constructs a state item.
     * @param name State name.
     * @param isEnd Whether the state is an end state.
     */
    explicit StateItem(const QString &name, bool isEnd = false);

    // ---------- QGraphicsItem interface ----------
    QRectF  boundingRect() const override;
    void    paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;
    QVariant itemChange(GraphicsItemChange, const QVariant &) override;

    // ---------- Property setters ----------
    /**
     * @brief Marks this state as active.
     * @param a True if active.
     */
    void setActive(bool a) { m_active = a; update(); }

    /**
     * @brief Sets whether this state is an end state.
     * @param end True to mark as end state.
     */
    void setEndState(bool end);

    /**
     * @brief Enables or disables a breakpoint on this state.
     * @param bp True to enable breakpoint.
     */
    void setBreakPoint(bool bp);

    /**
     * @brief Sets the name of the state.
     * @param n New name.
     */
    void setName(const QString &n) { m_name = n; update(); }

    /**
     * @brief Sets the delay value in milliseconds.
     * @param d Delay duration.
     */
    void setDelayMs(int d) { m_delayMs = d; update(); }

    /**
     * @brief Marks this state as the start state.
     * @param s True if this is the start state.
     */
    void setStart(bool s) { m_start = s; update(); }

    // ---------- Property getters ----------
    QString name()          const { return m_name; }
    bool    isEndState()    const { return m_endState; }
    bool    isBreakPoint()  const { return m_breakpoint; }
    bool    isActive()      const { return m_active; }
    int     delayMs()       const { return m_delayMs; }
    bool    isStart()       const { return m_start; }

    // ---------- Geometry helpers ----------
    /**
     * @brief Calculates the outer radius based on visual flags (end, breakpoint).
     * @return Outer radius.
     */
    qreal outerRadius() const;

    /**
     * @brief Returns the center point of the item in scene coordinates.
     * @return Scene center.
     */
    QPointF centerPoint() const { return mapToScene(QPointF(0, 0)); }

    // ---------- Transition bookkeeping ----------
    /**
     * @brief Adds an outgoing transition to this state.
     * @param t Pointer to the transition.
     */
    void addOutgoingTransition(TransitionItem *t);

    /**
     * @brief Adds an incoming transition to this state.
     * @param t Pointer to the transition.
     */
    void addIncomingTransition(TransitionItem *t);

    /**
     * @brief Removes an outgoing transition from this state.
     * @param t Pointer to the transition.
     */
    void removeOutgoingTransition(TransitionItem *t);

    /**
     * @brief Removes an incoming transition from this state.
     * @param t Pointer to the transition.
     */
    void removeIncomingTransition(TransitionItem *t);

    /**
     * @brief Checks if this state has an outgoing transition to a given destination state.
     * @param dest Destination state.
     * @return True if transition exists, false otherwise.
     */
    bool hasOutgoingTo(StateItem *dest) const;

    /**
     * @brief Returns the number of outgoing transitions.
     * @return Outgoing transition count.
     */
    int outgoingCount() const { return m_outgoing.size(); }

    /**
     * @brief Returns the number of incoming transitions.
     * @return Incoming transition count.
     */
    int incomingCount() const { return m_incoming.size(); }

    /**
     * @brief Returns a list of incoming transitions.
     * @return Const reference to incoming transitions.
     */
    const QVector<TransitionItem*>& incoming() const { return m_incoming; }

private:
    /**
     * @brief Updates connected transitions after state movement.
     */
    void syncConnectedTransitions();

    QString m_name;                         ///< State name.
    bool    m_active     {false};           ///< Is this state currently active?
    bool    m_endState   {false};           ///< Is this an end state?
    bool    m_breakpoint {false};           ///< Is this a breakpoint?
    bool    m_start      {false};           ///< Is this the start state?

    QVector<TransitionItem*> m_outgoing;    ///< List of outgoing transitions.
    QVector<TransitionItem*> m_incoming;    ///< List of incoming transitions.
};

#endif // STATEITEM_H
