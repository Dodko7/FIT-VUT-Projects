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
use IPP\Student\Value\SolValue;
use IPP\Student\Value\StringValue;

use DOMDocument;
use DOMElement;

use IPP\Core\AbstractInterpreter;
use IPP\Core\Exception\XMLException;
use IPP\Core\Interface\InputReader;
use IPP\Core\Interface\OutputWriter;

use IPP\Student\MessageDispatcher;

/**
 * Class representing an object value in SOL25.
 */
class ObjectValue extends SolValue
{
    /**
     * Associative array of object attributes.
     *
     * @var array<string, SolValue>
     */
    private array $attributes = [];

    /**
     * Native value of the object, used for Integer and String types.
     *
     * @var SolValue|null
     */
    private ?SolValue $nativeValue;

    /**
     * Constructor for the object value.
     *
     * @param SolClass $class The class of the object.
     * @param Interpreter $interpreter The interpreter instance.
     * @param SolValue|null $nativeValue The native value, if applicable (default null).
     */
    public function __construct(SolClass $class, Interpreter $interpreter, ?SolValue $nativeValue = null)
    {
        parent::__construct($class, $interpreter);
        $this->nativeValue = $nativeValue;
    }

    /**
     * Sends a message to the object and returns the result.
     *
     * @param string $selector The message selector.
     * @param array<SolValue> $arguments The arguments of the message.
     * @param Context $context The runtime context.
     * @return SolValue The result of the message.
     * @throws InterpretDNUException If the message is not supported.
     */
    public function sendMessage(string $selector, array $arguments, Context $context): SolValue
    {
        $dispatcher = new MessageDispatcher();
        return $dispatcher->dispatch($this, $selector, $arguments, $context);
    }

    /**
     * Sets the value of an object attribute.
     *
     * @param string $name The name of the attribute.
     * @param SolValue $value The value to set.
     * @param bool $isInitialization Indicates if this is an initialization (used for internal attributes).
     * @throws InterpretDNUException If attempting to modify internal 'value' attribute outside initialization.
     * @throws InterpretTypeException If the interpreter context is not available.
     */
    public function setAttribute(string $name, SolValue $value, bool $isInitialization = false): void
    {
        $context = $this->interpreter->getContext();
        if ($context === null) {
            throw new InterpretTypeException('Interpreter context is required for attribute manipulation');
        }

        if ($name === 'value' && !$isInitialization &&
            ($this->class->isSubclassOf('Integer', $context) ||
             $this->class->isSubclassOf('String', $context))) {
            throw new InterpretDNUException("Cannot modify internal attribute 'value' in " . $this->class->getName());
        }

        $this->attributes[$name] = $value;
    }

    /**
     * Retrieves the value of an object attribute.
     *
     * @param string $name The name of the attribute.
     * @return SolValue|null The attribute value, or null if the attribute does not exist.
     */
    public function getAttribute(string $name): ?SolValue
    {
        return $this->attributes[$name] ?? null;
    }

    /**
     * Checks if the object has a given attribute.
     *
     * @param string $name The name of the attribute.
     * @return bool True if the attribute exists, false otherwise.
     */
    public function hasAttribute(string $name): bool
    {
        return array_key_exists($name, $this->attributes);
    }

    /**
     * Retrieves the native value of the object (used for Integer and String).
     *
     * @return SolValue|null The native value, or null if not set.
     */
    public function getNativeValue(): ?SolValue
    {
        return $this->nativeValue;
    }

    /**
     * Returns the string representation of the object value.
     *
     * @return string The string representation.
     */
    public function printValue(): string
    {
        return $this->nativeValue !== null ? $this->nativeValue->printValue() : $this->class->getName();
    }
}