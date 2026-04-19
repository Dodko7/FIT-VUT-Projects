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
 * Abstract base class for expressions in the SOL25 language.
 *
 * Defines a common interface for all expression types (e.g., Variable, Send, Literal, BlockExpr)
 * and provides a factory method to create expressions from XML DOM elements.
 */
abstract class Expression
{
    /**
     * Constructs an expression instance.
     *
     * Initializes the expression with the provided interpreter instance for I/O and debugging.
     *
     * @param Interpreter $interpreter The interpreter instance for I/O and debugging operations.
     */
    public function __construct(protected readonly Interpreter $interpreter)
    {
    }

    /**
     * Evaluates the expression in the given context.
     *
     * Subclasses must implement this method to provide the evaluation logic for their specific
     * expression type.
     *
     * @param Context $context The runtime context containing variable bindings and class information.
     * @return SolValue The result of evaluating the expression.
     */
    abstract public function evaluate(Context $context): SolValue;

    /**
     * Creates an expression object from a DOM element.
     *
     * Factory method that instantiates the appropriate Expression subclass based on the DOM element's
     * node name (e.g., literal, var, send, block).
     *
     * @param DOMElement $element The DOM element representing the expression.
     * @param Interpreter $interpreter The interpreter instance for I/O and debugging.
     * @return Expression The created expression object.
     * @throws InterpretTypeException If the expression type is unknown, invalid, or the expr node is empty.
     */
    public static function createFromXML(DOMElement $element, Interpreter $interpreter): Expression
    {
        $type = $element->nodeName;

        // Handle <expr> nodes by processing their first child element
        if ($type === 'expr') {
            foreach ($element->childNodes as $node) {
                if ($node instanceof DOMElement) {
                    return self::createFromXML($node, $interpreter);
                }
            }
            throw new InterpretTypeException('Empty expr node');
        }

        // Instantiate the appropriate expression object
        $expression = match ($type) {
            'literal' => self::createLiteral($element, $interpreter),
            'var' => new Variable($interpreter, $element),
            'send' => new Send($interpreter, $element),
            'block' => new BlockExpr($interpreter, $element),
            default => null
        };

        if ($expression === null) {
            throw new InterpretTypeException("Unknown expression type: $type");
        }

        return $expression;
    }

    /**
     * Creates a literal expression from a DOM element.
     *
     * Parses the class and value attributes from the DOM element to instantiate a Literal expression.
     *
     * @param DOMElement $element The DOM element representing the literal expression.
     * @param Interpreter $interpreter The interpreter instance for I/O and debugging.
     * @return Literal The created literal expression.
     * @throws InterpretTypeException If required attributes are missing or invalid.
     */
    private static function createLiteral(DOMElement $element, Interpreter $interpreter): Literal
    {
        // Validate the class attribute
        if (!$element->hasAttribute('class')) {
            throw new InterpretTypeException('Missing class attribute in literal element');
        }
        $class = $element->getAttribute('class');
        if ($class === '') {
            throw new InterpretTypeException('Empty class attribute in literal element');
        }

        // Validate the value attribute
        if (!$element->hasAttribute('value')) {
            throw new InterpretTypeException('Missing value attribute in literal element');
        }
        $value = $element->getAttribute('value');

        return new Literal($class, $value, $interpreter);
    }
}