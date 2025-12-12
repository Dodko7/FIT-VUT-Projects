import type { NextRequest } from "next/server";
import { db as prisma } from "~/server/db";

/**
 * DELETE handler to delete a Ludo game by name.
 * @param request The incoming request.
 * @param param1 Game name parameter from the URL.
 * @returns A response indicating the result of the delete operation.
 */
export async function DELETE(
    request: NextRequest,
	{ params }: { params: Promise<{ gameName: string }> },
) {
	const gameName = (await params).gameName;

	if (!gameName) {
		return new Response(`Invalid game name: ${gameName}`, { status: 400 });
	}

	try {
		await prisma.game.delete({
			where: { name: gameName },
		});
		return new Response(null, { status: 204 });
	} catch (error) {
		return new Response(
			`Failed to delete game with name ${gameName}: ${(error as Error).message}`,
			{ status: 500 },
		);
	}
}
