SERVEROUTPUT ON;
CREATE TABLE Clen (
    id_clena INT NOT NULL PRIMARY KEY,
    cislo_preukaz INT,
    expir DATE,
    cislo_zam_karta INT,
    uroven_pristup VARCHAR(10),
    CONSTRAINT check_pristup CHECK (uroven_pristup IN ('zam','reg','admin')),
    --kontrolujeme spravny vstup vo formate zam = zamestnanec, reg = registrovany citatel, admin
    meno VARCHAR(20),
    adresa VARCHAR(20),
    telefon INT,
    mail VARCHAR(40),
    narodenie DATE
);

-- v tabulke clena sme zakomponovali generalizaciu, tato tabulka funguje
-- ako pre citatelov tak aj pre pracovnikov
-- citatel ma nahradenu zamestnanecku kartu hodnotou NULL a maju najnizsiu uroven pristupu
-- umoznujeme tak knihovnikom vztvorit si citatelsky preukaz

CREATE TABLE Vydavatelstvo(
    nazov   VARCHAR(30) PRIMARY KEY ,
    adresa  VARCHAR2(30),
    kontakt VARCHAR2(50),
    stranka    VARCHAR2(50)
);

CREATE TABLE Autor (
    id_autora INT PRIMARY KEY ,
    meno VARCHAR(30),
    zaner VARCHAR(20),
    ocenenia VARCHAR(100),
    narodnost VARCHAR(30),
    narodenie DATE,
    smrt DATE
);

CREATE TABLE Titul(
  nazov VARCHAR(20) PRIMARY KEY ,
  ISBN VARCHAR2(20),
  ISSN VARCHAR2(15),
  dostupnost VARCHAR(1), --znacime -> r - rezervovana, d - dostupna, v-vypozicana
  zaner VARCHAR(15),
  autor INT ,
  FOREIGN KEY (autor) REFERENCES Autor(id_autora),
  vydanie VARCHAR(30),
  FOREIGN KEY (vydanie) REFERENCES Vydavatelstvo(nazov) ON DELETE SET NULL,
  CONSTRAINT check_dostupnost CHECK (dostupnost IN ('r','v','d')),
  --CONSTRAINT check_ISBN CHECK (is_valid_isbn(ISBN)),
  CONSTRAINT check_ISSN CHECK (issn is NULL OR
    REGEXP_LIKE(
        REPLACE(issn, '-',''), -- Removing hyphens
        '^[0-9]{8}$' -- Checking for 8 digits
    )
    AND MOD(
        (
            8 * TO_NUMBER(SUBSTR(REPLACE(issn, '-',''), 1, 1)) +
            7 * TO_NUMBER(SUBSTR(REPLACE(issn, '-',''), 2, 1)) +
            6 * TO_NUMBER(SUBSTR(REPLACE(issn, '-',''), 3, 1)) +
            5 * TO_NUMBER(SUBSTR(REPLACE(issn, '-',''), 4, 1)) +
            4 * TO_NUMBER(SUBSTR(REPLACE(issn, '-',''), 5, 1)) +
            3 * TO_NUMBER(SUBSTR(REPLACE(issn, '-',''), 6, 1)) +
            2 * TO_NUMBER(SUBSTR(REPLACE(issn, '-',''), 7, 1)) +
            1 * TO_NUMBER(SUBSTR(REPLACE(issn, '-',''), 8, 1))
        ),
        11
    ) = 0),
    CONSTRAINT check_ISBN
CHECK (
    (
        REGEXP_LIKE(
            REPLACE(isbn, '-',''),
            '^[0-9]{10}$'
        )
        AND MOD(
            (
                10 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 1, 1)) +
                9 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 2, 1)) +
                8 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 3, 1)) +
                7 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 4, 1)) +
                6 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 5, 1)) +
                5 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 6, 1)) +
                4 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 7, 1)) +
                3 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 8, 1)) +
                2 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 9, 1)) +
                1 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-', ''),10,1))
            ),
            11
        ) = 0
    )
    OR
    (
        REGEXP_LIKE(
            REPLACE(isbn, '-',''), -- Removing hyphens
            '^[0-9]{13}$' -- Checking for ISBN-13 format
        )
        AND MOD(
            (
                1 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 1, 1)) +
                3 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 2, 1)) +
                1 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 3, 1)) +
                3 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 4, 1)) +
                1 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 5, 1)) +
                3 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 6, 1)) +
                1 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 7, 1)) +
                3 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 8, 1)) +
                1 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 9, 1)) +
                3 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 10, 1)) +
                1 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 11, 1)) +
                3 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''), 12, 1)) +
                1 * TO_NUMBER(SUBSTR(REPLACE(isbn, '-',''),13, 1))
            ),
            10
        ) = 0
    )
)
);
--pre kontrolu validity ISSN a ISBN som zvolil takuto variantu pretoze pre prikaz CHECK() nie je mozne zavolat do funkcie ako parameter

