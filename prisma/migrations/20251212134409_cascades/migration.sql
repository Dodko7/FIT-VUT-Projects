-- RedefineTables
PRAGMA defer_foreign_keys=ON;
PRAGMA foreign_keys=OFF;
CREATE TABLE "new_Pawn" (
    "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "position" INTEGER NOT NULL,
    "playerId" INTEGER NOT NULL,
    "inHome" BOOLEAN NOT NULL DEFAULT false,
    CONSTRAINT "Pawn_playerId_fkey" FOREIGN KEY ("playerId") REFERENCES "Player" ("id") ON DELETE CASCADE ON UPDATE CASCADE
);
INSERT INTO "new_Pawn" ("id", "inHome", "playerId", "position") SELECT "id", "inHome", "playerId", "position" FROM "Pawn";
DROP TABLE "Pawn";
ALTER TABLE "new_Pawn" RENAME TO "Pawn";
CREATE TABLE "new_Player" (
    "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "name" TEXT NOT NULL,
    "color" TEXT NOT NULL,
    "gameId" INTEGER NOT NULL,
    "isBot" BOOLEAN NOT NULL,
    "finished" BOOLEAN NOT NULL DEFAULT false,
    CONSTRAINT "Player_gameId_fkey" FOREIGN KEY ("gameId") REFERENCES "Game" ("id") ON DELETE CASCADE ON UPDATE CASCADE
);
INSERT INTO "new_Player" ("color", "finished", "gameId", "id", "isBot", "name") SELECT "color", "finished", "gameId", "id", "isBot", "name" FROM "Player";
DROP TABLE "Player";
ALTER TABLE "new_Player" RENAME TO "Player";
PRAGMA foreign_keys=ON;
PRAGMA defer_foreign_keys=OFF;
