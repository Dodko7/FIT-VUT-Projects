/**
 * @author Adam Taha (xtahaa00)
 * @file stateitem.cpp
 * @brief Defines the StateItem class, which represents a state node in the
 *        automaton's scene. Supports painting, movement, and transition bookkeeping.
 */

#include "stateitem.h"
#include "transitionitem.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QFont>
#include <QtMath>

/**
 * @brief Constructs a StateItem with a name and end-state flag.
 * @param name Name of the state.
 * @param isEnd True if the state is an end state.
 */
StateItem::StateItem(const QString &name, bool isEnd)
    : m_name(name), m_endState(isEnd)
{
    setFlags(ItemIsMovable | ItemIsSelectable);
    setCacheMode(DeviceCoordinateCache);
}

/**
 * @brief Calculates the outer radius of the state circle, including visual decorators.
 * @return Outer radius in scene units.
 */
qreal StateItem::outerRadius() const
{
    qreal r = BASE_RADIUS;
    if (m_endState)
        r += END_GAP + RING_WIDTH;     // gap + one outer ring
    else if (m_breakpoint)
        r += RING_WIDTH;               // only breakpoint ring
    return r;
}

/**
 * @brief Returns the bounding rectangle of the item for scene layout.
 * @return Bounding rectangle in scene coordinates.
 */
QRectF StateItem::boundingRect() const
{
    const qreal r   = outerRadius();
    const qreal pad = RING_WIDTH;      // extra for stroke
    return {-r-pad, -r-pad, 2*(r+pad), 2*(r+pad)};
}

/**
 * @brief Paints the visual representation of the state node.
 * 
 * States can be drawn with different borders depending on:
 * - whether it's a start, end, or active state
 * - whether it has a breakpoint
 * 
 * @param p Painter used to draw the item.
 * @param option Style options (unused).
 * @param widget Optional widget this is being painted onto.
 */
void StateItem::paint(QPainter *p,
                      const QStyleOptionGraphicsItem *,
                      QWidget *)
{
    p->setRenderHint(QPainter::Antialiasing, true);

    const QColor fill = m_start ? QColor(64,224,208)
                        : m_active ? Qt::green
                                   : Qt::lightGray;

    const qreal  outerR = outerRadius();

    // --- end-state gap (fill colour) ---
    if (m_endState) {
        p->setPen(Qt::NoPen);
        p->setBrush(fill);
        p->drawEllipse(QPointF(), BASE_RADIUS + END_GAP,
                       BASE_RADIUS + END_GAP);
    }

    // --- outer rings ---
    if (m_breakpoint) {
        p->setPen(QPen(Qt::red, 2, Qt::DashLine));
        p->setBrush(Qt::NoBrush);
        p->drawEllipse(QPointF(), outerR - RING_WIDTH/2,
                       outerR - RING_WIDTH/2);
    }
    if (m_endState && !m_breakpoint) {
        p->setPen(QPen(Qt::black, 2));
        p->setBrush(Qt::NoBrush);
        p->drawEllipse(QPointF(), outerR - RING_WIDTH/2,
                       outerR - RING_WIDTH/2);
    }

    // --- inner ring ---
    p->setPen(QPen(Qt::black, 2));
    p->setBrush(fill);
    p->drawEllipse(QPointF(), BASE_RADIUS, BASE_RADIUS);

    // --- label ---
    p->setPen(Qt::black);
    p->setFont(QFont("Arial", 10, QFont::Bold));
    const QRectF text(-BASE_RADIUS, -10, BASE_RADIUS*2, 20);
    p->drawText(text, Qt::AlignCenter, m_name);
}

/**
 * @brief Sets whether the state is an end state and updates visuals.
 * @param end True to set as end state.
 */
void StateItem::setEndState(bool end)
{
    if (m_endState == end) return;
    m_endState = end;
    prepareGeometryChange();
    syncConnectedTransitions();
    update();
}

/**
 * @brief Sets or clears the breakpoint flag and updates visuals.
 * @param bp True to enable breakpoint.
 */
void StateItem::setBreakPoint(bool bp)
{
    if (m_breakpoint == bp) return;
    m_breakpoint = bp;
    prepareGeometryChange();
    syncConnectedTransitions();
    update();
}

/**
 * @brief Adds an outgoing transition to the list.
 * @param t TransitionItem to add.
 */
void StateItem::addOutgoingTransition(TransitionItem *t)
{
    m_outgoing.append(t);
}

/**
 * @brief Adds an incoming transition to the list.
 * @param t TransitionItem to add.
 */
void StateItem::addIncomingTransition(TransitionItem *t)
{
    m_incoming.append(t);
}

/**
 * @brief Removes an outgoing transition from the list.
 * @param t TransitionItem to remove.
 */
void StateItem::removeOutgoingTransition(TransitionItem *t)
{
    m_outgoing.removeAll(t);
}

/**
 * @brief Removes an incoming transition from the list.
 * @param t TransitionItem to remove.
 */
void StateItem::removeIncomingTransition(TransitionItem *t)
{
    m_incoming.removeAll(t);
}

/**
 * @brief Checks whether the state has an outgoing transition to a specific destination.
 * @param dest Destination state.
 * @return True if such a transition exists, false otherwise.
 */
bool StateItem::hasOutgoingTo(StateItem *dest) const
{
    for (auto *t : m_outgoing)
        if (t->dest() == dest) return true;
    return false;
}

/**
 * @brief Updates all connected transitions to reflect new position/geometry.
 */
void StateItem::syncConnectedTransitions()
{
    for (auto *tr : std::as_const(m_outgoing))  tr->updatePosition();
    for (auto *tr : std::as_const(m_incoming))  tr->updatePosition();
}

/**
 * @brief Handles item movement changes to update connected transitions.
 * @param change Type of change.
 * @param v New value.
 * @return Value to propagate to base class.
 */
QVariant StateItem::itemChange(GraphicsItemChange change, const QVariant &v)
{
    if (change == ItemPositionHasChanged)
    {
        syncConnectedTransitions();
        m_savedPos = v.toPointF(); // Remember GUI coordinates
    }
    return QGraphicsObject::itemChange(change, v);
}
