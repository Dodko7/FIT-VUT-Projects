<?php

declare(strict_types=1);

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
 * Handler for the 'minus:' message to perform subtraction of two numbers.
 *
 * This class implements the logic for handling the 'minus:' selector, which subtracts
 * an integer argument from an integer receiver and returns the result as an IntegerValue.
 */
class MinusHandler implements MessageHandlerInterface
{
    use HandlerTrait;

    /**
     * @param Interpreter $interpreter The interpreter instance for accessing context and classes.
     */
    public function __construct(private readonly Interpreter $interpreter)
    {
    }

    /**
     * Checks if the handler supports the given class name and selector.
     *
     * @param string $className The name of the class to check.
     * @param string $selector The selector to check (e.g., 'minus:').
     * @return bool True if the selector is 'minus:', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return $selector === 'minus:';
    }

    /**
     * Handles the 'minus:' message by performing subtraction.
     *
     * Subtracts the integer value of the argument from the integer value of the receiver.
     * Both the receiver and argument must be IntegerValue instances.
     *
     * @param SolValue $receiver The receiver of the message (expected to be an IntegerValue or ObjectValue with IntegerValue attribute).
     * @param string $selector The selector of the message (expected to be 'minus:').
     * @param array<int, SolValue> $arguments The arguments passed to the message (exactly one expected).
     * @param Context $context The execution context for accessing class information.
     * @return SolValue The result of the subtraction as an IntegerValue.
     * @throws InterpretTypeException If the argument count is not exactly one or if the receiver/argument are not integers.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (count($arguments) !== 1) {
            throw new InterpretTypeException('minus: requires exactly one argument');
        }

        $arg = $arguments[0];
        $receiverValue = $receiver instanceof ObjectValue ? $receiver->getAttribute('value') : $receiver;
        $argValue = $arg instanceof ObjectValue ? $arg->getAttribute('value') : $arg;

        if (!$receiverValue instanceof IntegerValue || !$argValue instanceof IntegerValue) {
            throw new InterpretTypeException('Both receiver and argument for minus: must be integers');
        }

        $result = $receiverValue->getValue() - $argValue->getValue();
        $integerClass = $this->getClass($context, 'Integer');
        return new IntegerValue($result, $integerClass, $this->interpreter);
    }
}