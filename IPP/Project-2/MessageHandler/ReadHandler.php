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
 * Handler for the 'read' message on the String class.
 */
class ReadHandler implements MessageHandlerInterface
{
    use HandlerTrait;

    /**
     * Constructor for the ReadHandler.
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
        return $className === 'String' && $selector === 'read';
    }

    /**
     * Handles the 'read' message.
     *
     * @param SolValue $receiver The receiver of the message (ObjectValue of String class).
     * @param string $selector The message selector.
     * @param array<SolValue> $arguments The message arguments (expected to be empty).
     * @param Context $context The runtime context.
     * @return SolValue A StringValue containing the read input or empty string if no input.
     * @throws InterpretTypeException If the receiver is invalid or arguments are provided.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        // Validate receiver and arguments
        if (!$receiver instanceof ObjectValue || $receiver->getClass()->getName() !== 'String' || !empty($arguments)) {
            throw new InterpretTypeException('Invalid receiver or arguments for read');
        }

        // Read input string
        $input = $context->getInput()->readString();
        $stringClass = $this->getClass($context, 'String');

        // Return StringValue with input or empty string if input is null
        return new StringValue($input ?? '', $stringClass, $this->interpreter);
    }
}