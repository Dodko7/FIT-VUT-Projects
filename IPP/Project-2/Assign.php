<?php

namespace IPP\Student;

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
use IPP\Student\Expression\Variable;

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
 * Represents an assignment operation in the SOL25 interpreter.
 *
 * This class handles the assignment of a value (from evaluating an expression) to a variable
 * within the execution context.
 */
class Assign
{
    private int $order; // The execution order of the assignment
    private string $variable; // The name of the variable to assign
    private Expression $expression; // The expression to evaluate and assign to the variable

    /**
     * Constructor for Assign.
     *
     * Initializes the Assign instance with the interpreter for debugging purposes.
     *
     * @param Interpreter $interpreter The interpreter instance for I/O and debugging.
     */
    public function __construct(private Interpreter $interpreter)
    {
    }

    /**
     * Parse the assignment from a DOM element.
     *
     * @param DOMElement $element The <assign> element from XML.
     * @throws InterpretTypeException If the structure is invalid or required attributes are missing.
     */
    public function parse(DOMElement $element): void
    {

        // Get the execution order of the assignment
        $this->order = (int)$element->getAttribute('order');

        // Find the <var> element for the variable name
        $varNode = null;
        foreach ($element->childNodes as $node) {
            if ($node instanceof \DOMElement && $node->nodeName === 'var') {
                $varNode = $node;
                break;
            }
        }
        if ($varNode === null) {
            throw new InterpretTypeException('Missing <var> element in assign');
        }


        // Extract variable name
        $this->variable = $varNode->getAttribute('name');
        if (empty($this->variable)) {
            throw new InterpretTypeException('Missing name attribute in <var> element');
        }

        // Find and parse the <expr> element
        foreach ($element->childNodes as $node) {
            if ($node instanceof \DOMElement && $node->nodeName === 'expr') {
                $this->expression = Expression::createFromXML($node, $this->interpreter);
                break;
            }
        }

        if (!isset($this->expression)) {
            throw new InterpretTypeException('Missing <expr> element in assign');
        }

    }

    /**
     * Get the execution order of the assignment.
     *
     * @return int The value of the 'order' attribute.
     */
    public function getOrder(): int
    {
        return $this->order;
    }

    /**
     * Execute the assignment within a given context.
     *
     * For the 'print' message, assigns the receiver's value to the variable but evaluates the
     * message to perform side effects. For other messages or expressions, assigns the evaluated
     * expression value.
     *
     * @param Context $context The runtime context.
     * @return SolValue The value that was assigned.
     */
    public function execute(Context $context): SolValue
    {

        // Handle Send expressions specially for assignments like `var := receiver print`
        if ($this->expression instanceof Send && $this->expression->getSelector() === 'print') {
            // Get the receiver's value for assignment
            $receiverValue = $this->expression->getReceiverValue($context);
            // Evaluate the message to perform side effects (e.g., print)
            $this->expression->evaluate($context);
            // Assign the receiver's value to the variable
            $context->setVariable($this->variable, $receiverValue);
            // Return the assigned value
            return $receiverValue;
        }

        // For other expressions or Send messages, evaluate and assign the result
        $value = $this->expression->evaluate($context);

        // Assign the evaluated value to the variable
        $context->setVariable($this->variable, $value);

        // Return the value that was assigned
        return $value;
    }
}