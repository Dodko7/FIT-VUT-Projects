<?php

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\Value\BlockValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Exception\InterpretTypeException;

/**
 * Handler for the 'isBlock' message to check if a value is a block.
 */
class IsBlockHandler implements MessageHandlerInterface
{
    /**
     * Checks if the handler supports the given class and selector.
     *
     * Supports 'Block' class and 'isBlock' selector.
     *
     * @param string $class The class name.
     * @param string $selector The message selector.
     * @return bool True if supported.
     */
    public function supports(string $class, string $selector): bool
    {
        return $class === 'Block' && $selector === 'isBlock';
    }

    /**
     * Handles the 'isBlock' message.
     *
     * Returns true to indicate that the receiver is a block.
     *
     * @param SolValue $receiver The receiver of the message (BlockValue).
     * @param string $selector The message selector ('isBlock').
     * @param array<SolValue> $arguments The message arguments (empty).
     * @param Context $context The execution context.
     * @return SolValue True singleton.
     * @throws InterpretTypeException If the receiver is not a BlockValue or arguments are provided.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (!$receiver instanceof BlockValue || count($arguments) !== 0) {
            throw new InterpretTypeException('Invalid arguments for isBlock');
        }
        return $context->getTrue();
    }
}