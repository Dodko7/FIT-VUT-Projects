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
 * Handler for the 'multiplyBy:' message to perform multiplication of two numbers.
 */
class MultiplyByHandler implements MessageHandlerInterface
{
    use HandlerTrait;

    /**
     * Constructor for the MultiplyByHandler.
     *
     * @param Interpreter $interpreter The interpreter instance.
     */
    public function __construct(private readonly Interpreter $interpreter)
    {
    }

    /**
     * Checks if the handler supports the given class and selector.
     *
     * @param string $class The name of the class.
     * @param string $selector The message selector.
     * @return bool True if the class is 'Integer' and the selector is 'multiplyBy:', false otherwise.
     */
    public function supports(string $class, string $selector): bool
    {
        return $class === 'Integer' && $selector === 'multiplyBy:';
    }

    /**
     * Handles the 'multiplyBy:' message.
     *
     * @param SolValue $receiver The receiver of the message (IntegerValue or ObjectValue derived from Integer).
     * @param string $selector The message selector.
     * @param array<SolValue> $arguments The message arguments (one IntegerValue or ObjectValue derived from Integer).
     * @param Context $context The runtime context.
     * @return SolValue The result of multiplication as an IntegerValue.
     * @throws InterpretTypeException If the arguments are invalid.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        // Validate the number of arguments
        if (count($arguments) !== 1) {
            throw new InterpretTypeException('multiplyBy: requires exactly one argument');
        }

        // Retrieve the receiver's value
        $receiverValue = $this->getIntegerValue($receiver, $context);
        // Retrieve the argument's value
        $argumentValue = $this->getIntegerValue($arguments[0], $context);

        // Calculate the product
        $result = $receiverValue->getValue() * $argumentValue->getValue();
        $integerClass = $this->getClass($context, 'Integer');

        // Return new IntegerValue with the result
        return new IntegerValue($result, $integerClass, $this->interpreter);
    }

    /**
     * Retrieves an IntegerValue from a given value, supporting IntegerValue and ObjectValue derived from Integer.
     *
     * @param SolValue $value The value to process.
     * @param Context $context The runtime context.
     * @return IntegerValue The extracted integer value.
     * @throws InterpretTypeException If the value is invalid.
     */
    private function getIntegerValue(SolValue $value, Context $context): IntegerValue
    {
        // Handle direct IntegerValue
        if ($value instanceof IntegerValue) {
            return $value;
        }

        // Handle ObjectValue derived from Integer
        if ($value instanceof ObjectValue && $value->getClass()->isSubclassOf('Integer', $context)) {
            $innerValue = $value->getAttribute('value');
            if ($innerValue instanceof IntegerValue) {
                return $innerValue;
            }
            throw new InterpretTypeException('ObjectValue must have an IntegerValue attribute "value" for multiplyBy:');
        }

        // Throw exception for invalid types
        throw new InterpretTypeException('Invalid type for multiplyBy:, expected IntegerValue or ObjectValue derived from Integer');
    }
}