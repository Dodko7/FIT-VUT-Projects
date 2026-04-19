<?php

declare(strict_types=1);

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\Value\BoolValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Exception\InterpretTypeException;

/**
 * Handler for the 'not' message for logical negation.
 */
class NotHandler implements MessageHandlerInterface
{
    /**
     * Checks if the handler supports the given message.
     *
     * @param string $className The name of the receiver's class.
     * @param string $selector The message selector.
     * @return bool True if the class is 'True' or 'False' and the selector is 'not', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return in_array($className, ['True', 'False'], true) && $selector === 'not';
    }

    /**
     * Handles the 'not' message.
     *
     * @param SolValue $receiver The receiver of the message (BoolValue of True or False class).
     * @param string $selector The message selector ('not').
     * @param array<SolValue> $arguments Array of arguments (expected to be empty).
     * @param Context $context The runtime context.
     * @return SolValue BoolValue with the negated value (True → False, False → True).
     * @throws InterpretTypeException If arguments are provided or the receiver is not a BoolValue.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        // Validate that no arguments are provided
        if (!empty($arguments)) {
            throw new InterpretTypeException('not does not accept arguments');
        }

        // Validate the receiver type
        if (!$receiver instanceof BoolValue) {
            throw new InterpretTypeException('Receiver for not must be a BoolValue');
        }

        // Return BoolValue with negated value
        return $context->getBool(!$receiver->getValue());
    }
}