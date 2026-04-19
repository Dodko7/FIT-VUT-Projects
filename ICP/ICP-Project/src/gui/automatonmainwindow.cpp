/**
 * @author Adam Taha (xtahaa00)
 * @file automatonmainwindow.cpp
 * @brief Main application window for the Automaton Editor.
 *        Coordinates the FSM canvas (AutomatonEditor) with UI components,
 *        such as inspectors, toolbars, and dock panels.
 */

#include "automatonmainwindow.h"
#include "ui_automatonmainwindow.h"

#include "automatonEditor.h"
#include "stateinspectorwidget.h"
#include "fsmcreationdialog.h"
#include "fsminspectorwidget.h"
#include "startcreationdialog.h"
#include "transitioninspectorwidget.h"
#include "transitioncreationdialog.h"

#include <QDockWidget>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QWidgetAction>
#include <QToolBar>
#include <QVBoxLayout>
#include <QIcon>
#include <QKeyEvent>
#include <QApplication>
#include <QStackedWidget>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QFileDialog>

/**
 * @brief Static help text displayed in the log panel.
 */
static const char *HELP_TEXT =
    "Automata Editor - Help\n"
    "-------------------------\n"
    "Canvas Hotkeys:\n"
    "  A - Add state mode\n"
    "  T - Add transition mode\n"
    "  ESC - cancel current mode\n"
    "  Del / Backspace - delete selected item\n"
    "  Z - undo last deletion\n"
    "  Workflow toolbar mimics A, T, Z with buttons.\n"
    "  --------------------------------------------\n"
    "  To Select a state or transition, click on it.\n"
    "  After selecting a state you can toggle End/Break in the inspector.\n"
    "  To add a transition click on the source state, then on the destination.\n"
    "  Use the dock panels on the left to enter input and read output.\n"
    "  The log panel will shows all non-output messages.\n";


/**
 * @brief Constructs the main window and initializes all UI components and inspectors.
 * @param parent The parent widget.
 */
AutomatonMainWindow::AutomatonMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AutomatonMainWindow)
{
    ui->setupUi(this);

    // Setup left-side docks: Input, Output, Log
    // Setup right-side stack: State / Transition / FSM Inspectors
    // Setup toolbar: state, transition, FSM, debug, I/O actions
    // Connect signals from editor to inspector widgets
    // Configure title bar and dock layouts
}

/**
 * @brief Destructor to clean up UI.
 */
AutomatonMainWindow::~AutomatonMainWindow()
{
    delete ui;
}

/**
 * @brief Logs the intent to run the FSM.
 */
void AutomatonMainWindow::runAutomaton()
{
    logEdit->appendPlainText("[Run] – Connect to backend.\n");
}

/**
 * @brief Sets up or tears down a debug panel for stepping through FSM execution.
 */
void AutomatonMainWindow::debugAutomaton()
{
    if (debugDock) {
        debugDock->hide();
        debugDock->deleteLater();
        debugDock = nullptr;
        return;
    }

    debugDock = new QDockWidget(tr("Debugger"), this);
    debugDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea);

    auto *panel = new QWidget(debugDock);
    auto *hl = new QHBoxLayout(panel);
    hl->setContentsMargins(8,8,8,8);

    auto *stepBtn = new QPushButton(tr("Step"), panel);
    auto *contBtn = new QPushButton(tr("Continue"), panel);

    hl->addWidget(stepBtn);
    hl->addWidget(contBtn);
    hl->addStretch();

    debugDock->setWidget(panel);
    addDockWidget(Qt::BottomDockWidgetArea, debugDock);

    connect(stepBtn, &QPushButton::clicked, [this] {
        logEdit->appendPlainText("[Step] – stub\n");
    });
    connect(contBtn, &QPushButton::clicked, [this] {
        logEdit->appendPlainText("[Continue] – stub\n");
    });
}

/**
 * @brief Shows the help information in the log panel.
 */
void AutomatonMainWindow::showHelp()
{
    logEdit->appendPlainText(HELP_TEXT);
}

/**
 * @brief Triggers state placement mode.
 */
void AutomatonMainWindow::addState()
{
    forwardKeyToEditor(Qt::Key_A);
}

/**
 * @brief Launches a dialog to create a new transition.
 */
void AutomatonMainWindow::addTransition()
{
    TransitionCreationDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted && editor) {
        editor->createTransition(
            dlg.fromState(),
            dlg.toState(),
            dlg.output(),
            dlg.condition()
        );
    }
}

/**
 * @brief Triggers undo for the last deleted state or transition.
 */
void AutomatonMainWindow::undoLast()
{
    forwardKeyToEditor(Qt::Key_Z);
}

/**
 * @brief Creates and configures a new FSM from user input via dialog.
 */
void AutomatonMainWindow::addFsm()
{
    FsmCreationDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    editor->clearAutomaton();
    editor->setStepDelay(dlg.stepDelay());
    editor->setDescription(dlg.description());
    editor->setAllowedInput(dlg.allowed());

    const QPointF center = editor->mapToScene(editor->viewport()->rect().center());
    editor->createStartState(center);

    nameEdit->setText(dlg.name());
    logEdit->appendPlainText("[FSM Created] " + dlg.name() + '\n');

    fsmInspector->refresh();
}

/**
 * @brief Opens file dialog to load FSM from file.
 */
void AutomatonMainWindow::loadFsm()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Load FSM"),
        tr("FSM Files (*.fsm);;All Files (*)")
    );
    if (fileName.isEmpty())
        return;

    logEdit->appendPlainText("[Load FSM] File selected: " + fileName);
    // TODO: Connect to backend parser
}

/**
 * @brief Saves the current FSM to file (stub).
 */
void AutomatonMainWindow::saveFsm()
{
    logEdit->appendPlainText("[Save FSM] – stub\n");
}

/**
 * @brief Forwards a key press and release event to the FSM editor.
 * @param key Qt::Key to simulate.
 */
void AutomatonMainWindow::forwardKeyToEditor(Qt::Key key)
{
    if (!editor) return;
    QKeyEvent press(QEvent::KeyPress, key, Qt::NoModifier);
    QKeyEvent rel(QEvent::KeyRelease, key, Qt::NoModifier);
    QApplication::sendEvent(editor, &press);
    QApplication::sendEvent(editor, &rel);
}