CREATE TABLE vypozicanie (
    id_pozicania INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
    id_clena INT NOT NULL,
    FOREIGN KEY (id_clena) REFERENCES Clen(id_clena) ON DELETE CASCADE , --udaje o vypozicani sluzia na prevenciu nevratenych kniziek, preto ak neexistuje zodpovedna osoba zaznam straca zmysel
    nazov VARCHAR(20),
    FOREIGN KEY (nazov) REFERENCES Titul(nazov) ON DELETE CASCADE , --ak vyradime knizku ktoru si dany clen vypozical, nie je nutne udrziavat udaje o vypozicani
    vratenie DATE,
    spozdenie INT
);
CREATE TABLE rezervacia (
    id_rezervacie INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
    id_clena INT,
    FOREIGN KEY (id_clena) REFERENCES Clen(id_clena) ON DELETE CASCADE ,--ak neexistuje tvorca rezervacie, udaje o nej nemusime drzat
    nazov VARCHAR(20),
    FOREIGN KEY (nazov) REFERENCES Titul(nazov) ON DELETE CASCADE , -- nemozme vytvorit rezervaciu pre vyradenu knizku
    expiracia_rezervacie DATE
);
--zdielanie cudzich klucov clena a titulu zabezpeci aby sme nemohli vlozit rovnaky zaznam
-- aj medzi vypozicania aj medzi rezervacie

--automaticky k titulu zmeni dostupnost ak sa nazov titulu objavi v tabulke pre vypozicania alebo rezervacie

CREATE TABLE Historia (
    id_vytlacku VARCHAR2(20),
    nazov VARCHAR(30),
    id_clena INT,
    stav_vratenia INT CHECK (stav_vratenia <= 10),
    FOREIGN KEY (id_clena) REFERENCES Clen (id_clena) ON DELETE CASCADE, --historia pozicani pre neexistujuceho clena nemusi byt drzana
    PRIMARY KEY (id_vytlacku, id_clena)
);


--grating permissions for colleague
    GRANT ALL ON Clen TO XONDRE16;
    GRANT ALL ON Titul TO XONDRE16;
    GRANT ALL ON Autor TO XONDRE16;

--inserts
INSERT INTO Clen VALUES (1,123456, TO_DATE( '2025-12-31', 'YYYY-MM-DD'), NULL, 'reg', 'Jan Novak', 'Praha 1', 123456789, 'jan.novak@email.com', TO_DATE( '1990-01-01', 'YYYY-MM-DD'));
INSERT INTO Clen VALUES (2,654321, TO_DATE( '2024-12-31', 'YYYY-MM-DD'), 987, 'admin', 'Eva Kralova', 'Brno 3', 987654321, 'eva.kralova@email.com',TO_DATE( '1985-05-05', 'YYYY-MM-DD') );
INSERT INTO Clen VALUES (3,789012, TO_DATE( '2026-05-20', 'YYYY-MM-DD'), NULL, 'reg', 'Lucia Biela', 'Kosice 2', 321654987, 'lucia.biela@email.com',TO_DATE( '1992-04-08', 'YYYY-MM-DD'));
INSERT INTO Clen VALUES (4,210987, TO_DATE( '2027-11-15', 'YYYY-MM-DD'), 654, 'zam', 'Michal Horak', 'Ostrava 4', 789123456, 'michal.horak@email.com',TO_DATE(  '1980-09-12', 'YYYY-MM-DD'));
INSERT INTO Clen VALUES (5, 135790, TO_DATE('2026-08-15', 'YYYY-MM-DD'), NULL, 'reg', 'Peter Hruska', 'Kosice 5', 987654321, 'peter.hruska@email.com', TO_DATE('1995-03-20', 'YYYY-MM-DD'));
INSERT INTO Clen VALUES (6, 246801, TO_DATE('2025-10-10', 'YYYY-MM-DD'), NULL, 'reg', 'Anna Novotna', 'Prague 2', 654987321, 'anna.novotna@email.com', TO_DATE('1993-08-05', 'YYYY-MM-DD'));
INSERT INTO Clen VALUES (7, 123456, TO_DATE('2025-12-31', 'YYYY-MM-DD'), NULL, 'reg', 'Jan Novak', 'Praha 1', 123456789, 'jan.novak@email.com', TO_DATE('1990-01-01', 'YYYY-MM-DD'));
INSERT INTO Clen VALUES (8, 654321, TO_DATE('2024-12-31', 'YYYY-MM-DD'), 987, 'admin', 'Eva Kralova', 'Brno 3', 987654321, 'eva.kralova@email.com', TO_DATE('1985-05-05', 'YYYY-MM-DD'));
INSERT INTO Clen VALUES (9, 789012, TO_DATE('2026-05-20', 'YYYY-MM-DD'), NULL, 'reg', 'Lucia Biela', 'Kosice 2', 321654987, 'lucia.biela@email.com', TO_DATE('1992-04-08', 'YYYY-MM-DD'));

