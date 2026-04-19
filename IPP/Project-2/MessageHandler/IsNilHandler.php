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
 * Handler for the 'isNil' message to check if a value is Nil.
 *
 * Determines whether the receiver is a NilValue, returning a BoolValue with the result.
 */
class IsNilHandler implements MessageHandlerInterface
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
     * @return bool True if the class is Object, Integer, String, Block, True, False, or Nil and the selector is 'isNil', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return in_array($className, ['Object', 'Integer', 'String', 'Block', 'True', 'False', 'Nil'], true) && $selector === 'isNil';
    }

    /**
     * Handles the 'isNil' message.
     *
     * Checks if the receiver is a NilValue and returns a BoolValue indicating the result.
     *
     * @param SolValue $receiver The message receiver.
     * @param string $selector The message selector (expected to be 'isNil').
     * @param array<int, SolValue> $arguments The message arguments (expected to be empty).
     * @param Context $context The runtime context for accessing boolean values.
     * @return SolValue A BoolValue (true if the receiver is NilValue, false otherwise).
     * @throws InterpretTypeException If any arguments are provided.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (count($arguments) !== 0) {
            throw new InterpretTypeException('Invalid number of arguments for isNil');
        }

        return $receiver instanceof NilValue ? $context->getTrue() : $context->getFalse();
    }
}