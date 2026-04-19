/**
 * @author Adam Taha (xtahaa00)
 * @file transitionitem.cpp
 * @brief Defines the TransitionItem class, which visually represents an arrow
 *        between two StateItem nodes. Supports dynamic geometry and labels.
 */

#include "transitionitem.h"
#include "stateitem.h"

#include <QPainter>
#include <QtMath>

/**
 * @brief Constructs a TransitionItem between two states with specified output and condition.
 * @param source Pointer to the source StateItem.
 * @param dest Pointer to the destination StateItem.
 * @param output Output string label for the transition.
 * @param condition Condition string label for the transition.
 * @param parent Optional parent QGraphicsItem.
 */
TransitionItem::TransitionItem(StateItem *source,
                               StateItem *dest,
                               const QString &output,
                               const QString &condition,
                               QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_source(dest)
    , m_dest(source)
    , m_output(output)
    , m_condition(condition)
{
    setZValue(-1);                               // draw below states
    setFlags(QGraphicsItem::ItemIsSelectable);   // selectable / deletable

    if (m_source) m_source->addOutgoingTransition(this);
    if (m_dest)   m_dest  ->addIncomingTransition(this);
}

/**
 * @brief Helper function to calculate the point on a state’s border in the direction of a target point.
 * @param state Pointer to the state.
 * @param toward Target point in scene coordinates.
 * @return QPointF on the state’s edge.
 */
static QPointF edgePoint(const StateItem *state, const QPointF &toward)
{
    QLineF line(state->centerPoint(), toward);
    line.setLength(qMax(1.0, line.length()) - state->outerRadius());
    return line.p2();
}

/**
 * @brief Updates the visual geometry of the transition including path, arrowhead, and label positions.
 */
void TransitionItem::updatePosition()
{
    prepareGeometryChange();
    m_path.clear();
    m_arrowHead.clear();

    if (!m_source || !m_dest) return;

    // ---------- self-loop case ----------
    if (m_source == m_dest)
    {
        const qreal r = m_source->outerRadius();
        const qreal loop = r * 1.5;
        m_path.addEllipse(QPointF(r, -loop), loop, loop);
        return;
    }

    // ---------- straight arrow ----------
    const QPointF p1 = edgePoint(m_source, m_dest->centerPoint());
    const QPointF p2 = edgePoint(m_dest, m_source->centerPoint());

    m_startPoint = p1;
    m_endPoint = p2;

    m_path.moveTo(p1);
    m_path.lineTo(p2);

    QLineF line(p1, p2);
    const double ang = std::atan2(-line.dy(), line.dx());

    QPointF h1 = p2 - QPointF(std::cos(ang + M_PI / 6) * ARROW_SIZE,
                              std::sin(ang + M_PI / 6) * ARROW_SIZE);
    QPointF h2 = p2 - QPointF(std::cos(ang - M_PI / 6) * ARROW_SIZE,
                              std::sin(ang - M_PI / 6) * ARROW_SIZE);
    m_arrowHead << p2 << h1 << h2;

    // Label anchors (20% / 80% along the arrow)
    m_condPos = p1 + 0.20 * (p2 - p1);
    m_outPos  = p1 + 0.80 * (p2 - p1);
}

/**
 * @brief Returns the bounding rectangle of the transition for QGraphicsItem layout purposes.
 * @return Bounding rectangle including padding for arrowhead.
 */
QRectF TransitionItem::boundingRect() const
{
    constexpr qreal pad = ARROW_SIZE;
    return (m_path.isEmpty() ? QRectF{} : m_path.boundingRect()).adjusted(-pad, -pad, pad, pad);
}

/**
 * @brief Paints the transition on the QGraphicsScene, including path, arrowhead, and labels.
 * @param p QPainter to draw with.
 * @param option Style options (unused).
 * @param widget Optional widget pointer (unused).
 */
void TransitionItem::paint(QPainter *p,
                           const QStyleOptionGraphicsItem *,
                           QWidget *)
{
    updatePosition();                    // keep geometry fresh

    p->setRenderHint(QPainter::Antialiasing, true);

    // Draw arrow and head
    p->setPen(QPen(Qt::black, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p->setBrush(Qt::black);
    p->drawPath(m_path);
    if (!m_arrowHead.isEmpty())
        p->drawPolygon(m_arrowHead);

    // Draw labels
    p->setPen(Qt::white);
    p->setFont(QFont("Arial", 9));

    if (!m_condition.isEmpty())
        p->drawText(m_condPos, m_condition);

    if (!m_output.isEmpty())
        p->drawText(m_outPos,  m_output);
}
