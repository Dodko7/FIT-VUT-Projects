#include <catch.hpp>
#include <thread>  // For std::this_thread::sleep_for
#include "fsm.hpp"
#include "scriptEngine.hpp"
#include "fsmErrors.hpp"  // For InvalidScriptException

TEST_CASE("ScriptEngine evaluates conditions", "[ScriptEngine]") {
    FSM fsm;
    fsm.addVariable("in", "1");
    ScriptEngine engine(fsm);
    
    REQUIRE(engine.evaluateCondition("atoi(valueof('in')) == 1") == true);
    REQUIRE(engine.evaluateCondition("atoi(valueof('in')) == 0") == false);
    
    REQUIRE_THROWS_AS(
        engine.evaluateCondition("invalid code"),
        InvalidScriptException
    );
}

TEST_CASE("ScriptEngine executes actions", "[ScriptEngine]") {
    FSM fsm;
    fsm.addVariable("out", "0");
    ScriptEngine engine(fsm);
    
    engine.executeAction("output('out', '1');");
    REQUIRE(fsm.getVariables().at("out") == "1");
    
    REQUIRE_THROWS_AS(
        engine.executeAction("invalid code"),
        InvalidScriptException
    );
}

TEST_CASE("ScriptEngine handles timeouts", "[FSM]") {
    FSM fsm;
    fsm.addExpectedInput('i');
    fsm.addState("S1", "output('out', '0');", '0', false);
    fsm.addState("S2", "output('out', '1');", '1', true);
    fsm.setStartState("S1");
    fsm.addTransition("S1", "S2", "@ 100", 'i');
    fsm.setInput("i");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    fsm.run();
    
    REQUIRE(fsm.getCurrentState()->getName() == "S2");
    REQUIRE(fsm.getOutput() == "1");
}