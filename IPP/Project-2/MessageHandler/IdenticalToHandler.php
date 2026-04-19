<?php

declare(strict_types=1);

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\Exception\InterpretTypeException;
use IPP\Student\Value\SolValue;

/**
 * Handler for the 'identicalTo:' message to compare the identity of two values.
 *
 * Checks if the receiver and the argument are identical, returning a BoolValue with the result.
 */
class IdenticalToHandler implements MessageHandlerInterface
{
    /**
     * Checks if the handler supports the given message.
     *
     * @param string $className The class name of the message receiver.
     * @param string $selector The message selector.
     * @return bool True if the selector is 'identicalTo:', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return $selector === 'identicalTo:';
    }

    /**
     * Handles the 'identicalTo:' message.
     *
     * Compares the receiver and the argument for identity, returning a BoolValue indicating
     * whether they are identical.
     *
     * @param SolValue $receiver The message receiver (e.g., StringValue, ObjectValue).
     * @param string $selector The message selector (expected to be 'identicalTo:').
     * @param array<int, SolValue> $arguments The message arguments (exactly one expected).
     * @param Context $context The runtime context for accessing boolean values.
     * @return SolValue A BoolValue representing the result of the identity comparison.
     * @throws InterpretTypeException If exactly one argument is not provided.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (count($arguments) !== 1) {
            throw new InterpretTypeException('identicalTo: requires exactly one argument');
        }

        $arg = $arguments[0];
        return $context->getBool($receiver->isIdenticalTo($arg));
    }
}