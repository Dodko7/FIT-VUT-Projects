/*
  Warnings:

  - Added the required column `gameId` to the `Pawn` table without a default value. This is not possible if the table is not empty.

*/
-- RedefineTables
PRAGMA defer_foreign_keys=ON;
PRAGMA foreign_keys=OFF;
CREATE TABLE "new_Pawn" (
    "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "position" INTEGER NOT NULL,
    "playerId" INTEGER NOT NULL,
    "gameId" INTEGER NOT NULL,
    "inHome" BOOLEAN NOT NULL DEFAULT false,
    "color" TEXT NOT NULL,
    CONSTRAINT "Pawn_playerId_fkey" FOREIGN KEY ("playerId") REFERENCES "Player" ("id") ON DELETE CASCADE ON UPDATE CASCADE
);
INSERT INTO "new_Pawn" ("color", "id", "inHome", "playerId", "position") SELECT "color", "id", "inHome", "playerId", "position" FROM "Pawn";
DROP TABLE "Pawn";
ALTER TABLE "new_Pawn" RENAME TO "Pawn";
PRAGMA foreign_keys=ON;
PRAGMA defer_foreign_keys=OFF;
