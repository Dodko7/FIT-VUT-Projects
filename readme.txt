Adresárová štruktúra je rozdelená primárne na tri hry, z ktorých každý člen implementoval jednu. 
Igor Lacko: Ludo
Róbert Páleš: Pacman
Jozef ONdrejička: Snake
Zložka src so zdorjovými kódmi obsahuje následujúce podadresáre:
    1. app: Hlavná štruktúra aplikácie, obsahuje frontend a nextjs route handlery (backend).
        a) app/api: Nextjs route handlery, osobitne pre rôzne hry.
        b) app/page.ts a app/layout.tsx: Spoločná časť frontendu, hlavné menu.
        c) app/games: Jednotlivé frontendy hier. Z nich jeden implementovaný každým členom.
    2. components: Jednotlivé komponenty pre frontend. Obsahuje spoločnú časť (adresár arcade-machine) a opäť
    jednotlivé časti pre rôzne hry.
    3. lib: rôzne pomocné funkcie, oddelené pre jednotlivé hry
    4. server: spustenie spoločnej inštancie DB (prisma klient)