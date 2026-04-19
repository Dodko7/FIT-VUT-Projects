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
 * Handler for the 'divBy:' message to perform integer division.
 *
 * Divides the integer value of the receiver by the integer value of the argument,
 * returning the result as an IntegerValue.
 */
class DivByHandler implements MessageHandlerInterface
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
     * @return bool True if the class is 'Integer' and the selector is 'divBy:', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return $className === 'Integer' && $selector === 'divBy:';
    }

    /**
     * Handles the 'divBy:' message.
     *
     * Performs integer division of the receiver by the argument, returning the result
     * as an IntegerValue. Throws an exception if the divisor is zero or arguments are invalid.
     *
     * @param SolValue $receiver The message receiver (expected to be an IntegerValue).
     * @param string $selector The message selector (expected to be 'divBy:').
     * @param array<int, SolValue> $arguments The message arguments (exactly one IntegerValue expected).
     * @param Context $context The runtime context for accessing class information.
     * @return SolValue An IntegerValue representing the result of the division.
     * @throws InterpretTypeException If the receiver or argument is not an IntegerValue or the argument count is not one.
     * @throws InterpretValueException If the divisor is zero.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (!$receiver instanceof IntegerValue || count($arguments) !== 1 || !$arguments[0] instanceof IntegerValue) {
            throw new InterpretTypeException('Invalid arguments for divBy:');
        }

        $divisor = $arguments[0]->getValue();
        if ($divisor === 0) {
            throw new InterpretValueException('Division by zero in divBy:');
        }

        $result = intdiv($receiver->getValue(), $divisor);
        $integerClass = $this->getClass($context, 'Integer');
        return new IntegerValue($result, $integerClass, $this->interpreter);
    }
}