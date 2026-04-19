#ifndef SCRIPT_ENGINE_HPP
#define SCRIPT_ENGINE_HPP

#include <QScriptEngine>
#include <QScriptValue>
#include <QString>
#include <QVariant>
#include <unordered_map>
#include <string>

// Forward deklarácia triedy FSM
class FSM;

class ScriptEngine : public QObject {
    Q_OBJECT
public:
    explicit ScriptEngine(FSM& fsm, QObject* parent = nullptr);
    
    /**
     * @brief Inicializuje JavaScript kontext s premennými a funkciami.
     */
    void initialize();
    
    /**
     * @brief Vykoná JavaScript akciu stavu.
     * @param action JavaScript kód akcie.
     * @throws InvalidScriptException Ak akcia obsahuje chybu.
     */
    void executeAction(const std::string& action);
    
    /**
     * @brief Vyhodnotí JavaScript podmienku prechodu.
     * @param condition JavaScript kód podmienky.
     * @return True, ak je podmienka splnená.
     * @throws InvalidScriptException Ak podmienka obsahuje chybu alebo nevracia bool.
     */
    bool evaluateCondition(const std::string& condition);
    
    /**
     * @brief Synchronizuje premenné medzi FSM a JavaScript kontextom.
     */
    void syncVariables();

public Q_SLOTS:
    /**
     * @brief Nastaví hodnotu premennej.
     * @param name Názov premennej.
     * @param value Nová hodnota.
     */
    void output(const QString& name, const QVariant& value);
    
    /**
     * @brief Vráti hodnotu premennej alebo vstupu.
     * @param name Názov premennej alebo 'in' pre vstup.
     * @return Hodnota premennej/vstupu alebo undefined.
     */
    QVariant valueof(const QString& name);
    
    /**
     * @brief Konvertuje reťazec na celé číslo.
     * @param str Reťazec na konverziu.
     * @return Celé číslo.
     * @throws InvalidScriptException Ak konverzia zlyhá.
     */
    int atoi(const QString& str);
    
    /**
     * @brief Vráti čas od vstupu do aktuálneho stavu (v ms).
     * @return Čas v milisekundách.
     */
    qint64 elapsed();

private:
    FSM& fsm; // Referencia na FSM
    QScriptEngine engine; // JavaScript engine
};

#endif // SCRIPT_ENGINE_HPP