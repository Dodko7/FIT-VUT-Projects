# CHANGELOG - OMEGA: L4 Scanner

Tento súbor dokumentuje významné zmeny v projekte **OMEGA L4 Scanner (ipk-l4-scan)**.

## [1.0.0] - 2025-03-27

### Pridané
- **Počiatočná verzia**: Implementovaný skener vrstvy 4 podporujúci TCP a UDP skenovanie podľa zadania IPK 2024/2025.
- **TCP SYN skenovanie**:
  - Posiela SYN pakety na určenie stavu portov (`open`, `closed`, `filtered`) bez dokončenia handshake.
  - Opakuje SYN pri absencii odpovede na potvrdenie `filtered` stavu.
  - Podpora IPv4 aj IPv6.
- **UDP skenovanie**:
  - Interpretuje ICMP „port unreachable“ ako `closed`, inak predpokladá `open`.
  - Podpora IPv4 aj IPv6.
- **Príkazový riadok**:
  - Možnosti: `-i/--interface`, `-t/--pt` (TCP porty), `-u/--pu` (UDP porty), `-w/--wait` (timeout), `--help`.
  - Podpora rozsahov portov (`22`, `1-65535`, `22,23,24`).
  - Preklad hostnames na viacero IPv4/IPv6 adries (`getaddrinfo`).
- **Sokety a pakety**:
  - Použitie raw soketov (`SOCK_RAW`) s plnou kontrolou nad hlavičkami TCP/UDP.
  - Výpočet internetových kontrolných súčtov (IPv4, IPv6).
- **Zachytávanie odpovedí**:
  - `libpcap` na filtrovanie odpovedí podľa cieľových IP a portov.
  - Spracovanie SYN-ACK, RST a ICMP správ pre určenie stavu portov.
- **Výstup**:
  - Formát: `<IP> <port> <protocol> <state>` (napr. `127.0.0.1 22 tcp open`).
  - Podpora viac IP adries pri hostname resolvovaní.
- **Signal handling**: Korektné ukončenie cez `Ctrl + C` (SIGINT).
- **Testovanie**: Overené pomocou Wiresharku a Nmap pre TCP/UDP na IPv4/IPv6.

### Známé obmedzenia
- **Spoľahlivosť UDP skenovania**: `open` status pri absencii odpovede môže viesť k nesprávnej klasifikácii.
- **Limit filtra `libpcap`**: Buffer 65536 bajtov môže obmedziť rozsiahle filtre.
- **Sekvenčné skenovanie**: Bez multi-threadingu, čo môže spomaľovať veľké rozsahy.
- **Žiadne pokročilé funkcie**: Žiadna detekcia služieb, náhodné porty či vlastné UDP payloady.
- **Vyžaduje root oprávnenia**: Použitie raw soketov a `libpcap` vyžaduje `sudo`.

### Poznámky
- Implementácia plne spĺňa špecifikáciu IPK Project 2.
- IPv6 podpora zahrnutá ako implicitná funkcionalita.
- TCP retry mechanizmus zvyšuje spoľahlivosť detekcie `filtered` portov.