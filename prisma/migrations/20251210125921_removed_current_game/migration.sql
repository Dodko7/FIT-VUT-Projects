/*
  Warnings:

  - You are about to drop the `CurrentGame` table. If the table is not empty, all the data it contains will be lost.

*/
-- DropIndex
DROP INDEX "CurrentGame_gameId_key";

-- AlterTable
ALTER TABLE "Game" ADD COLUMN "diceRoll" INTEGER;

-- DropTable
PRAGMA foreign_keys=off;
DROP TABLE "CurrentGame";
PRAGMA foreign_keys=on;

-- RedefineTables
PRAGMA defer_foreign_keys=ON;
PRAGMA foreign_keys=OFF;
CREATE TABLE "new_Player" (
    "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "name" TEXT NOT NULL,
    "color" TEXT NOT NULL,
    "gameId" INTEGER NOT NULL,
    "isBot" BOOLEAN NOT NULL,
    "finished" BOOLEAN NOT NULL DEFAULT false,
    CONSTRAINT "Player_gameId_fkey" FOREIGN KEY ("gameId") REFERENCES "Game" ("id") ON DELETE RESTRICT ON UPDATE CASCADE
);
INSERT INTO "new_Player" ("color", "finished", "gameId", "id", "isBot", "name") SELECT "color", "finished", "gameId", "id", "isBot", "name" FROM "Player";
DROP TABLE "Player";
ALTER TABLE "new_Player" RENAME TO "Player";
PRAGMA foreign_keys=ON;
PRAGMA defer_foreign_keys=OFF;
