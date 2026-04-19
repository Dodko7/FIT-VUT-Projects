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
 * Handler for the 'plus:' message to perform addition of two numbers.
 */
class PlusHandler implements MessageHandlerInterface
{
    use HandlerTrait;

    /**
     * Constructor for the PlusHandler.
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
     * @return bool True if the message is supported, false otherwise.
     */
    public function supports(string $class, string $selector): bool
    {
        return $class === 'Integer' && $selector === 'plus:';
    }

    /**
     * Handles the 'plus:' message.
     *
     * @param SolValue $receiver The receiver of the message (IntegerValue).
     * @param string $selector The message selector.
     * @param array<SolValue> $arguments The message arguments (single IntegerValue).
     * @param Context $context The runtime context.
     * @return SolValue An IntegerValue containing the sum.
     * @throws InterpretTypeException If the receiver or arguments are invalid.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        // Validate receiver and arguments
        if (!$receiver instanceof IntegerValue || count($arguments) !== 1 || !$arguments[0] instanceof IntegerValue) {
            throw new InterpretTypeException('Invalid arguments for plus:');
        }

        // Calculate the sum
        $result = $receiver->getValue() + $arguments[0]->getValue();
        $integerClass = $this->getClass($context, 'Integer');

        // Return new IntegerValue with the result
        return new IntegerValue($result, $integerClass, $this->interpreter);
    }
}