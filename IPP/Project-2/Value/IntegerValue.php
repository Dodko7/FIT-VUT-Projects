<?php

declare(strict_types=1);

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

use IPP\Student\Value\BlockValue;
use IPP\Student\Value\BoolValue;
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
 * Class representing an integer value in the SOL25 language.
 */
class IntegerValue extends SolValue
{
    /**
     * Constructor for the integer value.
     *
     * @param int $value The integer value.
     * @param SolClass $class The class of the value (Integer).
     * @param Interpreter $interpreter The interpreter instance.
     */
    public function __construct(
        private readonly int $value,
        SolClass $class,
        Interpreter $interpreter
    ) {
        parent::__construct($class, $interpreter);
    }

    /**
     * Returns the integer value.
     *
     * @return int The integer value.
     */
    public function getValue(): int
    {
        return $this->value;
    }

    /**
     * Returns the string representation of the value.
     *
     * @return string The string representation of the integer.
     */
    public function printValue(): string
    {
        return (string)$this->value;
    }
}