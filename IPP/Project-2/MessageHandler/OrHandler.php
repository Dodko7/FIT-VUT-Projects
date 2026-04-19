<?php

declare(strict_types=1);

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\Value\BlockValue;
use IPP\Student\Value\BoolValue;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Exception\InterpretTypeException;

/**
 * Handler for the 'or:' message for logical OR operation.
 */
class OrHandler implements MessageHandlerInterface
{
    /**
     * Checks if the handler supports the given message.
     *
     * @param string $className The name of the receiver's class.
     * @param string $selector The message selector.
     * @return bool True if the class is 'True' or 'False' and the selector is 'or:', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return in_array($className, ['True', 'False'], true) && $selector === 'or:';
    }

    /**
     * Handles the 'or:' message.
     *
     * @param SolValue $receiver The receiver of the message (BoolValue of True or False class).
     * @param string $selector The message selector ('or:').
     * @param array<SolValue> $arguments Array of arguments (expects one BlockValue or ObjectValue with a BoolValue attribute).
     * @param Context $context The runtime context.
     * @return SolValue BoolValue with the result of the logical OR operation.
     * @throws InterpretTypeException If the argument is not a BlockValue or ObjectValue with a valid BoolValue attribute, or if the receiver is not a BoolValue.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        // Validate the number of arguments
        if (count($arguments) !== 1) {
            throw new InterpretTypeException('or: expects exactly one argument');
        }

        // Validate the receiver type
        if (!$receiver instanceof BoolValue) {
            throw new InterpretTypeException('Receiver for or: must be a BoolValue');
        }

        $arg = $arguments[0];

        // If receiver is True, return True without evaluating the argument (short-circuit)
        if ($receiver->getValue()) {
            return $context->getTrue();
        }

        // Handle BlockValue (lazy evaluation)
        if ($arg instanceof BlockValue) {
            $blockResult = $arg->sendMessage('value', [], $context);
            if (!$blockResult instanceof BoolValue) {
                throw new InterpretTypeException('Block for or: must return a BoolValue');
            }
            return $context->getBool($blockResult->getValue());
        }

        // Handle ObjectValue with value attribute
        if ($arg instanceof ObjectValue) {
            $value = $arg->getAttribute('value');
            if (!$value instanceof BoolValue) {
                throw new InterpretTypeException('ObjectValue for or: must have a BoolValue attribute "value"');
            }
            return $context->getBool($value->getValue());
        }

        // Throw exception for invalid argument types
        throw new InterpretTypeException('Argument for or: must be a BlockValue or ObjectValue with BoolValue attribute "value"');
    }
}