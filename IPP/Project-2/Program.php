<?php

namespace IPP\Student;

use IPP\Student\Assign;
use IPP\Student\Block;
use IPP\Student\Context;
use IPP\Student\Frame;
use IPP\Student\Interpreter;
use IPP\Student\Method;
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
 * Represents a SOL25 program, containing all class definitions.
 */
class Program
{
    /** @var array<string, SolClass> List of all class definitions in the program */
    private array $classes = [];

    /**
     * Program constructor.
     *
     * @param Interpreter $interpreter The interpreter instance used for logging and execution.
     */
    public function __construct(private Interpreter $interpreter)
    {
    }

    /**
     * Parse the program from a DOM document.
     *
     * @param DOMDocument $dom The DOM document representing the program.
     * @throws XMLException If the XML structure is invalid (e.g., missing program root).
     * @throws InterpretTypeException If the Main class is missing (code 31).
     */
    public function parse(DOMDocument $dom): void
    {

        // Get the root element of the XML document
        $root = $dom->documentElement;
        if ($root === null || $root->nodeName !== 'program') {
            throw new XMLException('Invalid XML: missing program root');
        }


        // Parse all class definitions in the program
        foreach ($root->childNodes as $node) {
            if ($node instanceof \DOMElement && $node->nodeName === 'class') {
                $class = new SolClass($this->interpreter);
                $class->parse($node);
                $className = $class->getName();
                $this->classes[$className] = $class;
            }
        }

        // Verify that the Main class is present
        if (!isset($this->classes['Main'])) {
            throw new InterpretTypeException(
                'Missing Main class',
                null,
                \IPP\Core\ReturnCode::PARSE_MAIN_ERROR
            );
        }

    }

    /**
     * Get all classes in the program.
     *
     * @return array<string, SolClass> An associative array of class names to SolClass objects.
     */
    public function getClasses(): array
    {
        return $this->classes;
    }

    /**
     * Get a specific class by its name.
     *
     * @param string $name The name of the class.
     * @return SolClass|null The class object or null if it does not exist.
     */
    public function getClass(string $name): ?SolClass
    {
        return $this->classes[$name] ?? null;
    }
}
