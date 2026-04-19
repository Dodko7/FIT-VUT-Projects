<?php

declare(strict_types=1);

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\Interpreter;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Value\StringValue;
use IPP\Student\Exception\InterpretTypeException;

/**
 * Handler for the 'concatenateWith:' message to concatenate two strings.
 *
 * Concatenates the string value of the receiver with the string value of the argument,
 * returning a new String ObjectValue or NilValue if the argument is not a String ObjectValue.
 */
class ConcatenateWithHandler implements MessageHandlerInterface
{
    use HandlerTrait;

    /**
     * @param Interpreter $interpreter The interpreter instance for accessing context and classes.
     */
    public function __construct(private readonly Interpreter $interpreter)
    {
    }

    /**
     * Checks if the handler supports the given message for a specific class.
     *
     * @param string $className The class name of the message receiver.
     * @param string $selector The message selector.
     * @return bool True if the class is 'String' and the selector is 'concatenateWith:', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return $className === 'String' && $selector === 'concatenateWith:';
    }

    /**
     * Handles the 'concatenateWith:' message.
     *
     * Concatenates the string value of the receiver with the string value of the argument,
     * creating a new String ObjectValue with the result.
     *
     * @param SolValue $receiver The message receiver (expected to be an ObjectValue of String class).
     * @param string $selector The message selector (expected to be 'concatenateWith:').
     * @param array<int, SolValue> $arguments The message arguments (exactly one expected).
     * @param Context $context The runtime context for accessing class information.
     * @return SolValue An ObjectValue of String class with the concatenated strings, or NilValue if the argument is invalid.
     * @throws InterpretTypeException If the argument count is not one, the receiver or argument is not a valid String ObjectValue,
     *                               or their value attributes are not StringValue instances.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (count($arguments) !== 1) {
            throw new InterpretTypeException('concatenateWith: expects exactly one argument');
        }

        if (!$receiver instanceof ObjectValue || $receiver->getClass()->getName() !== 'String') {
            throw new InterpretTypeException('Receiver for concatenateWith: must be an ObjectValue of String class');
        }

        $receiverValue = $receiver->getAttribute('value');
        if (!$receiverValue instanceof StringValue) {
            throw new InterpretTypeException('Receiver must have a StringValue attribute "value"');
        }

        $stringClass = $this->getClass($context, 'String');

        if ($arguments[0] instanceof ObjectValue && $arguments[0]->getClass()->getName() === 'String') {
            $argValue = $arguments[0]->getAttribute('value');
            if (!$argValue instanceof StringValue) {
                throw new InterpretTypeException('Argument must have a StringValue attribute "value"');
            }
            $result = $receiverValue->getValue() . $argValue->getValue();
            $newInstance = new ObjectValue($stringClass, $this->interpreter);
            $newInstance->setAttribute('value', new StringValue($result, $stringClass, $this->interpreter), true);
            return $newInstance;
        }

        return $context->getNil();
    }
}