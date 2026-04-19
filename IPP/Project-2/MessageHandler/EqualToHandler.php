<?php

namespace IPP\Student\MessageHandler;

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

use IPP\Student\Value\BoolValue;
use IPP\Student\Value\BlockValue;
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
 * Handler for the 'equalTo:' message to compare two objects or values.
 *
 * Compares the receiver with the provided argument for equality, returning a BoolValue
 * representing the result of the comparison.
 */
class EqualToHandler implements MessageHandlerInterface
{
    /**
     * Checks if the handler supports the given message.
     *
     * @param string $className The class name of the message receiver.
     * @param string $selector The message selector.
     * @return bool True if the selector is 'equalTo:', false otherwise.
     */
    public function supports(string $className, string $selector): bool
    {
        return $selector === 'equalTo:';
    }

    /**
     * Handles the 'equalTo:' message.
     *
     * Compares the receiver with the provided argument, returning a BoolValue indicating
     * whether they are equal based on their types and values.
     *
     * @param SolValue $receiver The message receiver (e.g., IntegerValue, ObjectValue).
     * @param string $selector The message selector (expected to be 'equalTo:').
     * @param array<int, SolValue> $arguments The message arguments (exactly one expected).
     * @param Context $context The runtime context for accessing boolean values.
     * @return SolValue A BoolValue representing the result of the comparison.
     * @throws InterpretTypeException If exactly one argument is not provided.
     */
    public function handle(SolValue $receiver, string $selector, array $arguments, Context $context): SolValue
    {
        if (count($arguments) !== 1) {
            throw new InterpretTypeException('equalTo: requires exactly one argument');
        }

        $arg = $arguments[0];

        // Extract comparable values (e.g., 'value' attribute for ObjectValue)
        $receiverValue = $receiver instanceof ObjectValue ? $receiver->getAttribute('value') ?? $receiver : $receiver;
        $argValue = $arg instanceof ObjectValue ? $arg->getAttribute('value') ?? $arg : $arg;

        // Compare values
        return $context->getBool($this->compareValues($receiverValue, $argValue));
    }

    /**
     * Compares two values based on their type and returns whether they are equal.
     *
     * @param SolValue $value1 The first value to compare.
     * @param SolValue $value2 The second value to compare.
     * @return bool True if the values are equal, false otherwise.
     */
    private function compareValues(SolValue $value1, SolValue $value2): bool
    {
        // Different types are not equal
        if (get_class($value1) !== get_class($value2)) {
            return false;
        }

        if ($value1 instanceof IntegerValue && $value2 instanceof IntegerValue) {
            return $value1->getValue() === $value2->getValue();
        }

        if ($value1 instanceof StringValue && $value2 instanceof StringValue) {
            return $value1->getValue() === $value2->getValue();
        }

        if ($value1 instanceof BoolValue && $value2 instanceof BoolValue) {
            return $value1->getValue() === $value2->getValue();
        }

        if ($value1 instanceof NilValue && $value2 instanceof NilValue) {
            return $value1->isIdenticalTo($value2);
        }

        if ($value1 instanceof ObjectValue && $value2 instanceof ObjectValue) {
            return $this->compareObjects($value1, $value2);
        }

        // Fallback comparison for other types
        return $value1->printValue() === $value2->printValue();
    }

    /**
     * Compares two ObjectValue instances based on their attributes.
     *
     * @param ObjectValue $object1 The first object to compare.
     * @param ObjectValue $object2 The second object to compare.
     * @return bool True if the objects are equal, false otherwise.
     */
    private function compareObjects(ObjectValue $object1, ObjectValue $object2): bool
    {
        // Different classes are not equal
        if ($object1->getClass()->getName() !== $object2->getClass()->getName()) {
            return false;
        }

        // Get attributes
        $attrs1 = $this->getAllAttributes($object1);
        $attrs2 = $this->getAllAttributes($object2);

        // Different attribute keys are not equal
        if (array_keys($attrs1) !== array_keys($attrs2)) {
            return false;
        }

        // Compare attribute values
        foreach ($attrs1 as $key => $value) {
            if (!$this->compareValues($value, $attrs2[$key])) {
                return false;
            }
        }

        return true;
    }

    /**
     * Retrieves all attributes of an object as an associative array.
     *
     * @param ObjectValue $object The object whose attributes are to be retrieved.
     * @return array<string, SolValue> An array of the object's attributes.
     */
    private function getAllAttributes(ObjectValue $object): array
    {
        // Use reflection to access attributes without depending on internal structure
        $reflection = new \ReflectionClass($object);
        $attributesProperty = $reflection->getProperty('attributes');
        $attributesProperty->setAccessible(true);
        /** @var array<string, SolValue> $attributes */
        $attributes = $attributesProperty->getValue($object);
        $attributesProperty->setAccessible(false);

        return $attributes;
    }
}