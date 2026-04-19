<?php

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\Interpreter;
use IPP\Student\Value\BlockValue;
use IPP\Student\Value\BoolValue;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Exception\InterpretTypeException;

/**
 * Handler for the 'whileTrue:' message to execute a block in a loop while the condition block returns true.
 */
class WhileTrueHandler implements MessageHandlerInterface
{
    public function __construct(private readonly Interpreter $interpreter)
    {
    }

    /**
     * Checks if the handler supports the given class and selector.
     *
     * Supports 'Block' class and 'whileTrue:' selector.
     *
     * @param string $class The class name.
     * @param string $selector The message selector.
     * @return bool True if supported.
     */
    public function supports(string $class, string $selector): bool
    {
        return $class === 'Block' && $selector === 'whileTrue:';
    }

    /**
     * Handles the 'whileTrue:' message.
     *
     * Executes the receiver block as a condition, and if it returns true, executes the argument block
     * in a loop until the condition returns false.
     *
     * @param SolValue $receiver The receiver of the message (BlockValue).
     * @param string $selector The message selector ('whileTrue:').
     * @param array<SolValue> $arguments The message arguments (single BlockValue).
     * @param Context $context The execution context.
     * @return SolValue The result of the loop (NilValue).
     * @throws InterpretTypeException If the receiver or arguments are invalid.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (!$receiver instanceof BlockValue || count($arguments) !== 1 || !$arguments[0] instanceof BlockValue) {
            throw new InterpretTypeException('Invalid arguments for whileTrue:');
        }

        $result = $context->getNil();
        // Use self from context if defined, otherwise create a new ObjectValue for this block
        $blockReceiver = $context->getVariable('self');
        if ($blockReceiver !== null && !($blockReceiver instanceof ObjectValue)) {
            throw new InterpretTypeException('Receiver must be an ObjectValue or null for whileTrue:');
        }
        $blockReceiver = $blockReceiver ?? new ObjectValue($receiver->getClass(), $this->interpreter);

        while (true) {
            $condition = $receiver->getBlock()->execute($blockReceiver, [], $context);
            if (!($condition instanceof BoolValue) || !$condition->getValue()) {
                break;
            }
            $result = $arguments[0]->sendMessage('value', [], $context);
        }

        return $result;
    }
}