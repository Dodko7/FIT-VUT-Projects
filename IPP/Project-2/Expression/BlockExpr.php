<?php

namespace IPP\Student\Expression;

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

use IPP\Student\Expression\Expression;
use IPP\Student\Expression\Literal;
use IPP\Student\Expression\Send;
use IPP\Student\Expression\Variable;

use IPP\Student\Value\BlockValue;
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
 * Represents a block expression in the SOL25 interpreter.
 *
 * This class encapsulates a block of code (a closure) and evaluates it to a BlockValue,
 * which can be executed later with the appropriate arguments.
 */
class BlockExpr extends Expression
{
    /** @var Block The block of code represented by this expression */
    private Block $block;

    /**
     * Constructor for BlockExpr.
     *
     * Initializes a BlockExpr by creating and parsing a Block from the provided DOM element.
     *
     * @param Interpreter $interpreter The interpreter instance for I/O and debugging.
     * @param DOMElement $element The DOM element representing the block expression.
     */
    public function __construct(Interpreter $interpreter, DOMElement $element)
    {
        parent::__construct($interpreter);
        $this->block = new Block($interpreter);
        // [BlockExpr.php]: Log block creation
        $this->block->parse($element);
        // [BlockExpr.php]: Log block parsing
    }

    /**
     * Evaluate the block expression in the given context.
     *
     * Creates a BlockValue representing the block, which can be executed later via messages
     * like 'value' or 'whileTrue:'.
     *
     * @param Context $context The execution context containing variable bindings and classes.
     * @return SolValue A BlockValue representing the block.
     */
    public function evaluate(Context $context): SolValue
    {
        // [BlockExpr.php]: Log evaluation start
        $result = new BlockValue($this->block, $context, $this->interpreter);
        // [BlockExpr.php]: Log evaluation result
        return $result;
    }
}