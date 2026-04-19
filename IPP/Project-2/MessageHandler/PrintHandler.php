<?php

declare(strict_types=1);

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\Value\BoolValue;
use IPP\Student\Value\NilValue;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Value\StringValue;
use IPP\Student\Exception\InterpretDNUException;
use IPP\Student\Exception\InterpretTypeException;

/**
 * Handler for the 'print' message.
 */
class PrintHandler implements MessageHandlerInterface
{
    /**
     * Checks if the handler supports the given class and selector.
     *
     * @param string $class The name of the class.
     * @param string $selector The name of the selector.
     * @return bool True if supported, false otherwise.
     */
    public function supports(string $class, string $selector): bool
    {
        return $selector === 'print' && in_array($class, ['String', 'True', 'False', 'Nil'], true);
    }

    /**
     * Handles the 'print' message.
     *
     * @param SolValue $receiver The receiver of the message.
     * @param string $selector The message selector.
     * @param array<SolValue> $arguments The message arguments.
     * @param Context $context The runtime context.
     * @return SolValue The Nil value.
     * @throws InterpretTypeException If arguments are provided or the 'value' attribute is not a StringValue.
     * @throws InterpretDNUException If the receiver is not a supported type.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        // Validate that no arguments are provided
        if (!empty($arguments)) {
            throw new InterpretTypeException('print does not accept arguments');
        }

        // Handle StringValue, BoolValue, or NilValue
        if ($receiver instanceof StringValue ||
            $receiver instanceof BoolValue ||
            $receiver instanceof NilValue
        ) {
            $context->getStdout()->writeString($receiver->printValue());
            return $context->getNil();
        }

        // Handle ObjectValue of String class
        if ($receiver instanceof ObjectValue && $receiver->getClass()->getName() === 'String') {
            $value = $receiver->getAttribute('value');
            if ($value instanceof StringValue) {
                $context->getStdout()->writeString($value->getValue());
                return $context->getNil();
            }
            throw new InterpretTypeException('String object must have a StringValue attribute "value"');
        }

        // Throw exception for unsupported receiver types
        throw new InterpretDNUException('Message print not understood by ' . $receiver->getClass()->getName());
    }
}