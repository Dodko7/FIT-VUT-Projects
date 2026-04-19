<?php

namespace IPP\Student\MessageHandler;

use IPP\Student\Block;
use IPP\Student\Context;
use IPP\Student\Value\BlockValue;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Exception\InterpretTypeException;

/**
 * Handler for the 'value[:]*' messages to execute a block with a variable number of arguments.
 */
class ValueHandler implements MessageHandlerInterface
{
    /**
     * Checks if the handler supports the given class and selector.
     *
     * Supports 'Block' class and selectors matching 'value[:]*' (e.g., 'value', 'value:', 'value:value:').
     *
     * @param string $class The class name.
     * @param string $selector The message selector.
     * @return bool True if supported.
     */
    public function supports(string $class, string $selector): bool
    {
        return $class === 'Block' && preg_match('/^value(:.*)?$/', $selector);
    }

    /**
     * Handles the 'value[:]*' message.
     *
     * Executes the block with the provided arguments, ensuring the number of arguments matches
     * the block's arity.
     *
     * @param SolValue $receiver The receiver of the message (BlockValue).
     * @param string $selector The message selector (e.g., 'value', 'value:', 'value:value:').
     * @param array<SolValue> $arguments The message arguments.
     * @param Context $context The execution context.
     * @return SolValue The result of the block execution.
     * @throws InterpretTypeException If the receiver is not a BlockValue, arguments don't match arity, or receiver is invalid.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (!$receiver instanceof BlockValue) {
            throw new InterpretTypeException('Receiver must be a BlockValue for value[:]*');
        }

        // Count the number of colons to determine expected argument count
        $expectedArgCount = substr_count($selector, ':');
        if ($selector === 'value') {
            $expectedArgCount = 0;
        }

        if (count($arguments) !== $expectedArgCount || $expectedArgCount !== $receiver->getArity()) {
            throw new InterpretTypeException('Arity mismatch for value[:]*');
        }

        // Try to use self from the block's definition context
        $blockContext = $receiver->getContext();
        $blockReceiver = $blockContext->getVariable('self');

        // If self from block context is not ObjectValue, use self from current context
        if (!($blockReceiver instanceof ObjectValue)) {
            $blockReceiver = $context->getVariable('self');
            if (!($blockReceiver instanceof ObjectValue)) {
                throw new InterpretTypeException('Block receiver must be an ObjectValue');
            }
        }

        return $receiver->getBlock()->execute($blockReceiver, $arguments, $context);
    }
}