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
 * Handler for the 'ifTrue:ifFalse:' message.
 *
 * Evaluates a conditional expression by executing the appropriate block based on the
 * boolean value of the receiver.
 */
class IfTrueIfFalseHandler implements MessageHandlerInterface
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
     * @return bool True if the class is 'True' or 'False' and the selector is 'ifTrue:ifFalse:', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return in_array($className, ['True', 'False'], true) && $selector === 'ifTrue:ifFalse:';
    }

    /**
     * Handles the 'ifTrue:ifFalse:' message.
     *
     * Executes the first block if the receiver is true, or the second block if false,
     * in a subcontext preserving the original 'self' variable.
     *
     * @param SolValue $receiver The message receiver (expected to be a BoolValue).
     * @param string $selector The message selector (expected to be 'ifTrue:ifFalse:').
     * @param array<int, SolValue> $arguments The message arguments (exactly two BlockValue instances expected).
     * @param Context $context The runtime context for accessing variables and creating subcontexts.
     * @return SolValue The result of executing the selected block.
     * @throws InterpretTypeException If the argument count is not two, the receiver is not a BoolValue,
     *                               the arguments are not BlockValue instances, or 'self' is not an ObjectValue or null.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (count($arguments) !== 2) {
            throw new InterpretTypeException('Invalid number of arguments for ifTrue:ifFalse:');
        }

        if (!$receiver instanceof BoolValue || !$arguments[0] instanceof BlockValue || !$arguments[1] instanceof BlockValue) {
            throw new InterpretTypeException('Invalid argument types for ifTrue:ifFalse:');
        }

        $block = $receiver->getValue() ? $arguments[0] : $arguments[1];

        // Retrieve the 'self' variable and verify its type
        $self = $context->getVariable('self');
        if ($self !== null && !$self instanceof ObjectValue) {
            throw new InterpretTypeException('Variable self must be an ObjectValue or null');
        }

        // Create a subcontext with the original 'self'
        $blockContext = $context->createSubContext($self);

        // Execute the block in the subcontext
        return $block->sendMessage('value', [], $blockContext);
    }
}