INSERT INTO Vydavatelstvo VALUES('XYZ Publishing', 'New York', '123-456-789', 'http://xyzpublishing.com');
INSERT INTO Vydavatelstvo VALUES ('ABC Books', 'London', '456-789-123', 'http://abcbooks.co.uk');
INSERT INTO Vydavatelstvo VALUES ('Penguin Books', 'London', '333-333-333', 'http://penguinbooks.co.uk');
INSERT INTO Vydavatelstvo VALUES ('HarperCollins', 'New York', '444-444-444', 'http://harpercollins.com');

INSERT INTO Autor VALUES (1, 'Karel Capek', 'Sci-Fi', 'Best Sci-Fi Author', 'Czech', TO_DATE('1890-01-09', 'YYYY-MM-DD'), TO_DATE('1938-12-25', 'YYYY-MM-DD'));
INSERT INTO Autor VALUES (2, 'J.K. Rowling', 'Fantasy', 'Best Fantasy Author', 'British', TO_DATE('1965-07-31', 'YYYY-MM-DD'), NULL);
INSERT INTO Autor VALUES (3, 'Isaac Asimov', 'Sci-Fi', 'Grand Master Award', 'American', TO_DATE('1920-01-02', 'YYYY-MM-DD'),TO_DATE( '1992-04-06', 'YYYY-MM-DD'));
INSERT INTO Autor VALUES (4, 'Agatha Christie', 'Mystery', 'Queen of Mystery', 'British', TO_DATE('1890-09-15', 'YYYY-MM-DD'), TO_DATE('1976-01-12', 'YYYY-MM-DD'));
INSERT INTO Autor VALUES (5, 'Leo Tolstoy', 'Fiction', 'Great Russian Novelist', 'Russian', TO_DATE('1828-09-09', 'YYYY-MM-DD'), TO_DATE('1910-11-20', 'YYYY-MM-DD'));

INSERT INTO Titul VALUES ('Harry Potter', '978-0-596-52068-7', NULL, 'd', 'Fantasy', 2, 'ABC Books');
INSERT INTO Titul VALUES  ('Foundation', NULL, '1234-5679', 'd', 'Sci-Fi', 3, 'XYZ Publishing');
INSERT INTO Titul VALUES ('RUR', '0-596-52068-9', NULL, 'd', 'Sci-Fi', 1, 'XYZ Publishing');
INSERT INTO Titul VALUES ('Murder on the Orient', '978-0062693662', NULL, 'd', 'Mystery', 4, 'HarperCollins');
INSERT INTO Titul VALUES ('War and Peace', '978-0140447934', NULL, 'd', 'Fiction', 5, 'Penguin Books');
INSERT INTO Titul VALUES ('Were None', '978-0062073488', NULL, 'd', 'Mystery', 4, 'HarperCollins');

INSERT INTO vypozicanie (id_clena, nazov, vratenie, spozdenie) VALUES (1, 'RUR', TO_DATE('2022-12-31', 'YYYY-MM-DD'), 1);
INSERT INTO vypozicanie (id_clena, nazov, vratenie, spozdenie) VALUES (2, 'Foundation', TO_DATE('2023-01-15', 'YYYY-MM-DD'), 5);
INSERT INTO vypozicanie (id_clena, nazov, vratenie, spozdenie) VALUES (1, 'Harry Potter', TO_DATE('2024-06-01', 'YYYY-MM-DD'), 0);
INSERT INTO vypozicanie(id_clena, nazov, vratenie, spozdenie) VALUES (4, 'Murder on the Orient', TO_DATE('2024-07-10', 'YYYY-MM-DD'), 2);
INSERT INTO vypozicanie(id_clena, nazov, vratenie, spozdenie) VALUES (5, 'Were None', TO_DATE('2024-08-20', 'YYYY-MM-DD'), 3);

