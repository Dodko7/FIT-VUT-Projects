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
 * Interface for message handlers in the SOL25 language.
 *
 * Defines the contract for handling messages sent to objects in the SOL25 interpreter.
 * Implementations must determine if they support a given message selector for a class
 * and provide the logic to handle the message execution.
 */
interface MessageHandlerInterface
{
    /**
     * Checks if the handler supports a given message for a specific class.
     *
     * @param string $className The name of the class to check.
     * @param string $selector The message selector (e.g., 'minus:').
     * @return bool True if the handler supports the message, false otherwise.
     */
    public function supports(string $className, string $selector): bool;

    /**
     * Handles a message sent to an object.
     *
     * Processes the message by applying the handler's logic to the receiver and arguments,
     * using the provided execution context to access class information and state.
     *
     * @param SolValue $receiver The receiver of the message (e.g., an object or value).
     * @param string $selector The message selector to handle.
     * @param array<int, SolValue> $arguments The arguments passed with the message.
     * @param Context $context The runtime context for accessing class and frame information.
     * @return SolValue The result of handling the message.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue;
}