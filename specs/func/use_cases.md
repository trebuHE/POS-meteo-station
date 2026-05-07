# Scenariusze użycia
Zidentyfikowano następujące główne przypadki użycia, pokrywające wymagania interesariuszy:
## UC-01: Przeglądanie danych pogodowych
* **Główny Aktor:** Działkowicz, Zarządca ROD
* **Cel:** Użytkownik chce sprawdzić aktualne warunki pogodowe na danej działce lub przejrzeć historię pomiarów
* **Wymagania powiązane:** INT-01, INT-02
* **Scenariusz główny**
    1. Użytkownik loguje się do systemu (przez stronę www lub aplikację GUI)
    2. System weryfikuje uprawnienia
    3. Użytkownik wybiera z listy konkrenty węzeł pomiarowy
    4. System pobiera z bazy danych najnowsze pomiary oraz dane historyczne
    5. System prezentuje dane w formie czytelnych wskaźników i wykresów

## UC-02: Konfiguracja ostrzeżeń pogodowych
* **Główny Aktor:** Działkowicz
* **Cel:** Zdefiniowanie warunków granicznych, po których przekroczeniu system wyśle e-mail z powiadomieniem
* **Wymagania powiązane:** INT-05
* **Scenariusz główny**
    1. Działkowicz przechodzi do zakładki "Ostrzeżenia" w panelu webowym
    2. Wybiera parametr pogodowy (np. temperatura)
    3. Określa warunek (np. spadek poniżej 0°C)
    4. Podaje lub zatwierdza adres e-mail
    5. System zapisuje nową regułę powiadamiania w bazie danych

## UC-03: Generowanie raportów okresowych
* **Główny Aktor:** Zarządca ROD
* **Cel:** Uzyskanie zagregowanych danych pogodowych dla całego obiektu w określonym przedziale czasu
* **Wymagania powiązane:** INT-04
* **Scenariusz główny**
    1. Zarządca uruchamia aplikację z interfejsem graficznym
    2. Przechodzi do modułu raportowania
    3. Definiuje zakres dat oraz wybiera parametry lub konkretne węzły do uwzględnienia
    4. System przetwarza zapytanie i agreguje dane
    5. System wyświetla raport z możliwością eksportu np. do pliku PDF

## UC-04: Zarządzanie użytkownikami i uprawnieniami
* **Główny Aktor:** Administrator IT
* **Cel:** Tworzenie i modyfikacja kont dostępowych dla działkowiczów i zarządców
* **Wymagania powiązane:** INT-03
* **Scenariusz główny**
    1. Administrator loguje się do panelu administracyjnego urządzenia centralnego
    2. Przechodzi do selekcji zarządzania kontami
    3. Dodaje nowego użytkownika, przypisując mu odpowiednią rolę oraz węzeł pomiarowy
    4. System zapisuje zmiany w bazie danych

## UC-05: Gromadzenie danych pogodowych
* **Główny Aktor:** Węzeł pomiarowy
* **Cel:** Cykliczne dostarczanie surowych danych do jednostki centralnej
* **Wymagania powiązane:** WP-01 do WP-06
* **Scenariusz główny**
    1. Węzeł pomiarowy dokonuje odczytu z czujników 
    2. Formatuje ramkę danych i wysyła ję drogą radiową
    3. Urządzenie centralne odbiera ramkę, formatuje ją pod strukturę bazy danych i zapisuje nowy rekord

## Diagram Przypadków Użycia
```mermaid
flowchart LR
    %% Deklaracja aktorów
    Dzialkowicz(["Działkowicz"])
    Zarzadca(["Zarządca ROD"])
    Admin(["Administrator IT"])
    Wezel(["Węzeł Pomiarowy\n<< System >>"])

    %% Granice systemu
    subgraph System ["System Monitorowania Pogody"]
        UC1([UC-01: Przeglądanie danych pogodowych])
        UC2([UC-02: Konfiguracja ostrzeżeń pogodowych])
        UC3([UC-03: Generowanie raportów okresowych])
        UC4([UC-04: Zarządzanie użytkownikami])
        UC5([UC-05: Automatyczne zbieranie pomiarów])
    end

    %% Relacje - Działkowicz
    Dzialkowicz --- UC1
    Dzialkowicz --- UC2

    %% Relacje - Zarządca ROD
    Zarzadca --- UC1
    Zarzadca --- UC3

    %% Relacje - Administrator IT
    Admin --- UC4

    %% Relacje - Aktorzy Systemowi
    Wezel --- UC5
    