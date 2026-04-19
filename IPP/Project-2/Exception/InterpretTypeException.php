<?php

namespace IPP\Student\Exception;

use IPP\Core\ReturnCode;
use Throwable;

/**
 * Exception thrown when a type-related error occurs during interpretation.
 */
class InterpretTypeException extends \IPP\Core\Exception\IPPException
{
    /**
     * Constructor for InterpretTypeException.
     *
     * @param string $message The exception message.
     * @param Throwable|null $previous The previous exception, if any.
     * @param int $code The return code for the exception (default is INTERPRET_TYPE_ERROR).
     */
    public function __construct(string $message = 'Type error', ?Throwable $previous = null, int $code = ReturnCode::INTERPRET_TYPE_ERROR)
    {
        // Initialize the exception with the provided message, code, and previous exception
        parent::__construct($message, $code, $previous, false);
    }
}