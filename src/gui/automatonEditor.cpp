/**
 * @author Adam Taha (xtahaa00)
 * @file automatonEditor.cpp
 * @brief Implementation of AutomatonEditor, a custom QGraphicsView used for visualizing and manipulating FSMs.
 */

#include "automatonEditor.h"
#include "stateitem.h"
#include "transitionitem.h"
#include "statecreationdialog.h"
#include "startcreationdialog.h"

#include <QGraphicsScene>
#include <QResizeEvent>
#include <QtMath>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug> // For backend save request logging

AutomatonEditor::AutomatonEditor(QWidget *parent)
    : QGraphicsView(parent)
    , m_scene(new QGraphicsScene(this))
{
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(RubberBandDrag);
    viewport()->setMouseTracking(true);
    fitScene();
}

/**
 * @brief Opens dialog and adds a new state to the canvas with optional end flag.
 * @param name Name of the state (used if dialog input is empty).
 * @param isEnd Whether the state is an accepting state.
 */
void AutomatonEditor::addState(const QString &name, bool isEnd)
{
    StateCreationDialog dlg(this);
    if(dlg.exec() == QDialog::Rejected) return;

    auto *state = new StateItem(dlg.name().isEmpty()
                                    ? "q" + QString::number(m_states.size())
                                    : dlg.name(),
                                dlg.isEnd());
    state->setDelayMs(dlg.delay());

    const double r = 80 + m_states.size() * 20.0;
    const double a = m_states.size() * 0.5;
    state->setPos(r * std::cos(a), r * std::sin(a));
    m_states << state;
    m_scene->addItem(state);

    fitScene();
}

/**
 * @brief Inserts a new state at a specific scene position.
 * @param name State name
 * @param isEnd Whether the state is accepting
 * @param delayMs Delay in milliseconds
 * @param pos Position on the scene
 * @return Pointer to created StateItem
 */
StateItem* AutomatonEditor::insertState(const QString& name, bool isEnd, int delayMs, const QPointF& pos)
{
    auto *state = new StateItem(name, isEnd);
    state->setDelayMs(delayMs);
    state->setPos(pos);
    m_scene->addItem(state);
    m_states << state;
    fitScene();
    return state;
}

/**
 * @brief Launches dialog to insert a start state with default styling and flags.
 * @param scenePos Position on the scene to place the start state.
 */
void AutomatonEditor::createStartState(const QPointF &scenePos)
{
    StartStateCreationDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    auto *start = insertState(dlg.name().isEmpty() ? "start" : dlg.name(),
                              false,
                              dlg.delay(),
                              scenePos);
    start->setStart(true);
    emit stateSelected(start);
}

/**
 * @brief Adjusts the view to ensure the full scene is visible.
 */
void AutomatonEditor::fitScene()
{
    if(m_scene->items().isEmpty()) return;
    QRectF b = m_scene->itemsBoundingRect();
    b.adjust(-100, -100, 100, 100);
    fitInView(b, Qt::KeepAspectRatio);
}

/**
 * @brief Resizes the view and re-fits the scene.
 */
void AutomatonEditor::resizeEvent(QResizeEvent *e)
{
    QGraphicsView::resizeEvent(e);
    fitScene();
}

/**
 * @brief Removes all items and clears internal state.
 */
void AutomatonEditor::clearAutomaton()
{
    m_scene->clear();
    m_states.clear();
    deletedStates.clear();
}

/**
 * @brief Handles keyboard input for mode switching, undo, and deletion.
 * @param e Key event
 */
void AutomatonEditor::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_A:
        m_placeState = true;
        setCursor(Qt::PointingHandCursor);
        return;
    case Qt::Key_T:
        emit requestTransitionDialog();
        return;
    case Qt::Key_Escape:
        if(m_placeState) { m_placeState = false; unsetCursor(); }
        return;
    case Qt::Key_Z:
        if(!deletedStates.isEmpty())
        {
            auto *s = deletedStates.pop();
            m_scene->addItem(s);
            m_states << s;
            s->setSelected(false);
            fitScene();
        }
        return;
    case Qt::Key_Delete:
    case Qt::Key_Backspace: {
        const auto items = m_scene->selectedItems();
        const int count = items.size();
        for (int i = 0; i < count; ++i) {
            QGraphicsItem *it = items.at(i);
            if (auto *s = qgraphicsitem_cast<StateItem*>(it)) {
                deletedStates.push(s);
                m_states.removeOne(s);
                m_scene->removeItem(s);
            } else if (auto *t = qgraphicsitem_cast<TransitionItem*>(it)) {
                deletedTransitions.push(t);
                t->source()->removeOutgoingTransition(t);
                t->dest()->removeIncomingTransition(t);
                m_scene->removeItem(t);
            }
        }
        return;
    }
    default: break;
    }

    QGraphicsView::keyPressEvent(e);
}