UPDATE Titul
SET dostupnost = 'v'
WHERE Titul.nazov IN (SELECT nazov FROM vypozicanie);

UPDATE Titul
SET dostupnost = 'd'
WHERE Titul.nazov = 'War and Peace';

INSERT INTO rezervacia (id_clena, nazov,expiracia_rezervacie) VALUES (4, 'Harry Potter',TO_DATE('2024-12-15', 'YYYY-MM-DD') );
UPDATE Titul
SET dostupnost = 'r'
WHERE Titul.nazov IN (SELECT nazov FROM rezervacia);

INSERT INTO rezervacia(id_clena, nazov, expiracia_rezervacie) VALUES (1, 'Foundation', TO_DATE('2024-12-15', 'YYYY-MM-DD'));
INSERT INTO rezervacia(id_clena, nazov, expiracia_rezervacie) VALUES (2, 'Harry Potter', TO_DATE('2024-12-20', 'YYYY-MM-DD'));
INSERT INTO rezervacia(id_clena, nazov, expiracia_rezervacie) VALUES (3, 'War and Peace', TO_DATE('2024-11-30', 'YYYY-MM-DD'));
INSERT INTO rezervacia(id_clena, nazov, expiracia_rezervacie) VALUES (6, 'Murder on the Orient', TO_DATE('2024-12-05', 'YYYY-MM-DD'));

--TRIGGER to check so you cannot reserve book that is not viable
CREATE OR REPLACE TRIGGER check_dostupnost_trigger
BEFORE INSERT ON rezervacia
FOR EACH ROW
DECLARE
    v_dostupnost Titul.dostupnost%TYPE;
BEGIN
    SELECT dostupnost INTO v_dostupnost
    FROM Titul
    WHERE nazov = :NEW.nazov;

    IF v_dostupnost = 'v' THEN
        RAISE_APPLICATION_ERROR(-20001, 'It is not possible to place reservation on this book. This book is currently unavailable.');
    END IF;
EXCEPTION
    WHEN NO_DATA_FOUND THEN
        RAISE_APPLICATION_ERROR(-20002, 'Book you are trying to place reservation on is not available within our library.');
END;

--TRIGGER to delete history upon deleting clen that owns that history
CREATE TRIGGER delete_history_on_clen_delete
AFTER DELETE ON Clen
FOR EACH ROW
BEGIN
    DELETE FROM Historia WHERE id_clena = OLD.id_clena;
END;

--TRIGGER upon returning a book it will update book status back to available and insert book to history
CREATE OR REPLACE TRIGGER trigger_book_return
AFTER DELETE ON vypozicanie
FOR EACH ROW
DECLARE
    isbn_or_issn VARCHAR2(20); -- Use VARCHAR2 for string variables in Oracle
    stav INT;
BEGIN
    -- Use COALESCE with proper error handling
    SELECT COALESCE(ISBN, ISSN, 'Unknown') INTO isbn_or_issn
    FROM Titul
    WHERE nazov = :OLD.nazov;
    
    
    -- Spustenie procedúry pre výpočet pokút.
    BEGIN
      sp_vypocet_pokuty;
    END;


    -- Set stav based on the condition
    IF :OLD.vratenie > TRUNC(SYSDATE) THEN
        stav := 1;
    ELSE
        stav := 0;
    END IF;

    -- Insert into Historia table
    INSERT INTO Historia (id_vytlacku, nazov, id_clena, stav_vratenia)
    VALUES (isbn_or_issn, :OLD.nazov, :OLD.id_clena, stav);

    -- Update Titul table to mark the book as available (dostupnost = 'd')
    UPDATE Titul
    SET dostupnost = 'd'
    WHERE nazov = :OLD.nazov; -- Use proper referencing to avoid ambiguity
END;

--function that will calculate your average score of returned books using your history
CREATE OR REPLACE FUNCTION calculate_average_score(
    p_id_clena IN INT
) RETURN NUMBER
IS
    v_average_score NUMBER;
