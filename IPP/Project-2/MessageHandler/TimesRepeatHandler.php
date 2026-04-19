<?php

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\Interpreter;
use IPP\Student\Value\BlockValue;
use IPP\Student\Value\IntegerValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Exception\InterpretTypeException;
use IPP\Student\Exception\InterpretValueException;

/**
 * Handler for the 'timesRepeat:' message to repeat a block.
 */
class TimesRepeatHandler implements MessageHandlerInterface
{
    use HandlerTrait;

    public function __construct(private readonly Interpreter $interpreter)
    {
    }

    /**
     * Checks if the handler supports the given class and selector.
     *
     * @param string $class The class name.
     * @param string $selector The message selector.
     * @return bool True if supported (Integer and timesRepeat:).
     */
    public function supports(string $class, string $selector): bool
    {
        return $class === 'Integer' && $selector === 'timesRepeat:';
    }

    /**
     * Handles the 'timesRepeat:' message.
     *
     * Executes the provided block n times, where n is the receiver's value, passing
     * the iteration number (1 to n) as an argument to the block.
     *
     * @param SolValue $receiver The receiver of the message (IntegerValue).
     * @param string $selector The message selector.
     * @param array<SolValue> $arguments The message arguments (BlockValue).
     * @param Context $context The execution context.
     * @return SolValue The result (NilValue).
     * @throws InterpretTypeException If arguments or types are invalid.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (!$receiver instanceof IntegerValue || count($arguments) !== 1 || !$arguments[0] instanceof BlockValue) {
            throw new InterpretTypeException('Invalid arguments for timesRepeat:');
        }

        $n = $receiver->getValue();
        if ($n <= 0) {
            return $context->getNil(); // If n <= 0, the block is not executed
        }

        $block = $arguments[0];
        if ($block->getArity() !== 1) {
            throw new InterpretTypeException('Block for timesRepeat: must have arity 1');
        }

        // Execute the block n times with iterations from 1 to n
        $integerClass = $this->getClass($context, 'Integer');
        for ($i = 1; $i <= $n; $i++) {
            $iterationValue = new IntegerValue($i, $integerClass, $this->interpreter);
            $block->sendMessage('value:', [$iterationValue], $context);
        }

        return $context->getNil();
    }
}