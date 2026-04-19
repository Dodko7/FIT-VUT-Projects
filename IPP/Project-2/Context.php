<?php

declare(strict_types=1);

namespace IPP\Student;

use IPP\Core\Interface\InputReader;
use IPP\Core\Interface\OutputWriter;
use IPP\Student\Exception\InterpretDNUException;
use IPP\Student\Exception\InterpretTypeException;
use IPP\Student\Exception\InterpretValueException;
use IPP\Student\Expression\BlockExpr;
use IPP\Student\Expression\Expression;
use IPP\Student\Expression\Literal;
use IPP\Student\Expression\Send;
use IPP\Student\Expression\Variable;
use IPP\Student\MessageHandler\AsIntegerHandler;
use IPP\Student\MessageHandler\AsStringHandler;
use IPP\Student\MessageHandler\ConcatenateWithHandler;
use IPP\Student\MessageHandler\DivByHandler;
use IPP\Student\MessageHandler\EqualToHandler;
use IPP\Student\MessageHandler\FromHandler;
use IPP\Student\MessageHandler\GreaterThanHandler;
use IPP\Student\MessageHandler\IfTrueIfFalseHandler;
use IPP\Student\MessageHandler\IsBlockHandler;
use IPP\Student\MessageHandler\IsNilHandler;
use IPP\Student\MessageHandler\IsStringHandler;
use IPP\Student\MessageHandler\IdenticalToHandler;
use IPP\Student\MessageHandler\MessageHandlerInterface;
use IPP\Student\MessageHandler\MinusHandler;
use IPP\Student\MessageHandler\MultiplyByHandler;
use IPP\Student\MessageHandler\NewHandler;
use IPP\Student\MessageHandler\NotHandler;
use IPP\Student\MessageHandler\AndHandler;
use IPP\Student\MessageHandler\OrHandler;
use IPP\Student\MessageHandler\PlusHandler;
use IPP\Student\MessageHandler\PrintHandler;
use IPP\Student\MessageHandler\ReadHandler;
use IPP\Student\MessageHandler\SubstringHandler;
use IPP\Student\MessageHandler\TimesRepeatHandler;
use IPP\Student\MessageHandler\ValueHandler;
use IPP\Student\MessageHandler\WhileTrueHandler;
use IPP\Student\Value\BlockValue;
use IPP\Student\Value\BoolValue;
use IPP\Student\Value\IntegerValue;
use IPP\Student\Value\NilValue;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Value\StringValue;

/**
 * Class representing the runtime context of the program.
 */
class Context
{
    /**
     * Array of registered classes, mapped by class name.
     *
     * @var array<string, SolClass>
     */
    private array $classes = [];

    /**
     * Array of message handlers for processing messages.
     *
     * @var array<MessageHandlerInterface>
     */
    private array $messageHandlers = [];

    /**
     * Global frame for storing variables.
     *
     * @var Frame
     */
    private Frame $globalFrame;

    /**
     * Stack of frames for variable scopes.
     *
     * @var array<Frame>
     */
    private array $frames = [];

    /**
     * Singleton instance for nil value.
     *
     * @var SolValue
     */
    private SolValue $nil;

    /**
     * Singleton instance for true value.
     *
     * @var SolValue
     */
    private SolValue $true;

    /**
     * Singleton instance for false value.
     *
     * @var SolValue
     */
    private SolValue $false;

    /**
     * Constructor for the context.
     *
     * @param Interpreter $interpreter The interpreter instance.
     * @param OutputWriter $stderr The error output writer.
     * @param OutputWriter $stdout The standard output writer.
     * @param InputReader $input The input reader.
     */
    public function __construct(
        private readonly Interpreter $interpreter,
        private readonly OutputWriter $stderr,
        private readonly OutputWriter $stdout,
        private readonly InputReader $input
    ) {
        // Initialize global frame without self
        $this->globalFrame = new Frame(null, $this->interpreter);
        $this->frames[] = $this->globalFrame;

        $this->initialize();
    }

    /**
     * Initializes the context (singletons, classes, handlers).
     */
    private function initialize(): void
    {
        $this->initializeSingletons();
        $this->initializeBuiltInClasses();
        $this->initializeMessageHandlers();
    }

    /**
     * Initializes singleton instances (nil, true, false).
     */
    private function initializeSingletons(): void
    {
        $singletonConfigs = [
            'Nil' => [
                'parent' => 'Object',
                'value' => fn(SolClass $class): NilValue => new NilValue($class, $this->interpreter),
            ],
            'True' => [
                'parent' => 'Object',
                'value' => fn(SolClass $class): BoolValue => new BoolValue(true, $class, $this->interpreter),
            ],
            'False' => [
                'parent' => 'Object',
                'value' => fn(SolClass $class): BoolValue => new BoolValue(false, $class, $this->interpreter),
            ],
        ];

        foreach ($singletonConfigs as $name => $config) {
            $class = new SolClass($this->interpreter);
            $class->parseFromData($name, $config['parent']);
            $this->classes[$name] = $class;
            $this->{strtolower($name)} = $config['value']($class);
        }
    }

