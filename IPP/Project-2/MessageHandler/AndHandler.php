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
 * Handler for the 'and:' message for logical AND operation.
 *
 * Processes the 'and:' message for boolean receivers (True or False), performing
 * logical AND with lazy evaluation for BlockValue arguments.
 */
class AndHandler implements MessageHandlerInterface
{
    /**
     * Checks if the handler supports the given message.
     *
     * @param string $className The class name of the message receiver.
     * @param string $selector The message selector.
     * @return bool True if the class is 'True' or 'False' and the selector is 'and:', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return in_array($className, ['True', 'False'], true) && $selector === 'and:';
    }

    /**
     * Handles the 'and:' message.
     *
     * Performs a logical AND operation between the boolean receiver and the argument,
     * supporting lazy evaluation for BlockValue arguments and ObjectValue with a BoolValue attribute.
     *
     * @param SolValue $receiver The message receiver (expected to be a BoolValue).
     * @param string $selector The message selector (expected to be 'and:').
     * @param array<int, SolValue> $arguments The message arguments (exactly one expected).
     * @param Context $context The runtime context for accessing boolean values.
     * @return SolValue A BoolValue representing the result of the logical AND.
     * @throws InterpretTypeException If the argument count is not one, the receiver is not a BoolValue,
     *                               the argument is not a BlockValue or valid ObjectValue, or the block result is not a BoolValue.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (count($arguments) !== 1) {
            throw new InterpretTypeException('and: expects exactly one argument');
        }

        if (!$receiver instanceof BoolValue) {
            throw new InterpretTypeException('Receiver for and: must be a BoolValue');
        }

        $arg = $arguments[0];

        // If receiver is False, return False without evaluating the argument
        if (!$receiver->getValue()) {
            return $context->getFalse();
        }

        // Handle BlockValue (lazy evaluation)
        if ($arg instanceof BlockValue) {
            $blockResult = $arg->sendMessage('value', [], $context);
            if (!$blockResult instanceof BoolValue) {
                throw new InterpretTypeException('Block for and: must return a BoolValue');
            }
            return $context->getBool($blockResult->getValue());
        }

        // Handle ObjectValue with value attribute
        if ($arg instanceof ObjectValue) {
            $value = $arg->getAttribute('value');
            if (!$value instanceof BoolValue) {
                throw new InterpretTypeException('ObjectValue for and: must have a BoolValue attribute "value"');
            }
            return $context->getBool($value->getValue());
        }

        throw new InterpretTypeException('Argument for and: must be a BlockValue or ObjectValue with BoolValue attribute "value"');
    }
}