<?php

namespace IPP\Student;

use IPP\Student\Assign;
use IPP\Student\Block;
use IPP\Student\Context;
use IPP\Student\Frame;
use IPP\Student\Interpreter;
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

use IPP\Student\Value\BlockValue;
use IPP\Student\Value\BoolValue;
use IPP\Student\Value\IntegerValue;
use IPP\Student\Value\NilValue;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Value\StringValue;

use IPP\Student\MessageHandler\AsStringHandler;
use IPP\Student\MessageHandler\AttrHandler;
use IPP\Student\MessageHandler\AttrSetHandler;
use IPP\Student\MessageHandler\EqualToHandler;
use IPP\Student\MessageHandler\FromHandler;
use IPP\Student\MessageHandler\IfTrueIfFalseHandler;
use IPP\Student\MessageHandler\IsNilHandler;
use IPP\Student\MessageHandler\IsStringHandler;
use IPP\Student\MessageHandler\MessageHandlerInterface;
use IPP\Student\MessageHandler\NewHandler;
use IPP\Student\MessageHandler\PlusHandler;
use IPP\Student\MessageHandler\PrintHandler;
use IPP\Student\MessageHandler\ReadHandler;
use IPP\Student\MessageHandler\MinusHandler;
use IPP\Student\MessageHandler\MultiplyByHandler;
use IPP\Student\MessageHandler\DivByHandler;

use DOMDocument;
use DOMElement;

use IPP\Core\AbstractInterpreter;
use IPP\Core\Exception\XMLException;
use IPP\Core\Interface\InputReader;
use IPP\Core\Interface\OutputWriter;

class Method
{
    /** @var string The method selector (its name) */
    private string $selector;

    /** @var Block The method body as a block */
    private Block $block;

    /**
     * Constructor for the Method class.
     *
     * @param Interpreter $interpreter The interpreter instance used for logging and execution.
     */
    public function __construct(private Interpreter $interpreter)
    {
    }

    /**
     * Parse the method definition from a DOM element.
     *
     * @param DOMElement $element The DOM element representing the method.
     * @throws XMLException If the selector or block is missing.
     */
    public function parse(DOMElement $element): void
    {

        // Retrieve and validate selector
        $this->selector = $element->getAttribute('selector');
        if (empty($this->selector)) {
            throw new XMLException('Missing selector in method');
        }

        // Find and parse block element
        foreach ($element->childNodes as $node) {
            if ($node instanceof \DOMElement && $node->nodeName === 'block') {
                $this->block = new Block($this->interpreter);
                $this->block->parse($node);
                break;
            }
        }

        // Check that block is set
        if (!isset($this->block)) {
            throw new XMLException('Missing block in method');
        }

    }

    /**
     * Get the selector (name) of the method.
     *
     * @return string The selector of the method.
     */
    public function getSelector(): string
    {
        return $this->selector;
    }

    /**
     * Execute the method using the given receiver object and arguments in a specific context.
     *
     * @param ObjectValue $receiver The object on which the method is called.
     * @param SolValue[] $arguments The arguments passed to the method.
     * @param Context $context The context in which the method executes.
     * @return SolValue The result of the method execution.
     */
    public function execute(ObjectValue $receiver, array $arguments, Context $context): SolValue
    {

        $result = $this->block->execute($receiver, $arguments, $context);

        return $result;
    }
}
