/**
 * @author Adam Taha (xtahaa00)
 * @file transitionitem.h
 * @brief Declaration of the TransitionItem class, which visually represents a transition (arrow) between two states in the FSM.
 */

#ifndef TRANSITIONITEM_H
#define TRANSITIONITEM_H

#include <QGraphicsItem>

class StateItem;

/**
 * @class TransitionItem
 * @brief Draws (and keeps updated) an arrow between two StateItem nodes.
 *
 * The arrow automatically tracks state moves and resizing. Only one transition per direction is guaranteed.
 */
class TransitionItem : public QGraphicsItem
{
public:
    /**
     * @brief Constructs a TransitionItem between two states.
     * @param source Pointer to the source StateItem.
     * @param dest Pointer to the destination StateItem.
     * @param output Output string label (optional).
     * @param condition Condition string label (optional).
     * @param parent Optional parent QGraphicsItem.
     */
    TransitionItem(StateItem *source,
                   StateItem *dest,
                   const QString &output = QString(),
                   const QString &condition = QString(),
                   QGraphicsItem *parent = nullptr);

    /**
     * @brief Returns the transition's output string.
     * @return Output string.
     */
    QString output() const { return m_output; }

    /**
     * @brief Returns the transition's condition string.
     * @return Condition string.
     */
    QString condition() const { return m_condition; }

    /**
     * @brief Returns a pointer to the source state.
     * @return Source StateItem pointer.
     */
    StateItem *source() const { return m_source; }

    /**
     * @brief Returns a pointer to the destination state.
     * @return Destination StateItem pointer.
     */
    StateItem *dest() const { return m_dest; }

    /**
     * @brief Refreshes the transition geometry when connected states move.
     */
    void updatePosition();

    // --- QGraphicsItem interface ---
    /**
     * @brief Returns the bounding rectangle of the transition item.
     * @return Bounding QRectF including the path and arrow.
     */
    QRectF boundingRect() const override;

    /**
     * @brief Paints the transition, including the arrow, arrowhead, and labels.
     * @param painter The painter to draw with.
     * @param option Style options (unused).
     * @param widget Optional widget pointer (unused).
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    StateItem *m_source {nullptr};     ///< Source state.
    StateItem *m_dest {nullptr};       ///< Destination state.

    QString m_output;                  ///< Output label shown on the arrow.
    QString m_condition;               ///< Condition label shown on the arrow.

    QPolygonF m_arrowHead;             ///< Points forming the arrowhead polygon.
    QPainterPath m_path;               ///< Path representing the arrow shaft.

    QPointF m_startPoint;              ///< Start point of the arrow.
    QPointF m_endPoint;                ///< End point of the arrow.

    QPointF m_condPos;                 ///< Anchor point for the condition label.
    QPointF m_outPos;                  ///< Anchor point for the output label.

    static constexpr qreal ARROW_SIZE = 10.0; ///< Constant size of the arrowhead.
};

#endif // TRANSITIONITEM_H
