<?php

namespace IPP\Student\Exception;

use IPP\Core\ReturnCode;
use Throwable;

/**
 * Exception thrown when a message is not understood during interpretation.
 */
class InterpretDNUException extends \IPP\Core\Exception\IPPException
{
    /**
     * Constructor for InterpretDNUException.
     *
     * @param string $message The exception message.
     * @param Throwable|null $previous The previous exception, if any.
     */
    public function __construct(string $message = 'Does not understand message', ?Throwable $previous = null)
    {
        // Initialize the exception with a specific return code for "does not understand" errors
        parent::__construct($message, ReturnCode::INTERPRET_DNU_ERROR, $previous, false);
    }
}