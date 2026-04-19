<?php

namespace IPP\Student\Exception;

use IPP\Core\ReturnCode;
use Throwable;

/**
 * Exception thrown when a value-related error occurs during interpretation.
 */
class InterpretValueException extends \IPP\Core\Exception\IPPException
{
    /**
     * Constructor for InterpretValueException.
     *
     * @param string $message The exception message.
     * @param Throwable|null $previous The previous exception, if any.
     */
    public function __construct(string $message = 'Value error', ?Throwable $previous = null)
    {
        // Initialize the exception with a specific return code for value errors
        parent::__construct($message, ReturnCode::INTERPRET_VALUE_ERROR, $previous, false);
    }
}