<?php

declare(strict_types=1);

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\Interpreter;
use IPP\Student\SolClass;
use IPP\Student\Value\IntegerValue;
use IPP\Student\Value\ObjectValue;
use IPP\Student\Value\SolValue;
use IPP\Student\Value\StringValue;
use IPP\Student\Exception\InterpretTypeException;

/**
 * Handler for the 'new' message to create a new object instance or return a singleton.
 */
class NewHandler implements MessageHandlerInterface
{
    use HandlerTrait;

    /**
     * Constructor for the NewHandler.
     *
     * @param Interpreter $interpreter The interpreter instance.
     */
    public function __construct(private readonly Interpreter $interpreter)
    {
    }

    /**
     * Checks if the handler supports the given message.
     *
     * @param string $className The name of the receiver's class.
     * @param string $selector The message selector.
     * @return bool True if the selector is 'new', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return $selector === 'new';
    }

    /**
     * Handles the 'new' message to create a new instance or return a singleton.
     *
     * @param SolValue $receiver The receiver of the message (class, e.g., True, Object).
     * @param string $selector The message selector ('new').
     * @param array<SolValue> $arguments Array of arguments (expected to be empty).
     * @param Context $context The runtime context.
     * @return SolValue A new instance or singleton (True, False, Nil).
     * @throws InterpretTypeException If arguments are provided or the receiver is not an ObjectValue.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        // Validate that no arguments are provided
        if (!empty($arguments)) {
            throw new InterpretTypeException('new does not accept arguments');
        }

        // Validate the receiver type
        if (!$receiver instanceof ObjectValue) {
            throw new InterpretTypeException('Receiver must be a class instance for new');
        }

        $className = $receiver->getClass()->getName();

        // Return singletons for True, False, Nil
        return match ($className) {
            'True' => $context->getTrue(),
            'False' => $context->getFalse(),
            'Nil' => $context->getNil(),
            default => $this->createNewInstance($receiver->getClass(), $context),
        };
    }

    /**
     * Creates a new instance for a class that is not a singleton.
     *
     * @param SolClass $class The class for which to create an instance.
     * @param Context $context The runtime context.
     * @return ObjectValue A new object instance.
     */
    private function createNewInstance(SolClass $class, Context $context): ObjectValue
    {
        $newInstance = new ObjectValue($class, $this->interpreter);

        // Initialize Integer instances with a default value of 0
        if ($class->getName() === 'Integer' || $class->isSubclassOf('Integer', $context)) {
            $integerClass = $this->getClass($context, 'Integer');
            $newInstance->setAttribute('value', new IntegerValue(0, $integerClass, $this->interpreter), true);
        // Initialize String instances with an empty string
        } elseif ($class->getName() === 'String' || $class->isSubclassOf('String', $context)) {
            $stringClass = $this->getClass($context, 'String');
            $newInstance->setAttribute('value', new StringValue('', $stringClass, $this->interpreter), true);
        }

        return $newInstance;
    }
}