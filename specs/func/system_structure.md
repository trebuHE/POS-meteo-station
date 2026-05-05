# Struktura systemu

System został zaprojektowany w architekturze rozproszonej typu klient-serwer, z podziałem na warstwę zbierania danych (Edge), przetwarzania i składowania (Backend) oraz prezentacji (Frontend).

## Diagram komponentów

Poniższy diagram przedstawia kluczowe elementy systemu oraz relacje między nimi:

```mermaid
flowchart BT
 subgraph subGraph0["Węzeł Pomiarowy"]
        MCU["Mikrokontroler STM32L0"]
        Sensors["Zestaw Czujników"]
        RadioTX["Moduł Radiowy"]
  end
 subgraph subGraph1["Urządzenie Centralne"]
        Gateway["Aplikacja Odbiorcza"]
        RadioRX["Moduł Radiowy"]
        DB[("Baza Danych SQL")]
        WebServer["Serwer WWW / API"]
        AdminApp["Aplikacja GUI Zarządcy"]
  end
    Sensors --> MCU
    MCU --> RadioTX
    RadioTX -. kanał radiowy .-> RadioRX
    RadioRX --> Gateway
    Gateway --> DB
    DB --> WebServer & AdminApp
    WebServer --> Browser["Przeglądarka - Działkowicz"]
    AdminApp --> Manager["Zarządca ROD"]
    