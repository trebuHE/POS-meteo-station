# Struktura systemu

System został zaprojektowany w architekturze rozproszonej typu klient-serwer, z podziałem na warstwę zbierania danych (Edge), przetwarzania i składowania (Backend) oraz prezentacji (Frontend).

## Diagram komponentów

Poniższy diagram przedstawia kluczowe elementy systemu oraz relacje między nimi:

```mermaid
graph TD
    subgraph "Węzeł Pomiarowy (STM32)"
        Sensors[Zestaw Czujników] --> MCU[Mikrokontroler STM32L0]
        MCU --> RadioTX[Moduł Radiowy 433MHz]
    end

    RadioTX -.->|Transmisja radiowa| RadioRX[Odbiornik radiowy]

    subgraph "Urządzenie Centralne (Raspberry Pi)"
        RadioRX --> Gateway[Aplikacja Odbiorcza]
        Gateway --> DB[(Baza Danych SQL)]
        DB --> WebServer[Serwer WWW / API]
        DB --> AdminApp[Aplikacja GUI Zarządcy]
    end

    WebServer --> Browser[Przeglądarka - Działkowicz]
    AdminApp --> Manager[Zarządca ROD]
    ```