<?php

declare(strict_types=1);

namespace IPP\Student;

use IPP\Student\Assign;
use IPP\Student\Block;
use IPP\Student\Context;
use IPP\Student\Frame;
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
 * Main interpreter class for the SOL25 language.
 */
class Interpreter extends AbstractInterpreter
{
    /**
     * Program structure parsed from XML input.
     *
     * @var Program
     */
    private Program $program;

    /**
     * Runtime context for the interpreter, null until initialized.
     *
     * @var Context|null
     */
    private ?Context $context = null;

    /**
     * Message dispatcher for handling messages, lazily initialized.
     *
     * @var MessageDispatcher|null
     */
    private ?MessageDispatcher $dispatcher = null;

    /**
     * Main method to execute the interpreter.
     * Handles parsing and execution of the program defined in XML input.
     *
     * @return int Return code (0 for success, otherwise error code).
     * @throws XMLException If XML parsing fails.
     * @throws InterpretDNUException If a message is not understood.
     * @throws InterpretTypeException If type errors or missing Main class/method occur.
     * @throws InterpretValueException If invalid values are encountered.
     */
    public function execute(): int
    {
        try {
            // Load the XML document
            $dom = $this->source->getDOMDocument();

            // Parse the program structure from XML
            $this->program = new Program($this);
            $this->program->parse($dom);

            // Initialize the runtime context
            $this->context = new Context($this, $this->stderr, $this->stdout, $this->input);

            // Register user-defined classes
            foreach ($this->program->getClasses() as $className => $class) {
                $this->context->registerClass($className, $class);
            }

            // Verify the Main class exists
            $mainClass = $this->context->getClass('Main');
            if ($mainClass === null) {
                throw new InterpretTypeException(
                    'Missing Main class',
                    null,
                    \IPP\Core\ReturnCode::PARSE_MAIN_ERROR
                );
            }

            // Verify the run method exists in Main class
            $runMethod = $mainClass->getMethod('run');
            if ($runMethod === null) {
                throw new InterpretTypeException(
                    'Run method not found in class Main',
                    null,
                    \IPP\Core\ReturnCode::PARSE_MAIN_ERROR
                );
            }

            // Create an instance of Main class
            $mainInstance = new ObjectValue($mainClass, $this);

            // Execute the run method
            $runMethod->execute($mainInstance, [], $this->context);

            // Successful execution
            return 0;

        } catch (XMLException $e) {
            // Handle XML parsing errors
            throw $e;
        } catch (InterpretDNUException | InterpretTypeException | InterpretValueException $e) {
            // Handle interpreter-specific exceptions
            throw $e;
        } catch (\Error $e) {
            // Handle internal errors, e.g., missing class
            throw new InterpretTypeException(
                'Internal error: ' . $e->getMessage(),
                $e,
                \IPP\Core\ReturnCode::INTERNAL_ERROR
            );
        } catch (\Throwable $e) {
            // Handle unexpected errors
            throw new InterpretTypeException(
                'Internal error: ' . $e->getMessage(),
                $e,
                \IPP\Core\ReturnCode::INTERNAL_ERROR
            );
        }
    }

    /**
     * Returns the current runtime context.
     *
     * @return Context|null The current context, or null if not initialized.
     */
    public function getContext(): ?Context
    {
        return $this->context;
    }

    /**
     * Returns the standard output writer.
     *
     * @return OutputWriter The standard output writer.
     */
    public function getStdout(): OutputWriter
    {
        return $this->stdout;
    }

    /**
     * Returns the error output writer.
     *
     * @return OutputWriter The error output writer.
     */
    public function getStderr(): OutputWriter
    {
        return $this->stderr;
    }

    /**
     * Returns the input reader.
     *
     * @return InputReader The input reader.
     */
    public function getInput(): InputReader
    {
        return $this->input;
    }

    /**
     * Returns the message dispatcher.
     *
     * @return MessageDispatcher The message dispatcher for handling messages.
     */
    public function getDispatcher(): MessageDispatcher
    {
        if ($this->dispatcher === null) {
            $this->dispatcher = new MessageDispatcher();
        }
        return $this->dispatcher;
    }
}