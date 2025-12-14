/**
 * Environment configuration
 * Simple env wrapper for type safety
 */

export const env = {
	NODE_ENV: process.env.NODE_ENV ?? "development",
	DATABASE_URL: process.env.DATABASE_URL ?? "file:./db.sqlite",
} as const;
