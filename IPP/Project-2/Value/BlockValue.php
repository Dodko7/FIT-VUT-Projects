<?php

namespace IPP\Student\Value;

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
 * Represents a block value in the SOL25 interpreter.
 *
 * This class encapsulates a block of code (a closure) that can be executed with a specified
 * number of arguments. It delegates message handling to registered message handlers.
 */
class BlockValue extends SolValue
{
    /**
     * Constructor for BlockValue.
     *
     * Initializes a BlockValue with the given block of code, execution context, and interpreter.
     *
     * @param Block $block The block of code associated with this value.
     * @param Context $context The execution context for variable and class management.
     * @param Interpreter $interpreter The interpreter instance for I/O and debugging.
     * @throws InterpretTypeException If the Block class is not found in the context.
     */
    public function __construct(
        private readonly Block $block,
        private readonly Context $context,
        Interpreter $interpreter
    ) {
        $blockClass = $context->getClass('Block');
        if ($blockClass === null) {
            throw new InterpretTypeException('Block class not found');
        }
        parent::__construct($blockClass, $interpreter);
    }

    /**
     * Handle messages sent to the block object.
     *
     * Delegates message handling to registered message handlers via the context's dispatcher.
     *
     * @param string $selector The message selector (e.g., 'value', 'value:', 'whileTrue:').
     * @param array<SolValue> $arguments The arguments passed with the message.
     * @param Context $context The execution context for accessing singletons and frames.
     * @return SolValue The result of the message execution.
     * @throws InterpretDNUException If the message is not understood.
     */
    public function sendMessage(string $selector, array $arguments, Context $context): SolValue
    {
        return $this->interpreter->getDispatcher()->dispatch($this, $selector, $arguments, $context);
    }

    /**
     * Check if this value is identical to another value.
     *
     * Compares whether this BlockValue is the exact same instance as another SolValue.
     *
     * @param SolValue $other The value to compare with.
     * @return bool True if the values are identical (same instance), false otherwise.
     */
    public function isIdenticalTo(SolValue $other): bool
    {
        return $this === $other;
    }

    /**
     * Print the block value as a string.
     *
     * Returns a string representation of the block for output or debugging.
     *
     * @return string The string representation of the block ('a block').
     */
    public function printValue(): string
    {
        return 'a block';
    }

    /**
     * Returns the arity of the block.
     *
     * @return int The number of parameters the block expects.
     */
    public function getArity(): int
    {
        return $this->block->getArity();
    }

    /**
     * Returns the block of code associated with this value.
     *
     * @return Block The block of code.
     */
    public function getBlock(): Block
    {
        return $this->block;
    }

    /**
     * Returns the context in which the block was defined.
     *
     * @return Context The context associated with this block.
     */
    public function getContext(): Context
    {
        return $this->context;
    }
}