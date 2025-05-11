#include "script_engine.hpp"
#include "fsm.hpp"
#include "fsmErrors.hpp"
#include <QScriptValueIterator>
#include <stdexcept>

ScriptEngine::ScriptEngine(FSM& fsm, QObject* parent)
    : QObject(parent), fsm(fsm) {
    initialize();
}

void ScriptEngine::initialize() {
    // Register JavaScript functions
    QScriptValue outputFunc = engine.newQObject(this);
    engine.globalObject().setProperty("ScriptEngine", outputFunc);
    
    // output function
    engine.evaluate(
        "function output(name, value) { ScriptEngine.output(name, value); }"
    );
    
    // valueof function
    engine.evaluate(
        "function valueof(name) { return ScriptEngine.valueof(name); }"
    );
    
    // atoi function
    engine.evaluate(
        "function atoi(str) { return ScriptEngine.atoi(str); }"
    );
    
    // elapsed function
    engine.evaluate(
        "function elapsed() { return ScriptEngine.elapsed(); }"
    );
    
    // Initialize variables
    syncVariables();
}

void ScriptEngine::executeAction(const std::string& action) {
    if (action.empty()) return;
    
    // Sync variables before action
    syncVariables();
    
    // Execute action
    QScriptValue result = engine.evaluate(QString::fromStdString(action));
    if (result.isError()) {
        throw InvalidScriptException(
            "Action error: " + result.toString().toStdString()
        );
    }
    
    // Sync variables after action
    syncVariables();
}

bool ScriptEngine::evaluateCondition(const std::string& condition) {
    if (condition.empty()) return true; // Empty condition is always true
    
    // Sync variables
    syncVariables();
    
    // Evaluate condition
    QScriptValue result = engine.evaluate(QString::fromStdString(condition));
    if (result.isError()) {
        throw InvalidScriptException(
            "Condition error: " + result.toString().toStdString()
        );
    }
    
    if (!result.isBool()) {
        throw InvalidScriptException(
            "Condition must return boolean, got: " + result.toString().toStdString()
        );
    }
    
    return result.toBool();
}

void ScriptEngine::syncVariables() {
    QScriptValue global = engine.globalObject();
    
    // From FSM to JavaScript
    for (const auto& [name, value] : fsm.getVariables()) {
        global.setProperty(
            QString::fromStdString(name),
            engine.toScriptValue(QString::fromStdString(value))
        );
    }
    
    // From JavaScript to FSM
    for (const auto& [name, _] : fsm.getVariables()) {
        QScriptValue value = global.property(QString::fromStdString(name));
        if (value.isUndefined()) continue;
        fsm.addVariable(name, value.toString().toStdString(), true); // Add overwrite=true flag
    }
}

void ScriptEngine::output(const QString& name, const QVariant& value) {
    std::string nameStr = name.toStdString();
    std::string valueStr = value.toString().toStdString();
    
    // Update variable with overwrite=true to allow updating existing variables
    fsm.addVariable(nameStr, valueStr, true);
}

QVariant ScriptEngine::valueof(const QString& name) {
    std::string nameStr = name.toStdString();
    
    // If it's 'in' input, return current input character
    if (nameStr == "in" && !fsm.getInput().empty()) {
        return QString(fsm.getInput()[0]);
    }
    
    // Otherwise return variable value
    auto vars = fsm.getVariables();
    auto it = vars.find(nameStr);
    if (it != vars.end()) {
        return QString::fromStdString(it->second);
    }
    
    return QVariant(); // Non-existent variable
}

int ScriptEngine::atoi(const QString& str) {
    bool ok;
    int result = str.toInt(&ok);
    if (!ok) {
        throw InvalidScriptException("atoi: Invalid number: " + str.toStdString());
    }
    return result;
}

qint64 ScriptEngine::elapsed() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - fsm.getCurrentStateEntryTime()
    ).count();
    return duration;
}