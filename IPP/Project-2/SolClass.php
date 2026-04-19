<?php

namespace IPP\Student;

use IPP\Student\Assign;
use IPP\Student\Block;
use IPP\Student\Context;
use IPP\Student\Frame;
use IPP\Student\Interpreter;
use IPP\Student\Method;
use IPP\Student\Program;

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
 * Represents a SOL class with optional inheritance and methods.
 */
class SolClass
{
    private string $name;
    private ?string $parent = null;
    /** @var array<string, Method> */
    private array $methods = [];
    /** @var array<string, SolValue> */
    private array $attributes = [];

    /**
     * SolClass constructor.
     *
     * @param Interpreter $interpreter Interpreter instance for debug output.
     */
    public function __construct(private Interpreter $interpreter)
    {
    }

    /**
     * Parse the class definition from a DOM element.
     *
     * @param DOMElement $element The DOM element representing the class.
     */
    public function parse(DOMElement $element): void
    {
        $this->name = $element->getAttribute('name');

        if ($element->hasAttribute('parent')) {
            $this->parent = $element->getAttribute('parent');
        }

        // Parse all methods defined in the class
        foreach ($element->childNodes as $node) {
            if ($node instanceof \DOMElement && $node->nodeName === 'method') {
                $method = new Method($this->interpreter);
                $method->parse($node);
                $selector = $method->getSelector();
                $this->methods[$selector] = $method;
            }
        }
    }

    /**
     * Parse a built-in class from provided data.
     *
     * @param string $name The name of the class.
     * @param string|null $parent The name of the parent class (if any).
     */
    public function parseFromData(string $name, ?string $parent): void
    {
        $this->name = $name;
        $this->parent = $parent;
    }

    /**
     * Check if this class is a subclass of the specified class.
     *
     * @param string $parentName The name of the potential parent class.
     * @param Context $context The execution context.
     * @return bool True if this class is a subclass of the specified class, false otherwise.
     */
    public function isSubclassOf(string $parentName, Context $context): bool
    {
        if ($this->name === $parentName) {
            return true;
        }
        $current = $this;
        while ($current->parent !== null) {
            $current = $current->getParent($context);
            if ($current === null) {
                return false;
            }
            if ($current->name === $parentName) {
                return true;
            }
        }
        return false;
    }

    /**
     * Get the name of the class.
     *
     * @return string The name of the class.
     */
    public function getName(): string
    {
        return $this->name;
    }

    /**
     * Get the parent class of this class.
     *
     * @param Context $context The execution context.
     * @return SolClass|null The parent class or null if no parent exists.
     */
    public function getParent(Context $context): ?SolClass
    {
        return $this->parent ? $context->getClass($this->parent) : null;
    }

    /**
     * Check if the class has a method with the given selector.
     *
     * @param string $selector The method selector.
     * @return bool True if the method exists, false otherwise.
     */
    public function hasMethod(string $selector): bool
    {
        $result = isset($this->methods[$selector]);
        return $result;
    }

    /**
     * Get a method by its selector.
     *
     * @param string $selector The method selector.
     * @return Method|null The method or null if it does not exist.
     */
    public function getMethod(string $selector): ?Method
    {
        return $this->methods[$selector] ?? null;
    }

    /**
     * Create an instance of this class.
     *
     * @param Context $context The execution context.
     * @return ObjectValue The created instance.
     */
    public function createInstance(Context $context): ObjectValue
    {
        return new ObjectValue($this, $this->interpreter);
    }

    /**
     * Get the attributes of the class.
     *
     * @return array<string, SolValue> The attributes of the class.
     */
    public function getAttributes(): array
    {
        return $this->attributes;
    }
}