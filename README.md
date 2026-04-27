# Projektowanie Oprogramowania Systemów

To repozytorium powstaje jako część projektu z przedmiotu Projektowanie Oprogramowania Systemów.

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

W oparciu o analizę potrzeb interesariuszy oraz założeń projektowych sformułowano wymagania funkcjonalne i pozafunkcjonalne. Kompletny wykaz wymagań znajduje się w [`/specs/reqs`](./specs/reqs/).

## Zawartość repozytorium

- [`/docs`](./docs/) zawiera dokumentację kodu wygenerowaną przy użyciu narzędzia Doxygen.
- [`/specs/reqs`](./specs/reqs) zawiera wykaz wymagań funkcjonalnych oraz pozafunkcjonalnych i specyfikację funkcjonalną systemu.
- [`/specs/tech`](./specs/tech/) zawiera dokumentację techniczną systemu.
- [`/src/firmware`](./src/firmware/) zawiera kod urządzenia pomiarowego opartego o mikrokontroler STM32L073RZT6. Kod rozwijano z zastosowaniem CubeIDE.