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

use IPP\Student\Expression\BlockExpr;
use IPP\Student\Expression\Expression;
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
 * Class representing a literal expression in the SOL25 language.
 *
 * Handles literals such as strings, integers, booleans, nil, or class references,
 * and evaluates them to their corresponding SolValue instances.
 */
class Literal extends Expression
{
    /**
     * @param string $class The class name of the literal (e.g., String, Integer, Nil).
     * @param string $value The value of the literal.
     * @param Interpreter $interpreter The interpreter instance for I/O and debugging.
     */
    public function __construct(private readonly string $class, private readonly string $value, Interpreter $interpreter)
    {
        parent::__construct($interpreter);
    }

    /**
     * Evaluates the literal and returns its value.
     *
     * Converts the literal's class and value into the appropriate SolValue instance
     * based on the class type (e.g., Integer, String, Nil, True, False, or a class reference).
     *
     * @param Context $context The runtime context containing class and value information.
     * @return SolValue The evaluated value of the literal.
     * @throws InterpretTypeException If the literal's class is unknown, does not exist, or the value is invalid.
     */
    public function evaluate(Context $context): SolValue
    {
        if ($this->class === 'class') {
            // Literal represents a class (e.g., String, Integer)
            $targetClass = $context->getClass($this->value);
            if ($targetClass === null) {
                throw new InterpretTypeException("Class not found: {$this->value}");
            }
            return new ObjectValue($targetClass, $this->interpreter);
        }

        $classObj = $context->getClass($this->class);
        if ($classObj === null) {
            throw new InterpretTypeException("Class for literal not found: {$this->class}");
        }

        return match ($this->class) {
            'Nil' => $context->getNil(),
            'True' => $context->getTrue(),
            'False' => $context->getFalse(),
            'Integer' => new IntegerValue(
                is_numeric($this->value) && floor((float)$this->value) === (float)$this->value
                    ? (int)$this->value
                    : throw new InterpretTypeException("Invalid integer value: {$this->value}"),
                $classObj,
                $this->interpreter
            ),
            'String' => new StringValue($this->value, $classObj, $this->interpreter),
            default => throw new InterpretTypeException("Unknown literal class: {$this->class}"),
        };
    }
}