<?php

namespace IPP\Student\Expression;

use IPP\Student\Assign;
use IPP\Student\Block;
use IPP\Student\Context;
use IPP\Student\Frame;
use IPP\Student\Interpreter;
use IPP\Student\Method;
use IPP\Student\Program;
use IPP\Student\SolClass;

use IPP\Student\Exception\InterpretDNUException;
use IPP\Student\Exception\InterpretTypeException;
use IPP\Student\Exception\InterpretValueException;

use IPP\Student\Expression\BlockExpr;
use IPP\Student\Expression\Expression;
use IPP\Student\Expression\Literal;
use IPP\Student\Expression\Send;

use IPP\Student\Value\BlockValue;
use IPP\Student\Value\BoolValue;
use IPP\Student\Value\IntegerValue;
use IPP\Student\Value\NilValue;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Value\StringValue;

use DOMDocument;
use DOMElement;

use IPP\Core\AbstractInterpreter;
use IPP\Core\Exception\XMLException;
use IPP\Core\Interface\InputReader;
use IPP\Core\Interface\OutputWriter;

/**
 * Represents a variable expression in the SOL25 interpreter.
 *
 * This class encapsulates a variable reference, allowing the interpreter to evaluate
 * the variable's value from the execution context.
 */
class Variable extends Expression
{
    /** @var string The name of the variable */
    private string $name;

    /**
     * Constructor for Variable.
     *
     * Initializes a Variable expression by parsing the variable name from the provided
     * DOM element.
     *
     * @param Interpreter $interpreter The interpreter instance for I/O and debugging.
     * @param DOMElement $element The DOM element representing the variable (with 'name' attribute).
     */
    public function __construct(Interpreter $interpreter, DOMElement $element)
    {
        parent::__construct($interpreter);
        $this->name = $element->getAttribute('name');
        // [Variable.php]: Log creation of Variable
    }

    /**
     * Evaluate the variable in the given context.
     *
     * Retrieves the value of the variable from the context. Throws an exception if the
     * variable is undefined.
     *
     * @param Context $context The execution context containing variable bindings.
     * @return SolValue The value of the variable.
     * @throws InterpretTypeException If the variable is undefined.
     */
    public function evaluate(Context $context): SolValue
    {
        // [Variable.php]: Log variable evaluation
        $value = $context->getVariable($this->name);
        if ($value === null) {
            // [Variable.php]: Log undefined variable
            throw new InterpretTypeException('Undefined variable: ' . $this->name);
        }
        // [Variable.php]: Log retrieved value
        return $value;
    }
}