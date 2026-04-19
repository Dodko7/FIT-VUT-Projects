<?php

namespace IPP\Student\MessageHandler;

use IPP\Student\Assign;
use IPP\Student\Block;
use IPP\Student\Context;
use IPP\Student\Frame;
use IPP\Student\Interpreter;
use IPP\Student\Method;
use IPP\Student\Program;
use IPP\Student\SolClass;

use IPP\Student\Exception\InterpretDNUException;
use IPP\Student\Exception\InterpretTypeException;
use IPP\Student\Exception\InterpretValueException;

use IPP\Student\Expression\BlockExpr;
use IPP\Student\Expression\Expression;
use IPP\Student\Expression\Literal;
use IPP\Student\Expression\Send;
use IPP\Student\Expression\Variable;

use IPP\Student\Value\BoolValue;
use IPP\Student\Value\BlockValue;
use IPP\Student\Value\IntegerValue;
use IPP\Student\Value\NilValue;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Value\StringValue;

use DOMDocument;
use DOMElement;

use IPP\Core\AbstractInterpreter;
use IPP\Core\Exception\XMLException;
use IPP\Core\Interface\InputReader;
use IPP\Core\Interface\OutputWriter;

/**
 * Handler for the 'greaterThan:' message for the Integer class.
 *
 * Compares the receiver's integer value with the argument's integer value,
 * returning a BoolValue indicating if the receiver is greater.
 */
class GreaterThanHandler implements MessageHandlerInterface
{
    /**
     * Constructs the handler.
     */
    public function __construct()
    {
    }

    /**
     * Checks if the handler supports the given message for a specific class.
     *
     * @param string $className The class name of the message receiver.
     * @param string $selector The message selector.
     * @return bool True if the class is 'Integer' and the selector is 'greaterThan:', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return $className === 'Integer' && $selector === 'greaterThan:';
    }

    /**
     * Handles the 'greaterThan:' message.
     *
     * Compares the receiver's integer value with the argument's integer value,
     * returning a BoolValue indicating if the receiver is greater than the argument.
     *
     * @param SolValue $receiver The message receiver (expected to be an IntegerValue).
     * @param string $selector The message selector (expected to be 'greaterThan:').
     * @param array<int, SolValue> $arguments The message arguments (exactly one IntegerValue expected).
     * @param Context $context The runtime context for accessing boolean values.
     * @return SolValue A BoolValue indicating if the receiver is greater than the argument.
     * @throws InterpretTypeException If the receiver or argument is not an IntegerValue or the argument count is not one.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (!$receiver instanceof IntegerValue) {
            throw new InterpretTypeException("Receiver for 'greaterThan:' must be an Integer");
        }

        if (count($arguments) !== 1) {
            throw new InterpretTypeException("'greaterThan:' requires exactly one argument");
        }

        $arg = $arguments[0];
        if (!$arg instanceof IntegerValue) {
            throw new InterpretTypeException("Argument for 'greaterThan:' must be an Integer");
        }

        $result = $receiver->getValue() > $arg->getValue();
        return $context->getBool($result);
    }
}