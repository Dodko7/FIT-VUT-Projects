/**
 * General purpose type for a promise which may or may not succeed.
 */
export type Result = {
    success: true;
} | {
    success: false;
    error: string;
}

/**
 * The same but for a promise returning a value.
 */
export type TypedResult<T> = {
    success: true;
    value: T;
} | {
    success: false;
    error: string;
}
 