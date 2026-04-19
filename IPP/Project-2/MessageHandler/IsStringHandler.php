<?php

declare(strict_types=1);

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\Exception\InterpretTypeException;
use IPP\Student\Value\SolValue;
use IPP\Student\Value\StringValue;

/**
 * Handler for the 'isString' message to check if an object is an instance of String or its subclass.
 *
 * Determines whether the receiver is a StringValue or an instance of String or its subclass,
 * returning a BoolValue with the result.
 */
class IsStringHandler implements MessageHandlerInterface
{
    /**
     * Checks if the handler supports the given message.
     *
     * @param string $className The class name of the message receiver.
     * @param string $selector The message selector.
     * @return bool True if the selector is 'isString', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return $selector === 'isString';
    }

    /**
     * Handles the 'isString' message.
     *
     * Checks if the receiver is a StringValue or an instance of String or its subclass,
     * returning a BoolValue indicating the result.
     *
     * @param SolValue $receiver The message receiver (e.g., ObjectValue, StringValue).
     * @param string $selector The message selector (expected to be 'isString').
     * @param array<int, SolValue> $arguments The message arguments (expected to be empty).
     * @param Context $context The runtime context for accessing boolean values.
     * @return SolValue A BoolValue indicating if the receiver is a StringValue or an instance of String or its subclass.
     * @throws InterpretTypeException If any arguments are provided.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (count($arguments) !== 0) {
            throw new InterpretTypeException('isString expects no arguments');
        }

        $isString = $receiver instanceof StringValue || $receiver->getClass()->isSubclassOf('String', $context);
        return $context->getBool($isString);
    }
}