BEGIN
    SELECT AVG(stav_vratenia)
    INTO v_average_score
    FROM Historia
    WHERE id_clena = p_id_clena
    GROUP BY id_clena;

    RETURN v_average_score;
END;


-- Táto procedúra vypočíta a vypíše pokuty pre všetkých členov, ktorí vrátili knihy neskôr ako je dnešný dátum.
CREATE OR REPLACE PROCEDURE sp_vypocet_pokuty AS
  -- Deklarácia premenných pre výpočet pokuty a maximálnu možnú výšku pokuty.
  v_pokuta DECIMAL(10, 2);
  v_max_pokuta CONSTANT DECIMAL(10, 2) := 100;
BEGIN
  -- Prechádzanie všetkých výpožičiek, ktoré majú dátum vrátenia menší ako dnešný dátum.
  FOR vyp IN (SELECT * FROM vypozicanie WHERE vratenie < SYSDATE) LOOP
    -- Výpočet počtu dní zpoždenia a následnej pokuty.
    v_pokuta := (SYSDATE - vyp.vratenie) * 10;
    -- Kontrola, či výška pokuty neprekračuje maximálnu hodnotu.
    IF v_pokuta > v_max_pokuta THEN
      v_pokuta := v_max_pokuta;
    END IF;
    -- Výpis informácie o pokute pre daného člena.
    DBMS_OUTPUT.PUT_LINE('Člen s ID ' || vyp.id_clena || ' má pokutu ve výši ' || v_pokuta || ' Kč za zpoždění vrácení knihy.');
  END LOOP;
EXCEPTION
  -- Ošetrenie výnimiek a výpis prípadných chýb.
  WHEN OTHERS THEN
    DBMS_OUTPUT.PUT_LINE('Došlo k chybě při výpočtu pokuty.');
END sp_vypocet_pokuty;


-- Táto procedúra vypočíta štatistiku o počte výpožičiek pre každý titul a vypíše ju.
CREATE OR REPLACE PROCEDURE sp_vypozicanie_statistika AS
  -- Deklarácia premenných pre počet výpožičiek.
  v_pocet_vypozicaniach INT := 0;
BEGIN
  -- Prechádzanie všetkých titulov a počítanie počtu výpožičiek pre každý z nich.
  FOR titul_rec IN (SELECT nazov, COUNT(*) AS pocet_vypozicaniach FROM vypozicanie GROUP BY nazov) LOOP
    -- Priradenie počtu výpožičiek do lokálnej premennej.
    v_pocet_vypozicaniach := titul_rec.pocet_vypozicaniach;

    -- Ošetrenie výnimiek a výpis prípadných chýb.
    BEGIN
      -- Kontrola platnosti počtu výpožičiek (nesmie byť záporný).
      IF v_pocet_vypozicaniach < 0 THEN
        RAISE_APPLICATION_ERROR(-20001, 'Počet výpožičiek nesmie byť záporný.');
      END IF;

      -- Výpis počtu výpožičiek pre daný titul.
      DBMS_OUTPUT.PUT_LINE('Titul ' || titul_rec.nazov || ' bol vypžičaný ' || v_pocet_vypozicaniach || ' krát.');
    EXCEPTION
      WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('Došlo k chybě při výpočtu statistiky o výpožičkách.');
    END;
  END LOOP;
END sp_vypozicanie_statistika;
/

-- Query 1: Retrieve the names of members who have reserved a book.
SELECT c.meno
FROM Clen c
JOIN rezervacia r ON c.id_clena = r.id_clena;

-- Query 2: Retrieve the titles of books authored by J.K. Rowling.
SELECT t.nazov
FROM Titul t
JOIN Autor a ON t.autor = a.id_autora
WHERE a.meno = 'J.K. Rowling';

-- Query 3: Retrieve the titles and genres of books available for borrowing.
SELECT t.nazov, t.zaner
FROM Titul t
WHERE t.dostupnost = 'd';

-- Query 4: Count the number of books each member has borrowed.
SELECT v.id_clena, COUNT(*) AS num_borrowed
FROM vypozicanie v
GROUP BY v.id_clena;

-- Query 5: Find members who have borrowed books and have overdue returns.
SELECT DISTINCT c.meno
FROM Clen c
JOIN vypozicanie v ON c.id_clena = v.id_clena
WHERE v.vratenie < CURRENT_DATE;

