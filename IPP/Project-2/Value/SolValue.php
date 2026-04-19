<?php

declare(strict_types=1);

namespace IPP\Student\Value;

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
use IPP\Student\Expression\Variable;

use IPP\Student\Value\BlockValue;
use IPP\Student\Value\BoolValue;
use IPP\Student\Value\IntegerValue;
use IPP\Student\Value\NilValue;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\StringValue;

use DOMDocument;
use DOMElement;

use IPP\Core\AbstractInterpreter;
use IPP\Core\Exception\XMLException;
use IPP\Core\Interface\InputReader;
use IPP\Core\Interface\OutputWriter;

/**
 * Abstract base class for all value types in the SOL25 interpreter.
 */
abstract class SolValue
{
    /**
     * The class of the value.
     *
     * @var SolClass
     */
    protected SolClass $class;

    /**
     * The interpreter instance.
     *
     * @var Interpreter
     */
    protected Interpreter $interpreter;

    /**
     * Constructor for the value.
     *
     * @param SolClass $class The class of the value.
     * @param Interpreter $interpreter The interpreter instance.
     */
    public function __construct(SolClass $class, Interpreter $interpreter)
    {
        $this->class = $class;
        $this->interpreter = $interpreter;
    }

    /**
     * Sends a message to the value and returns the result.
     *
     * @param string $selector The message selector.
     * @param array<SolValue> $arguments The arguments of the message.
     * @param Context $context The runtime context.
     * @return SolValue The result of the message.
     * @throws InterpretDNUException If the message is not understood.
     */
    public function sendMessage(string $selector, array $arguments, Context $context): SolValue
    {
        // Check if a handler exists for this message
        foreach ($context->getMessageHandlers() as $handler) {
            if ($handler->supports($this->class->getName(), $selector)) {
                return $handler->handle($this, $selector, $arguments, $context);
            }
        }

        // If no handler is found, try to find a method in the class hierarchy
        $currentClass = $this->class;
        while ($currentClass !== null) {
            $method = $currentClass->getMethod($selector);
            if ($method !== null && $this instanceof ObjectValue) {
                return $method->execute($this, $arguments, $context);
            }
            $currentClass = $currentClass->getParent($context);
        }

        throw new InterpretDNUException("Message $selector not understood");
    }

    /**
     * Checks if this value is identical to another value.
     *
     * @param SolValue $other The value to compare with.
     * @return bool True if the values are identical, false otherwise.
     */
    public function isIdenticalTo(SolValue $other): bool
    {
        return $this === $other;
    }

    /**
     * Returns the class of the value.
     *
     * @return SolClass The class of the value.
     */
    public function getClass(): SolClass
    {
        return $this->class;
    }

    /**
     * Returns the string representation of the value.
     *
     * @return string The string representation.
     */
    abstract public function printValue(): string;
}