<?php

namespace IPP\Student\MessageHandler;

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

use IPP\Student\Value\BoolValue;
use IPP\Student\Value\BlockValue;
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
 * Handler for the 'from:' message to create a new instance from a value or object.
 *
 * Creates a new instance of the receiver's class, initializing it with the provided
 * argument's value or copying attributes from a compatible object.
 */
class FromHandler implements MessageHandlerInterface
{
    /**
     * @param Interpreter $interpreter The interpreter instance for creating new instances.
     */
    public function __construct(private readonly Interpreter $interpreter)
    {
    }

    /**
     * Checks if the handler supports the given message.
     *
     * @param string $className The class name of the message receiver.
     * @param string $selector The message selector.
     * @return bool True if the selector is 'from:', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return $selector === 'from:';
    }

    /**
     * Handles the 'from:' message.
     *
     * Creates a new instance of the receiver's class, initializing it with the argument's
     * value (for Integer or String) or copying attributes from a compatible object.
     *
     * @param SolValue $receiver The message receiver (expected to be an ObjectValue).
     * @param string $selector The message selector (expected to be 'from:').
     * @param array<int, SolValue> $arguments The message arguments (exactly one expected).
     * @param Context $context The runtime context for accessing class information.
     * @return SolValue A new ObjectValue instance initialized with the argument's value or attributes.
     * @throws InterpretTypeException If the argument count is not one, the receiver is not an ObjectValue,
     *                               the class is not found, or the argument type is invalid.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (count($arguments) !== 1) {
            throw new InterpretTypeException('from: requires exactly one argument');
        }

        if (!$receiver instanceof ObjectValue) {
            throw new InterpretTypeException('Receiver must be a class instance for from:');
        }

        $className = $receiver->getClass()->getName();
        $class = $context->getClass($className);
        if ($class === null) {
            throw new InterpretTypeException("Class $className not found");
        }

        $arg = $arguments[0];
        $newInstance = new ObjectValue($class, $this->interpreter);

        // Process the argument
        if ($arg instanceof IntegerValue && $class->isSubclassOf('Integer', $context)) {
            $newInstance->setAttribute('value', $arg, true);
        } elseif ($arg instanceof StringValue && $class->isSubclassOf('String', $context)) {
            $newInstance->setAttribute('value', $arg, true);
        } elseif ($arg instanceof ObjectValue) {
            // Verify class compatibility
            if (!$this->isCompatibleClass($receiver->getClass(), $arg->getClass(), $context)) {
                throw new InterpretTypeException('Argument class must be same, subclass, or superclass of receiver class');
            }
            // Copy attributes
            $this->copyAttributes($arg, $newInstance);
        } else {
            throw new InterpretTypeException('Invalid argument type for from:');
        }

        return $newInstance;
    }

    /**
     * Checks if the argument's class is compatible (same, subclass, or superclass).
     *
     * @param SolClass $receiverClass The class of the receiver.
     * @param SolClass $argClass The class of the argument.
     * @param Context $context The runtime context for class hierarchy checks.
     * @return bool True if the classes are compatible, false otherwise.
     */
    private function isCompatibleClass(SolClass $receiverClass, SolClass $argClass, Context $context): bool
    {
        return $receiverClass->getName() === $argClass->getName() ||
               $receiverClass->isSubclassOf($argClass->getName(), $context) ||
               $argClass->isSubclassOf($receiverClass->getName(), $context);
    }

    /**
     * Copies attributes from a source object to a target object.
     *
     * @param ObjectValue $source The source object to copy attributes from.
     * @param ObjectValue $target The target object to copy attributes to.
     */
    private function copyAttributes(ObjectValue $source, ObjectValue $target): void
    {
        if ($value = $source->getAttribute('value')) {
            $target->setAttribute('value', $value, true);
        }
    }
}