-- Query 6: Find members who have made reservations but haven't borrowed any books yet.
SELECT DISTINCT c.meno
FROM Clen c
JOIN rezervacia r ON c.id_clena = r.id_clena
WHERE NOT EXISTS (
    SELECT 1
    FROM vypozicanie v
    WHERE v.id_clena = c.id_clena
);

-- Query 7: Retrieve titles of books published by 'XYZ Publishing' which are reserved or borrowed.
SELECT t.nazov
FROM Titul t
WHERE t.vydanie = 'XYZ Publishing'
AND t.nazov IN (SELECT nazov FROM rezervacia)
OR t.nazov IN (SELECT nazov FROM vypozicanie);

--Creating indexes to fasten query using Author
CREATE INDEX idx_historia_id_clena ON Historia (id_clena);
CREATE INDEX idx_autor_meno ON Autor (meno);

--Will select all books that i have read from particular author
EXPLAIN PLAN FOR
    SELECT DISTINCT t.nazov AS book_title
    FROM Clen c
    JOIN Historia h ON c.id_clena = h.id_clena
    JOIN Titul t ON h.nazov = t.nazov
    JOIN Autor a ON t.autor = a.id_autora
    WHERE c.meno = 'Jan Novak'
    AND a.meno = 'J.K. Rowling';

--Show table  created by EXPLAIN PLAN --using indexes on history and author_name
SELECT * FROM TABLE(DBMS_XPLAN.DISPLAY);
DROP INDEX idx_historia_id_clena;

--Will display all books i have read from particular publisher
EXPLAIN PLAN FOR
    SELECT DISTINCT t.nazov AS book_title
    FROM Clen c
    JOIN Historia h ON c.id_clena = h.id_clena
    JOIN Titul t ON h.nazov = t.nazov
    JOIN Vydavatelstvo v ON t.vydanie = v.nazov
    WHERE c.meno = 'Jan Novak'
    AND v.nazov = 'ABC Books';

--show table  created by EXPLAIN PLAN --not using indexes
SELECT * FROM TABLE(DBMS_XPLAN.DISPLAY);

--will select books with same genre or author (if we liked book we can search for recommendation like this)
WITH ProvidedBook AS (
    SELECT
        t.nazov AS provided_book,
        a.zaner AS provided_genre,
        a.meno AS provided_author
    FROM
        Titul t
    JOIN
        Autor a ON t.autor = a.id_autora
    WHERE
        t.nazov = 'Foundation'
)
SELECT
    t.nazov AS book_title,
    a.meno AS author,
    a.zaner AS genre
FROM
    Titul t
JOIN
    Autor a ON t.autor = a.id_autora
JOIN
    ProvidedBook pb ON t.nazov <> pb.provided_book -- Exclude the provided book
WHERE
    t.nazov <> pb.provided_book
    AND t.dostupnost = 'd' -- Filter books with dostupnost 'd'
    AND (
        CASE
            WHEN pb.provided_genre IS NOT NULL THEN
                CASE
                    WHEN a.zaner = pb.provided_genre THEN 1
                    ELSE 0
                END
            ELSE
                CASE
                    WHEN a.meno = pb.provided_author THEN 1
                    ELSE 0
                END
        END
    ) = 1;

--testing trigger
select * from vypozicanie;

delete from vypozicanie
where nazov = 'Harry Potter';

delete from vypozicanie
where nazov = 'RUR';

select * from Historia;


--calling function to calculate score of Jan Novak with id_clena = 1
SELECT calculate_average_score(1) AS average_score FROM dual;


-- Spustenie procedúry pre výpočet štatistiky o výpožičkách.
BEGIN
  sp_vypozicanie_statistika;
END;

-- Spustenie procedúry pre výpočet pokuty
BEGIN
  sp_vypocet_pokuty;
END;



CREATE MATERIALIZED VIEW vypozicanie_summary
BUILD IMMEDIATE
REFRESH COMPLETE
START WITH SYSDATE NEXT SYSDATE + 1
AS
SELECT id_clena, COUNT(*) AS pocet_vypoziciek
FROM vypozicanie
GROUP BY id_clena;

-- Získání souhrnu výpůjček pro určitého člena
SELECT *
FROM vypozicanie_summary
WHERE id_clena = 1;

BEGIN
    DBMS_MVIEW.REFRESH('vypozicanie_summary');
END;

-- Další dotaz na aktualizovaná data z materializovaného pohledu
SELECT *
FROM vypozicanie_summary;
