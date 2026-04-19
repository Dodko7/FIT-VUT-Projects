<?php

namespace IPP\Student;

use IPP\Student\Assign;
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
 * Represents a block of code that can be executed with parameters.
 */
class Block
{
    private int $arity; // Number of parameters the block expects
    /** @var array<int, string> */
    private array $parameters = []; // Map of parameter order to names
    /** @var array<int, Assign> */
    private array $statements = []; // List of statements in the block

    public function __construct(private Interpreter $interpreter)
    {
    }

    /**
     * Parses a <block> element from XML and sets up its parameters and statements.
     *
     * @param DOMElement $element XML element representing the block
     * @throws InterpretTypeException if parameter count doesn't match arity or order is invalid
     */
    public function parse(DOMElement $element): void
    {
        $this->arity = (int)$element->getAttribute('arity');
        $this->parameters = [];
        $this->statements = [];

        // Process each child node of the <block> element
        foreach ($element->childNodes as $node) {
            if ($node instanceof \DOMElement) {
                if ($node->nodeName === 'parameter') {
                    $paramName = $node->getAttribute('name');
                    $order = (int)$node->getAttribute('order');
                    if (empty($paramName)) {
                        throw new InterpretTypeException('Missing parameter name');
                    }
                    if ($order < 1 || $order > $this->arity) {
                        throw new InterpretTypeException('Invalid parameter order');
                    }
                    $this->parameters[$order] = $paramName;
                } elseif ($node->nodeName === 'assign') {
                    $assign = new Assign($this->interpreter);
                    $assign->parse($node);
                    $this->statements[$assign->getOrder()] = $assign;
                }
            }
        }

        // Ensure statements are in correct execution order
        ksort($this->statements);

        // Check if the actual number of parameters matches expected arity
        if (count($this->parameters) !== $this->arity) {
            throw new InterpretTypeException('Parameter count does not match arity');
        }

        // Check if all required orders (1 to arity) are defined
        for ($i = 1; $i <= $this->arity; $i++) {
            if (!isset($this->parameters[$i])) {
                throw new InterpretTypeException('Missing parameter for order ' . $i);
            }
        }
    }

    /**
     * Returns the number of parameters this block expects.
     *
     * @return int
     */
    public function getArity(): int
    {
        return $this->arity;
    }

    /**
     * Executes the block with a given object receiver and a list of arguments.
     *
     * @param ObjectValue $receiver The object calling the block
     * @param array<SolValue> $arguments Argument values passed to the block
     * @param Context $context Current runtime context
     * @return SolValue Result of executing the block
     * @throws InterpretTypeException if argument count doesn't match arity
     */
    public function execute(ObjectValue $receiver, array $arguments, Context $context): SolValue
    {
        if (count($arguments) !== $this->arity) {
            throw new InterpretTypeException('Arity mismatch');
        }

        // Create a new execution frame and push it to the stack
        $context->pushFrame(new Frame($receiver, $this->interpreter));

        // Bind arguments to the parameter names based on order
        foreach ($this->parameters as $order => $param) {
            $argumentIndex = $order - 1; // Convert order (1-based) to index (0-based)
            $context->setVariable($param, $arguments[$argumentIndex]);
        }

        // Default result is Nil unless statements override it
        $result = $context->getNil();

        // Execute each statement in the block
        foreach ($this->statements as $statement) {
            $result = $statement->execute($context);
        }

        // Pop the frame after block execution finishes
        $context->popFrame();

        // Return the last evaluated result
        return $result;
    }
}
