/**
 * @author Adam Taha (xtahaa00)
 * @file main.cpp
 * @brief Entry point for the Automaton Editor application.
 *
 * Initializes the Qt application and displays the main window.
 */

#include "automatonmainwindow.h"
#include <QApplication>

/**
 * @brief Main function that starts the Qt application.
 * @param argc Argument count.
 * @param argv Argument values.
 * @return Application exit code.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AutomatonMainWindow w;
    w.show();
    return a.exec();
}
