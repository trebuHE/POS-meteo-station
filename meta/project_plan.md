# Plan realizacji projektu

## Cel

Ten dokument określa zespół projektowy oraz harmonogram realizacji projektu w ramach którego powstaje automatyczny system monitorowania pogody. Ten dokument jest częścią dokumentacji wewnętrznej zespołu projektowego, **nie jest** przeznaczony dla klienta i użytkowników.

## Zespół projektowy

W celu profesjonalnej realizacji projektu zidentyfikowano role potrzebne w zespole projektowym.

| Rola | Odpowiedzialność | 
| :--- | :--- |
| Kierownik Projektu | Zarządzanie pracą zespołu projektowego, komunikacja z interesariuszami. |
| Maintainer | Nadzór nad repozytorium, opracowanie struktury dokumentacji, konfiguracja Doxygen oraz generowanie dokumentacji. |
| Embedded Developer | Projekt oraz implementacja oprogramowania wbudowanego węzła pomiarowego. |
| Software Engineer | Projekt oraz implementacja logiki urządzenia centralnego, obsługa bazy danych *SQLite* oraz serwera *Flask*. |
| Frontend Developer | Projekt oraz implementacja aplikacji GUI z wykorzystaniem *CustomTkinter*. |
| Web Developer | Opracowanie strony internetowej zawierającej dashboard użytkownika oraz panel administracji. |
| QA Engineer | Opracowanie scenariuszy testowych, implementacja testów jednostkowych oraz analiza wydajności. |

## Harmonogram prac

W celu terminowej realizacji projektu oraz możliwości śledzenia postępów w realizacji opracowano harmonogram prac z podziałem na fazy (kamienie milowe) składające się z zadań. Czas na realizację poszczególnych kamieni milowych określono z dokładnością do tygodnia.

### Faza 1: Analiza i Planowanie (tydzień 1)

- [x] Rozpoznanie interesariuszy.
- [x] Sformułowanie wymagań funkcjonalnych oraz pozafunkcjonalnych.
- [x] Opracowanie wstępnego modelu systemu.
- [x] Opracowanie kompletnej dokumentacji technicznej.
- [x] Wyznaczenie zespołu projektowego i opracowanie harmonogramu prac.

### Faza 2: Warstwa Sprzętowa i Komunikacja Radiowa (tydzień 2-4)

- [x] Opracowanie wstępnej dokumentacji technicznej systemu.
- [x] Konfiguracja peryferiów mikrokontrolera STM32.
- [x] Opracowanie sterowników sprzętowych modułów pomiarowych.
- [x] Implementacja protokołu radiowego węzła pomiarowego.
- [ ] Implementacja protokołu radiowego urządzenia centralnego.
- [ ] Wstępne testy transmisji radiowej.

### Faza 3: Warstwa Danych i Backend (tydzień 5-6)

- [ ] Implementacja bazy danych *SQLite*.
- [ ] Implementacja modułu zarządzającego bazą danych na urządzeniu centralnym.
- [ ] Implementacja serwera *Flask*.
- [ ] Implementacja funkcjonalności tworzenia i zarządzania kontami użytkowników na stronie internetowej.

### Faza 4: Warstwa Prezentacji i Interfejsy (tydzień 7-9)

- [ ] Implementacja dashboardu na stronie internetowej.
- [ ] Implementacja panelu administracji na stronie internetowej.
- [ ] Opracowanie aplikacji GUI.
- [ ] Umożliwienie generowania raportów okresowych z bazy danych.
- [ ] Umożliwienie definiowania zakresów ostrzegania na stronie internetowej.

### Faza 5: Testy, Poprawki i Integracja (tydzień 9-13)

- [ ] Testy kluczowych modułów systemu w izolacji, poprawki.
- [ ] Testy integracyjne systemu, poprawki.
- [ ] Testy wydajnościowe systemu, poprawki.
- [ ] Testy jakościowe systemu, poprawki.

### Faza 6: Raportowanie i Finalizacja (tydzień 14-15)

- [ ] Przygotowanie raportu testów i wprowadzonych poprawek.
- [ ] Opracowanie kompletnej dokumentacji systemu.
- [ ] Przygotowanie sprawozdania końcowego.
- [ ] Przedstawienie sprawozdania końcowego klientowi.

### Faza 7: Wdrożenie (tydzień 16)

- [ ] Przekazanie kompletnego rozwiązania klientowi.
- [ ] Asysta wdrożeniowa.