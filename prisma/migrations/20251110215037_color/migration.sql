-- RedefineTables
PRAGMA defer_foreign_keys=ON;
PRAGMA foreign_keys=OFF;
CREATE TABLE "new_Game" (
    "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "name" TEXT NOT NULL,
    "lastPlayed" DATETIME NOT NULL,
    "turn" TEXT NOT NULL DEFAULT 'RED',
    "musicOn" BOOLEAN NOT NULL DEFAULT false,
    "isTemp" BOOLEAN NOT NULL DEFAULT false
);
INSERT INTO "new_Game" ("id", "isTemp", "lastPlayed", "musicOn", "name", "turn") SELECT "id", "isTemp", "lastPlayed", "musicOn", "name", "turn" FROM "Game";
DROP TABLE "Game";
ALTER TABLE "new_Game" RENAME TO "Game";
PRAGMA foreign_keys=ON;
PRAGMA defer_foreign_keys=OFF;
