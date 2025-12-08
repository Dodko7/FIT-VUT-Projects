import { PrismaClient } from '@prisma/client'
const prisma = new PrismaClient()

async function main() {
    await prisma.pacmanLevel.upsert({
        where: { id: 1 },
        update: {},
        create: {
            name: "Easy",
            map: [
                "1111111111111111111",
                "1       111       1",
                "1                 1",
                "1   b    r    p   1",
                "1                 1",
                "11   1111 1111   11",
                "1                 1",
                "1    1   1   1    1",
                "1    1   1   1    1",
                "1                 1",
                "1    1   1   1    1",
                "1    1   1   1    1",
                "1                 1",
                "1    1   1   1    1",
                "1    1   1   1    1",
                "1        P        1",
                "1                 1",
                "11   1111 1111   11",
                "1                 1",
                "1       111       1",
                "1111111111111111111"
            ]
        }
    })
    await prisma.pacmanLevel.upsert({
        where: { id: 2 },
        update: {},
        create: {
            name: "Medium",
            map: [
                "1111111111111111111",
                "1     1111111     1",
                "1       111       1",
                "1 1 111 111 111 1 1",
                "1        bp       1",
                "1 111  111111 111 1",
                "1        ro       1",
                "11 111  111  111 11",
                "1   1         1   1",
                "1       111       1",
                "11 1111  1  1111 11",
                "1                 1",
                "1 111  111111 111 1",
                "1                 1",
                "11  111 111 111  11",
                "1        P        1",
                "1   11111111111   1",
                "1                 1",
                "1 1 111  1  111 1 1",
                "1                 1",
                "1111111111111111111"
            ]
        }
    })
    await prisma.pacmanLevel.upsert({
        where: { id: 3 },
        update: {},
        create: {
            name: "Hard",
            map: [
                "1111111111111111111",
                "1        1        1",
                "1 11 111 1 111 11 1",
                "1                 1",
                "1 11 1 11111 1 11 1",
                "1    1       1    1",
                "1111 1111 1111 1111",
                "OOO1 1       1 1OOO",
                "1111 1 11r11 1 1111",
                "1       bpo       1",
                "1111 1 11111 1 1111",
                "OOO1 1       1 1OOO",
                "1111 1 11111 1 1111",
                "1        1        1",
                "1 11 111 1 111 11 1",
                "1  1     P     1  1",
                "11 1 1 11111 1 1 11",
                "1    1   1   1    1",
                "1 111111 1 111111 1",
                "1                 1",
                "1111111111111111111"
            ]
        }
    })
}

main()
  .then(async () => { await prisma.$disconnect() })
  .catch(async (e) => { console.error(e); await prisma.$disconnect(); process.exit(1) })