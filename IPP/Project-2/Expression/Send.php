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
 * Represents a message send expression in the SOL25 interpreter.
 *
 * This class encapsulates a message send operation, where a selector is sent to a receiver
 * with optional arguments, evaluated in the provided context.
 */
class Send extends Expression
{
    /** @var string The message selector */
    private string $selector;
    /** @var Expression The receiver of the message */
    private Expression $receiver;
    /** @var array<int, Expression> The arguments passed with the message, indexed by order */
    private array $arguments = [];

    /**
     * Constructor for Send.
     *
     * Initializes a Send expression by parsing the selector, receiver, and arguments from
     * the provided DOM element.
     *
     * @param Interpreter $interpreter The interpreter instance for I/O and debugging.
     * @param DOMElement $element The DOM element representing the send expression (with 'selector' attribute).
     * @throws InterpretTypeException If the selector, receiver, or argument order is invalid.
     */
    public function __construct(Interpreter $interpreter, DOMElement $element)
    {
        parent::__construct($interpreter);

        // Parse the selector
        $this->selector = $element->getAttribute('selector');
        if (empty($this->selector)) {
            throw new InterpretTypeException('Missing selector in send');
        }

        // Parse the receiver and arguments
        $argumentsTemp = [];
        foreach ($element->childNodes as $node) {
            if ($node instanceof \DOMElement) {
                if ($node->nodeName === 'expr' && !isset($this->receiver)) {
                    $this->receiver = Expression::createFromXML($node, $interpreter);
                } elseif ($node->nodeName === 'arg') {
                    $order = (int)$node->getAttribute('order');
                    if ($order < 1) {
                        throw new InterpretTypeException('Invalid argument order in send');
                    }
                    foreach ($node->childNodes as $argNode) {
                        if ($argNode instanceof \DOMElement && $argNode->nodeName === 'expr') {
                            $argumentsTemp[$order] = Expression::createFromXML($argNode, $interpreter);
                            break;
                        }
                    }
                }
            }
        }

        // Ensure the receiver is defined
        if (!isset($this->receiver)) {
            throw new InterpretTypeException('Missing receiver expr in send');
        }

        // Sort arguments by order and ensure all orders are consecutive
        ksort($argumentsTemp);
        $this->arguments = array_values($argumentsTemp);
        foreach (array_keys($argumentsTemp) as $index => $order) {
            if ($order !== $index + 1) {
                throw new InterpretTypeException('Non-consecutive or missing argument order in send');
            }
        }
    }

    /**
     * Evaluate the send expression in the given context.
     *
     * Evaluates the receiver and arguments, then sends the message to the receiver, returning
     * the result of the message execution.
     *
     * @param Context $context The execution context containing variable bindings and classes.
     * @return SolValue The result of the message execution.
     * @throws InterpretTypeException If evaluation fails (e.g., invalid receiver or arguments).
     */
    public function evaluate(Context $context): SolValue
    {
        // Evaluate the receiver
        $receiverValue = $this->receiver->evaluate($context);

        // Evaluate the arguments
        $argValues = [];
        foreach ($this->arguments as $arg) {
            $argValues[] = $arg->evaluate($context);
        }

        // Send the message
        $result = $receiverValue->sendMessage($this->selector, $argValues, $context);
        return $result;
    }

    /**
     * Get the evaluated value of the receiver.
     *
     * This method is used by Assign to retrieve the receiver's value for assignment purposes,
     * ensuring that assignments like `var := receiver print` assign the receiver's value
     * rather than the message result.
     *
     * @param Context $context The execution context.
     * @return SolValue The evaluated receiver value.
     */
    public function getReceiverValue(Context $context): SolValue
    {
        $receiverValue = $this->receiver->evaluate($context);
        return $receiverValue;
    }

    /**
     * Get the selector of the message.
     *
     * This method is used by Assign to determine whether to assign the receiver's value
     * (e.g., for 'print') or the message result.
     *
     * @return string The message selector.
     */
    public function getSelector(): string
    {
        return $this->selector;
    }
}