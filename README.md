# Projektowanie Oprogramowania Systemów

To repozytorium powstaje jako część projektu z przedmiotu Projektowanie Oprogramowania Systemów w toku studiów II stopnia na Politechnice Gdańskiej.

## Cel projektu

Celem projektu jest stworzenie *mockupu* oprogramowania systemu oraz kompletnej dokumentacji projektowej, w szczególności należy:
- określić klienta, użytkowników końcowych i innych interesariuszy projektowanego systemu,
- określić wymagania funkcjonalne i pozafunkcjonalne,
- utworzyć specyfikację funkcjonalną,
- utworzyć repozytorium projektu ze strukturą odpowiednią dla zastosowanych narzędzi i środowisk,
- utworzyć model UML kluczowych elementów projektowanego systemu i zawrzeć go w dokumentacji technicznej,
- utworzyć harmonogram prac,
- utworzyć dokumentację HTML przy pomocy [Doxygen](https://github.com/doxygen/doxygen),
- wykorzystać system do raportowania i śledzenia błędów,
- przygotować i przeprowadzić testy jednostkowe,
- użyć profilera do wykrycia *hot-spotów* w oprogramowaniu i podjąć próbę optymalizacji.

## Temat projektu

Tematem projektu jest **automatyczny system monitorowania pogody**.

### Przyjęte założenia

W celu osadzenia projektu w rzeczywistości przyjęto, że zarządca Rodzinnych Ogródków Działkowych zamawia system monitorowania pogody na działkach. System powinien składać się z centralki (pod kontrolą zarządcy) oraz węzłów pomiarowych (udostępnianych działkowiczom) komunikujących się drogą radiową. Centralka agreguje dane z węzłów pomiarowych i umieszcza je w bazie danych. Zarządca otrzymuje do niej dostęp przez aplikację z interfejsem graficznym. Dodatkowo, centralka serwuje stronę www, na której działkowicze mogą monitorować pogodę na swojej działce w czasie rzeczywistym oraz przeglądać uśrednione dane pomiarowe ze wszystkich urządzeń podłączonych do centralki.

## Projektowane rozwiązanie

Niniejsza sekcja opisuje ważniejsze aspekty projektowanego rozwiązania oraz zawiera odnośniki do bardziej szczegółowych dokumentów.

### Interesariusze

Zidentyfikowano następujące strony zainteresowane projektem:

| Interesariusz | Rola | Główne potrzeby /  Oczekiwania |
| :--- | :--- | :---|
| Zarządca ROD | Klient | Dostęp do szczegółowych danych o pogodzie na terenie całego obiektu, generowanie raportów na podstawie zebranych danych. |
| Działkowicz | Użytkownik końcowy | Podgląd parametrów pogodowych na własnej działce w czasie rzeczywistym oraz dostęp do historii pomiarów przez stronę internetową. |
| Administrator IT | Wsparcie techniczne | Zapewnienie ciągłości działania oraz bezpieczeństwa bazy danych i strony, zarządzanie uprawnieniami i kontami użytkowników. |
| Użytkownik pasma ISM | Osoba postronna | Minimalizacja negatywnego wpływu systemu na inne urządzenia działające w paśmie ISM. |
| Urząd Komunikacji Elektronicznej | Organ regulacyjny | Zgodność urządzeń z obowiązującym prawem. |

### Wymagania

W oparciu o analizę potrzeb interesariuszy oraz założeń projektowych sformułowano wymagania funkcjonalne i pozafunkcjonalne. Kompletny wykaz wymagań znajduje się w specyfikacji funkcjonalnej systemu [`/specs/func`](./specs/func/).

### Model systemu

System został zaprojektowany w architekturze rozproszonej typu klient-serwer, z podziałem na warstwę zbierania danych (Edge), przetwarzania i składowania (Backend) oraz prezentacji (Frontend).

Podstawowe scenariusze użycia to m.in. przeglądanie danych pogodowych przez działkowicza, zarządzanie użytkownikami i uprawnieniami przez administratora, czy gromadenie danych przez węzeł pomiarowy.

Szczegółowy opis struktury systemu oraz przypadków użycia zawarto w specyfikacji funkcjonalnej systemu [`/specs/func`](./specs/func/).

## Plan realizacji projektu

W celu profesjonalnej i terminowej realizacji projektu opracowano skład zespołu projektowego z podziałem na role i wyszczególniono odpowiedzialność każdej z nich. Przygotowano harmonogram prac z podziałem na kamienie milowe składające się z zadań; czas realizacji określono w tygodniach. Szczegółowy plan realizacji projektu znajduje się w [`/meta/project_plan`](/meta/project_plan.md).

## Zawartość repozytorium

- [`/docs`](./docs/) zawiera dokumentację kodu wygenerowaną przy użyciu narzędzia Doxygen.
- [`/meta`](./meta/) zawiera dokumentację wewnętrzną projektu oraz pliki pomocne w jego realizacji.
- [`/specs/func`](./specs/func) zawiera specyfikację funkcjonalną systemu, w tym: wykaz wymagań funkcjonalnych i pozafunkcjonalnych oraz scenariusze użycia.
- [`/specs/tech`](./specs/tech/) zawiera dokumentację techniczną systemu.
- [`/src/database`](./src/database/) zawiera bazę danych pomiarowych opartą o *SQLite*.
- [`/src/firmware`](./src/firmware/) zawiera kod urządzenia pomiarowego opartego o mikrokontroler STM32L073RZT6. Kod rozwijano z zastosowaniem *CubeIDE*.
- [`/src/gateway`](./src/gateway/) zawiera kod urządzenia centranego w języku Python. Pozwala na odbieranie danych pomiarowych i obsługę bazy danych.
- [`/src/gui`](./src/gui/) zawiera kod w jęzuku Python aplikacji okienkowej do obsługi systemu.
- [`/src/web/backend`](./src/web/backend/) zawiera kod w jęzuku Python składający się na backend aplikacji webowej. Zastosowano *Flask*.
- [`/src/web/frontend`](./src/web/frontend/) zawiera kod składający się na frontend aplikacji webowej. Zastosowano *Jinja* oraz *Chart.js*.
- [`tests/firmware`](./tests/firmware/) zawiera testy jednostkowe kodu węzła pomiarowego.
- [`tests/gateway`](./tests/gateway/) zawiera testy jednostkowe kodu urządzenia centralnego.

## Autorzy

- Adrian Czekański, czekanski.adrian@wp.pl, 193216, EiT
- Hubert Mucha, hu_mu@wp.pl, 193152, EiT

