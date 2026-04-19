<?php

declare(strict_types=1);

namespace IPP\Student;

use IPP\Student\Assign;
use IPP\Student\Block;
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
use IPP\Student\MessageHandler\AsIntegerHandler;
use IPP\Student\MessageHandler\GreaterThanHandler;

use DOMDocument;
use DOMElement;

use IPP\Core\AbstractInterpreter;
use IPP\Core\Exception\XMLException;
use IPP\Core\Interface\InputReader;
use IPP\Core\Interface\OutputWriter;

/**
 * Class responsible for dispatching messages sent to objects in SOL25.
 */
class MessageDispatcher
{
    /**
     * Cache for method lookups, mapping class and selector to method or null.
     *
     * @var array<string, Method|null>
     */
    private array $methodCache = [];

    /**
     * Dispatches a message to an object and returns the result.
     *
     * @param SolValue $receiver The receiver of the message.
     * @param string $selector The message selector.
     * @param array<SolValue> $arguments The arguments of the message.
     * @param Context $context The runtime context.
     * @return SolValue The result of the message.
     * @throws InterpretDNUException If the message is not supported.
     * @throws InterpretTypeException If cyclic inheritance or invalid type is detected.
     */
    public function dispatch(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        // Check built-in message handlers
        $handlerResult = $this->findHandler($receiver, $selector, $arguments, $context);
        if ($handlerResult !== null) {
            return $handlerResult;
        }

        // If receiver is ObjectValue, check methods and attributes
        if ($receiver instanceof ObjectValue) {
            // Check methods in class hierarchy
            $methodResult = $this->findMethodInClassHierarchy($receiver, $selector, $arguments, $context);
            if ($methodResult !== null) {
                return $methodResult;
            }

            // Check attributes
            $attributeName = rtrim($selector, ':');
            if (count($arguments) === 0) {
                // No-argument message: return attribute value
                if ($receiver->hasAttribute($attributeName)) {
                    $attributeValue = $receiver->getAttribute($attributeName);
                    if ($attributeValue === null) {
                        throw new InterpretDNUException("Attribute '$attributeName' not set");
                    }
                    return $attributeValue;
                }
            } elseif (count($arguments) === 1) {
                // Single-argument message: set attribute and return self
                $receiver->setAttribute($attributeName, $arguments[0]);
                return $receiver;
            }

            // Delegate to native value for Integer and String subclasses
            $nativeValue = $receiver->getNativeValue();
            if ($nativeValue !== null &&
                ($receiver->getClass()->isSubclassOf('Integer', $context) ||
                 $receiver->getClass()->isSubclassOf('String', $context))) {
                return $nativeValue->sendMessage($selector, $arguments, $context);
            }
        }

        throw new InterpretDNUException("Message '$selector' not understood by " . $receiver->getClass()->getName());
    }

    /**
     * Finds and executes a handler for the message.
     *
     * @param SolValue $receiver The receiver of the message.
     * @param string $selector The message selector.
     * @param array<SolValue> $arguments The arguments of the message.
     * @param Context $context The runtime context.
     * @return SolValue|null The result of the handler, or null if no handler is found.
     */
    private function findHandler(SolValue $receiver, string $selector, array $arguments, Context $context): ?SolValue
    {
        foreach ($context->getMessageHandlers() as $handler) {
            if ($handler->supports($receiver->getClass()->getName(), $selector)) {
                return $handler->handle($receiver, $selector, $arguments, $context);
            }
        }
        return null;
    }

    /**
     * Finds and executes a method in the class hierarchy.
     *
     * @param ObjectValue $receiver The receiver of the message.
     * @param string $selector The message selector.
     * @param array<SolValue> $arguments The arguments of the message.
     * @param Context $context The runtime context.
     * @return SolValue|null The result of the method, or null if no method is found.
     * @throws InterpretTypeException If cyclic inheritance is detected.
     */
    private function findMethodInClassHierarchy(
        ObjectValue $receiver,
        string $selector,
        array $arguments,
        Context $context
    ): ?SolValue {
        $cacheKey = $receiver->getClass()->getName() . '::' . $selector;
        if (array_key_exists($cacheKey, $this->methodCache)) {
            $method = $this->methodCache[$cacheKey];
            return $method ? $method->execute($receiver, $arguments, $context) : null;
        }

        $currentClass = $receiver->getClass();
        $visitedClasses = [];

        while ($currentClass !== null) {
            $className = $currentClass->getName();
            if (in_array($className, $visitedClasses, true)) {
                throw new InterpretTypeException("Cyclic inheritance detected for class '$className'");
            }
            $visitedClasses[] = $className;

            $method = $currentClass->getMethod($selector);
            if ($method !== null) {
                $this->methodCache[$cacheKey] = $method;
                return $method->execute($receiver, $arguments, $context);
            }
            $currentClass = $currentClass->getParent($context);
        }

        $this->methodCache[$cacheKey] = null;
        return null;
    }
}