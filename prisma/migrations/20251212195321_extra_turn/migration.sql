/*
  Warnings:

  - You are about to drop the column `musicOn` on the `Game` table. All the data in the column will be lost.

*/
-- RedefineTables
PRAGMA defer_foreign_keys=ON;
PRAGMA foreign_keys=OFF;
CREATE TABLE "new_Game" (
    "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "name" TEXT NOT NULL,
    "lastPlayed" DATETIME NOT NULL,
    "turn" TEXT NOT NULL DEFAULT 'RED',
    "extraTurn" BOOLEAN NOT NULL DEFAULT false
);
INSERT INTO "new_Game" ("id", "lastPlayed", "name", "turn") SELECT "id", "lastPlayed", "name", "turn" FROM "Game";
DROP TABLE "Game";
ALTER TABLE "new_Game" RENAME TO "Game";
CREATE UNIQUE INDEX "Game_name_key" ON "Game"("name");
PRAGMA foreign_keys=ON;
PRAGMA defer_foreign_keys=OFF;
