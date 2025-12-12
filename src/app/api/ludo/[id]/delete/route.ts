import type { NextRequest } from "next/server";
import { db as prisma } from "~/server/db";

/**
 * DELETE handler to delete a Ludo game by ID.
 * @param request The incoming request.
 * @param param1 Game ID parameter from the URL.
 * @returns A response indicating the result of the delete operation.
 */
export async function DELETE(
    request: NextRequest,
	{ params }: { params: Promise<{ id: string }> },
) {
	const id = parseInt((await params).id, 10);

	if (isNaN(id)) {
		return new Response(`Invalid game ID: ${id}`, { status: 400 });
	}

	try {
		await prisma.game.delete({
			where: { id: id },
		});
		return new Response(null, { status: 204 });
	} catch (error) {
		console.error("Error deleting game:", error);
		return new Response(
			`Failed to delete game with ID ${id}: ${(error as Error).message}`,
			{ status: 500 },
		);
	}
}
