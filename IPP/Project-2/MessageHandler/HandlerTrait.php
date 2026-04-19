<?php

namespace IPP\Student\MessageHandler;

use IPP\Student\Context;
use IPP\Student\SolClass;
use IPP\Student\Exception\InterpretTypeException;

/**
 * Trait providing helper methods for message handlers.
 *
 * Offers utility functions to simplify common operations in message handlers, such as
 * retrieving class instances from the context.
 */
trait HandlerTrait
{
    /**
     * Retrieves a class from the context, ensuring it exists.
     *
     * @param Context $context The runtime context containing class information.
     * @param string $className The name of the class to retrieve (e.g., 'Integer', 'String').
     * @return SolClass The class instance.
     * @throws InterpretTypeException If the class does not exist in the context.
     */
    protected function getClass(Context $context, string $className): SolClass
    {
        $class = $context->getClass($className);
        if ($class === null) {
            throw new InterpretTypeException("Class '$className' not found in context");
        }
        return $class;
    }
}