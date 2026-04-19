<?php

declare(strict_types=1);

namespace IPP\Student;

use IPP\Student\Assign;
use IPP\Student\Block;
use IPP\Student\Context;
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

/**
 * Class representing a frame (local variable scope) during method execution.
 */
class Frame
{
    /**
     * Associative array storing variables in the frame.
     *
     * @var array<string, SolValue>
     */
    private array $variables = [];

    /**
     * Constructor for the frame.
     *
     * @param ObjectValue|null $self The 'self' object in the current context, null for global frame.
     * @param Interpreter $interpreter Reference to the interpreter instance.
     */
    public function __construct(
        private ?ObjectValue $self,
        private readonly Interpreter $interpreter
    ) {
        if ($self !== null) {
            $this->variables['self'] = $self;
            $this->variables['super'] = $self; // Assumption for 'super'
        }
    }

    /**
     * Returns the 'self' object assigned to this frame.
     *
     * @return ObjectValue|null The 'self' object, or null if not defined.
     */
    public function getSelf(): ?ObjectValue
    {
        return $this->self;
    }

    /**
     * Sets a variable in the frame.
     *
     * @param string $name The name of the variable.
     * @param SolValue $value The value to assign to the variable.
     */
    public function setVariable(string $name, SolValue $value): void
    {
        $this->variables[$name] = $value;
    }

    /**
     * Retrieves the value of a variable from the frame.
     *
     * Special behavior for 'self' and 'super' keywords:
     * - 'self' returns the current object.
     * - 'super' returns a new instance of the parent class or NilValue if no parent exists.
     *
     * @param string $name The name of the variable.
     * @return SolValue|null The value of the variable, or null if it does not exist.
     * @throws InterpretTypeException If the context for 'self' or 'super' cannot be obtained.
     */
    public function getVariable(string $name): ?SolValue
    {
        // Handle special 'self' variable
        if ($name === 'self') {
            $context = $this->interpreter->getContext();
            if ($context === null) {
                throw new InterpretTypeException("Context is not available for 'self'");
            }
            return $this->self ?? $context->getNil();
        }

        // Handle special 'super' variable
        if ($name === 'super') {
            if ($this->self === null) {
                $context = $this->interpreter->getContext();
                if ($context === null) {
                    throw new InterpretTypeException("Context is not available for 'super'");
                }
                return $context->getNil();
            }

            // Ensure 'self' has access to context and valid class
            $class = $this->self->getClass();
            $context = $this->interpreter->getContext();
            if ($context === null) {
                throw new InterpretTypeException("Context is not available for 'super'");
            }
            $parentClass = $class->getParent($context);

            $result = $parentClass
                ? new ObjectValue($parentClass, $this->interpreter)
                : $context->getNil();

            return $result;
        }

        // Return regular variable if it exists
        return $this->variables[$name] ?? null;
    }
}