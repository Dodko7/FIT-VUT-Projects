#ifndef AUTOMATONMAINWINDOW_H
#define AUTOMATONMAINWINDOW_H

#include "startcreationdialog.h"
#include "fsminspectorwidget.h"

#include <QMainWindow>
#include <QPushButton>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QStackedWidget>
#include "transitioninspectorwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class AutomatonMainWindow;
}
QT_END_NAMESPACE

class AutomatonEditor;
class StateInspectorWidget;
class QPlainTextEdit;
class QLineEdit;
class QToolBar;
class QLabel;

/**
 * @author Adam Taha (xtahaa00)
 * @file automatonmainwindow.h
 * @brief Declares the AutomatonMainWindow class, which represents the main window for the automaton editor.
 *        It manages the FSM editor view, state and transition inspectors, toolbars, and docked widgets.
 */

/**
 * @class AutomatonMainWindow
 * @brief The main application window containing the FSM canvas and UI for editing and inspecting automata.
 */
class AutomatonMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the AutomatonMainWindow.
     * @param parent Optional parent widget.
     */
    explicit AutomatonMainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~AutomatonMainWindow() override;

private slots:
    /**
     * @brief Slot to handle running the FSM (placeholder).
     */
    void runAutomaton();

    /**
     * @brief Slot to show/hide the debug panel.
     */
    void debugAutomaton();

    /**
     * @brief Slot to display application help in the log.
     */
    void showHelp();

    /**
     * @brief Slot to trigger state creation.
     */
    void addState();

    /**
     * @brief Slot to trigger transition creation dialog.
     */
    void addTransition();

    /**
     * @brief Slot to undo the last state or transition deletion.
     */
    void undoLast();

    /**
     * @brief Slot to create a new FSM via dialog.
     */
    void addFsm();

    /**
     * @brief Slot to load FSM from a file.
     */
    void loadFsm();

    /**
     * @brief Slot to save FSM to a file (stub).
     */
    void saveFsm();

private:
    /**
     * @brief Forwards a key press and release event to the FSM editor.
     * @param key Key code to simulate.
     */
    void forwardKeyToEditor(Qt::Key key);

    Ui::AutomatonMain