    /**
     * Initializes built-in classes (Object, Integer, String, Block).
     */
    private function initializeBuiltInClasses(): void
    {
        $builtInClasses = [
            'Object' => null,
            'Integer' => 'Object',
            'String' => 'Object',
            'Block' => 'Object',
        ];

        foreach ($builtInClasses as $name => $parent) {
            $class = new SolClass($this->interpreter);
            $class->parseFromData($name, $parent);
            $this->classes[$name] = $class;
        }
    }

    /**
     * Initializes message handlers.
     */
    private function initializeMessageHandlers(): void
    {
        $this->messageHandlers = [
            new PrintHandler(),
            new PlusHandler($this->interpreter),
            new IfTrueIfFalseHandler(),
            new EqualToHandler(),
            new AsStringHandler($this->interpreter),
            new IsNilHandler(),
            new IsStringHandler(),
            new FromHandler($this->interpreter),
            new NewHandler($this->interpreter),
            new ReadHandler($this->interpreter),
            new MinusHandler($this->interpreter),
            new MultiplyByHandler($this->interpreter),
            new DivByHandler($this->interpreter),
            new AsIntegerHandler($this->interpreter),
            new GreaterThanHandler(),
            new ConcatenateWithHandler($this->interpreter),
            new TimesRepeatHandler($this->interpreter),
            new ValueHandler(),
            new WhileTrueHandler($this->interpreter),
            new IsBlockHandler(),
            new IdenticalToHandler(),
            new SubstringHandler($this->interpreter),
            new NotHandler(),
            new AndHandler(),
            new OrHandler(),
        ];
    }

    /**
     * Creates a subcontext that inherits classes, handlers, and singletons from the current context.
     *
     * @param ObjectValue|null $self The self value for the new frame (optional).
     * @return Context The new subcontext.
     */
    public function createSubContext(?ObjectValue $self = null): Context
    {
        $subContext = new Context(
            $this->interpreter,
            $this->stderr,
            $this->stdout,
            $this->input
        );

        // Copy classes
        $subContext->classes = $this->classes;

        // Copy handlers
        $subContext->messageHandlers = $this->messageHandlers;

        // Copy singletons
        $subContext->nil = $this->nil;
        $subContext->true = $this->true;
        $subContext->false = $this->false;

        // Create new frame with provided self
        $subContext->pushFrame(new Frame($self, $this->interpreter));

        return $subContext;
    }

    /**
     * Returns a boolean value (True or False) based on the input value.
     *
     * @param bool $value The boolean value.
     * @return SolValue Instance of True or False.
     */
    public function getBool(bool $value): SolValue
    {
        return $value ? $this->true : $this->false;
    }

    /**
     * Registers a class in the context.
     *
     * @param string $name The name of the class.
     * @param SolClass $class The class instance.
     */
    public function registerClass(string $name, SolClass $class): void
    {
        $this->classes[$name] = $class;
    }

    /**
     * Returns a class by name.
     *
     * @param string $name The name of the class.
     * @return SolClass|null The class instance, or null if it does not exist.
     */
    public function getClass(string $name): ?SolClass
    {
        return $this->classes[$name] ?? null;
    }

    /**
     * Pushes a new frame onto the stack.
     *
     * @param Frame $frame The new frame.
     */
    public function pushFrame(Frame $frame): void
    {
        $this->frames[] = $frame;
    }

    /**
     * Pops the top frame from the stack.
     */
    public function popFrame(): void
    {
        array_pop($this->frames);
    }

    /**
     * Returns the value of a variable from the current frame.
     *
     * @param string $name The name of the variable.
     * @return SolValue|null The variable's value, or null if it does not exist.
     */
    public function getVariable(string $name): ?SolValue
    {
        return $this->frames ? end($this->frames)->getVariable($name) : null;
    }

    /**
     * Sets the value of a variable in the current frame.
     *
     * @param string $name The name of the variable.
     * @param SolValue $value The value to set.
     */
    public function setVariable(string $name, SolValue $value): void
    {
        if ($this->frames) {
            end($this->frames)->setVariable($name, $value);
        }
    }

    /**
     * Returns the nil singleton.
     *
     * @return SolValue The nil value.
     */
    public function getNil(): SolValue
    {
        return $this->nil;
    }

    /**
     * Returns the true singleton.
     *
     * @return SolValue The true value.
     */
    public function getTrue(): SolValue
    {
        return $this->true;
    }

    /**
     * Returns the false singleton.
     *
     * @return SolValue The false value.
     */
    public function getFalse(): SolValue
    {
        return $this->false;
    }

    /**
     * Returns the list of message handlers.
     *
     * @return array<MessageHandlerInterface> The message handlers.
     */
    public function getMessageHandlers(): array
    {
        return $this->messageHandlers;
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
}