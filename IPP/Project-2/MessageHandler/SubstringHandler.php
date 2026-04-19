<?php

declare(strict_types=1);

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\Interpreter;
use IPP\Student\Value\IntegerValue;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Value\StringValue;
use IPP\Student\Exception\InterpretTypeException;

/**
 * Handler for the 'startsWith:endsBefore:' message to extract a substring.
 */
class SubstringHandler implements MessageHandlerInterface
{
    use HandlerTrait;

    /**
     * Constructor for the SubstringHandler.
     *
     * @param Interpreter $interpreter The interpreter instance.
     */
    public function __construct(private readonly Interpreter $interpreter)
    {
    }

    /**
     * Checks if the handler supports the given message for the specified class.
     *
     * @param string $className The name of the class.
     * @param string $selector The message selector.
     * @return bool True if the message is supported, false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return $className === 'String' && $selector === 'startsWith:endsBefore:';
    }

    /**
     * Handles the 'startsWith:endsBefore:' message.
     *
     * @param SolValue $receiver The receiver of the message (ObjectValue of String class).
     * @param string $selector The message selector.
     * @param array<SolValue> $arguments The message arguments (two ObjectValue of Integer class).
     * @param Context $context The runtime context.
     * @return SolValue ObjectValue of String class with the substring or NilValue.
     * @throws InterpretTypeException If the arguments or receiver are invalid.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        // Validate the number of arguments
        if (count($arguments) !== 2) {
            throw new InterpretTypeException('startsWith:endsBefore: expects exactly two arguments');
        }

        // Validate the receiver type and class
        if (!$receiver instanceof ObjectValue || $receiver->getClass()->getName() !== 'String') {
            throw new InterpretTypeException('Receiver for startsWith:endsBefore: must be an ObjectValue of String class');
        }

        // Retrieve and validate the receiver's value attribute
        $receiverValue = $receiver->getAttribute('value');
        if (!$receiverValue instanceof StringValue) {
            throw new InterpretTypeException('Receiver must have a StringValue attribute "value"');
        }

        // Retrieve the start and end arguments
        $startArg = $arguments[0];
        $endArg = $arguments[1];

        // Validate the argument types and classes
        if (!$startArg instanceof ObjectValue || $startArg->getClass()->getName() !== 'Integer' ||
            !$endArg instanceof ObjectValue || $endArg->getClass()->getName() !== 'Integer') {
            throw new InterpretTypeException('Arguments for startsWith:endsBefore: must be ObjectValue of Integer class');
        }

        // Retrieve and validate the arguments' value attributes
        $startValue = $startArg->getAttribute('value');
        $endValue = $endArg->getAttribute('value');

        if (!$startValue instanceof IntegerValue || !$endValue instanceof IntegerValue) {
            throw new InterpretTypeException('Arguments must have an IntegerValue attribute "value"');
        }

        // Extract the start, end, and string values
        $start = $startValue->getValue();
        $end = $endValue->getValue();
        $string = $receiverValue->getValue();
        $stringClass = $this->getClass($context, 'String');

        // Check for invalid indices
        if ($start <= 0 || $end <= 0 || $end < $start) {
            return $context->getNil();
        }

        // Convert indices to PHP indexing (0-based)
        $startIndex = $start - 1;
        $length = $end - $start;

        // Validate the range
        if ($startIndex >= strlen($string) || $end > strlen($string)) {
            return $context->getNil();
        }

        // Extract the substring
        $result = substr($string, $startIndex, $length);
        $newInstance = new ObjectValue($stringClass, $this->interpreter);
        $newInstance->setAttribute('value', new StringValue($result, $stringClass, $this->interpreter), true);
        return $newInstance;
    }
}