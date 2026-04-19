<?php

declare(strict_types=1);

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\Interpreter;
use IPP\Student\Value\IntegerValue;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Value\StringValue;
use IPP\Student\Exception\InterpretTypeException;

/**
 * Handler for the 'asInteger' message to convert a string to an integer.
 *
 * Converts a String ObjectValue to an IntegerValue if the string represents a valid integer,
 * otherwise returns a NilValue.
 */
class AsIntegerHandler implements MessageHandlerInterface
{
    use HandlerTrait;

    /**
     * @param Interpreter $interpreter The interpreter instance for accessing context and classes.
     */
    public function __construct(private readonly Interpreter $interpreter)
    {
    }

    /**
     * Checks if the handler supports the given message.
     *
     * @param string $className The class name of the message receiver.
     * @param string $selector The message selector.
     * @return bool True if the selector is 'asInteger' and the class is 'String', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return $selector === 'asInteger' && $className === 'String';
    }

    /**
     * Handles the 'asInteger' message.
     *
     * Converts the string value of the receiver to an integer if it represents a valid integer,
     * otherwise returns a NilValue.
     *
     * @param SolValue $receiver The message receiver (expected to be an ObjectValue of String class).
     * @param string $selector The message selector (expected to be 'asInteger').
     * @param array<int, SolValue> $arguments The message arguments (expected to be empty).
     * @param Context $context The runtime context for accessing class information.
     * @return SolValue An IntegerValue for a valid integer string, otherwise a NilValue.
     * @throws InterpretTypeException If arguments are provided, the receiver is not a String ObjectValue,
     *                               or the value attribute is not a StringValue.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (!empty($arguments)) {
            throw new InterpretTypeException('asInteger does not accept arguments');
        }

        if (!$receiver instanceof ObjectValue || $receiver->getClass()->getName() !== 'String') {
            throw new InterpretTypeException('Receiver for asInteger must be an ObjectValue of String class');
        }

        $value = $receiver->getAttribute('value');
        if (!$value instanceof StringValue) {
            throw new InterpretTypeException('String object must have a StringValue attribute "value"');
        }

        $stringValue = $value->getValue();
        if (!is_numeric($stringValue) || floor((float)$stringValue) !== (float)$stringValue) {
            return $context->getNil();
        }

        $integerClass = $this->getClass($context, 'Integer');
        return new IntegerValue((int)$stringValue, $integerClass, $this->interpreter);
    }
}