/**
 * @brief Handles mouse click to select or place states and transitions.
 * @param e Mouse event
 */
void AutomatonEditor::mousePressEvent(QMouseEvent *e)
{
    const QPointF scenePos = mapToScene(e->pos());
    QGraphicsItem *hit = m_scene->itemAt(scenePos, QTransform());
    TransitionItem *transHit = qgraphicsitem_cast<TransitionItem*>(hit);
    StateItem *stateHit = (!transHit ? qgraphicsitem_cast<StateItem*>(hit) : nullptr);

    if(m_placeState)
    {
        if(!hit)
        {
            StateCreationDialog dlg(this);
            if(dlg.exec() == QDialog::Accepted)
            {
                insertState(dlg.name().isEmpty()
                            ? "q" + QString::number(m_states.size())
                            : dlg.name(),
                            dlg.isEnd(),
                            dlg.delay(),
                            scenePos);
                fitScene();
            }
        }
        m_placeState = false;
        unsetCursor();
        return;
    }

    if(transHit)
        emit transitionSelected(transHit);
    else if(stateHit && m_scene->items().contains(stateHit))
        emit stateSelected(stateHit);
    else {
        emit stateSelected(nullptr);
        emit transitionSelected(nullptr);
    }

    QGraphicsView::mousePressEvent(e);
}

void AutomatonEditor::mouseMoveEvent(QMouseEvent *e)
{
    QGraphicsView::mouseMoveEvent(e);
}

void AutomatonEditor::mouseReleaseEvent(QMouseEvent *e)
{
    QGraphicsView::mouseReleaseEvent(e);
}

/**
 * @brief Programmatically selects and centers a state in the view.
 * @param s State to select
 */
void AutomatonEditor::selectState(StateItem *s)
{
    if(!s) return;
    scene()->clearSelection();
    s->setSelected(true);
    centerOn(s);
    emit stateSelected(s);
}

/**
 * @brief Returns the FSM's start state.
 * @return Pointer to start StateItem, or nullptr if none set
 */
StateItem* AutomatonEditor::startState() const
{ for(auto *s: m_states) if(s->isStart()) return s; return nullptr; }

/**
 * @brief Returns the currently active state in the FSM.
 * @return Pointer to active StateItem, or nullptr
 */
StateItem* AutomatonEditor::activeState() const
{ for(auto *s: m_states) if(s->isActive()) return s; return nullptr; }

/**
 * @brief Creates and adds a transition between two states.
 * @param fromName Source state name
 * @param toName Destination state name
 * @param output Output label
 * @param condition Condition label
 */
void AutomatonEditor::createTransition(const QString &fromName,
                                       const QString &toName,
                                       const QString &output,
                                       const QString &condition)
{
    StateItem *src = nullptr, *dst = nullptr;
    for (auto *st : m_states) {
        if (st->name() == fromName) src = st;
        if (st->name() == toName)   dst = st;
    }
    if (!src || !dst) return;
    auto *arrow = new TransitionItem(src, dst, output, condition);
    m_scene->addItem(arrow);
    arrow->updatePosition();
}

/**
 * @brief Saves the specified state to backend.
 * @param state Pointer to the StateItem to save.
 *
 * Emits save request.
 */
void AutomatonEditor::saveState(StateItem* state)
{
    if (!state) return;
    qDebug() << "[Save State] connect to backend";
}

/**
 * @brief Saves the specified transition to backend.
 * @param transition Pointer to the TransitionItem to save.
 *
 * Emits save request.
 */
void AutomatonEditor::saveTransition(TransitionItem* transition)
{
    if (!transition) return;
    qDebug() << "[Save Transition] connect to backend";
}

/**
 * @brief Saves the entire FSM structure to backend.
 *
 * Emits save request.
 */
void AutomatonEditor::saveFsmStructure()
{
    qDebug() << "[Save FSM] connect to backend";
}
