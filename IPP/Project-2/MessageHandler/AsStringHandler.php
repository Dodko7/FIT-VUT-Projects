<?php

declare(strict_types=1);

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\Interpreter;
use IPP\Student\Value\BoolValue;
use IPP\Student\Value\BlockValue;
use IPP\Student\Value\IntegerValue;
use IPP\Student\Value\NilValue;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Value\StringValue;
use IPP\Student\Exception\InterpretTypeException;
/**
 * Handler for the 'asString' message to convert a value to a string.
 *
 * Converts various SolValue types (e.g., IntegerValue, BoolValue, NilValue) to their
 * string representation as a StringValue.
 */
class AsStringHandler implements MessageHandlerInterface
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
     * @return bool True if the selector is 'asString', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return $selector === 'asString';
    }

    /**
     * Handles the 'asString' message.
     *
     * Converts the receiver's value to a string representation, returning a StringValue.
     * Supports StringValue, IntegerValue, BoolValue, NilValue, BlockValue, and ObjectValue types.
     *
     * @param SolValue $receiver The message receiver (e.g., ObjectValue, StringValue).
     * @param string $selector The message selector (expected to be 'asString').
     * @param array<int, SolValue> $arguments The message arguments (expected to be empty).
     * @param Context $context The runtime context for accessing class information.
     * @return SolValue A StringValue representing the receiver's value.
     * @throws InterpretTypeException If arguments are provided or the receiver type is unsupported.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (!empty($arguments)) {
            throw new InterpretTypeException('asString does not accept arguments');
        }

        $stringClass = $this->getClass($context, 'String');

        if ($receiver instanceof StringValue) {
            return $receiver;
        } elseif ($receiver instanceof IntegerValue) {
            return new StringValue((string)$receiver->getValue(), $stringClass, $this->interpreter);
        } elseif ($receiver instanceof BoolValue) {
            return new StringValue($receiver->getValue() ? 'true' : 'false', $stringClass, $this->interpreter);
        } elseif ($receiver instanceof NilValue) {
            return new StringValue('nil', $stringClass, $this->interpreter);
        } elseif ($receiver instanceof BlockValue) {
            return new StringValue('[]', $stringClass, $this->interpreter);
        } elseif ($receiver instanceof ObjectValue) {
            return new StringValue('', $stringClass, $this->interpreter);
        }

        throw new InterpretTypeException('Unsupported type for asString');
    }
}