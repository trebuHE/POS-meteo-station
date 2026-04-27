# Wymagania funkcjonalne

Wymagania funkcjonalne systemu sformułowano w oparciu o analizę potrzeb i oczekiwań interesariuszy oraz założeń projektowych. 

Wymagania podzielono, przydzielając im ID, w zależności od tego do jakiej części systemu się odnoszą:
- **WP-x** - Węzeł pomiarowy
- **UC-x** - Urządzenie centralne
- **INT-x** - Interfejsy

Wyróżniono trzy poziomy priorytetu:
1. **ma** - wymaganie o krytycznym znaczeniu,
2. **powinien** - wymaganie, którego spełnienie nie jest niezbędne dla poprawnego działania systemu,
3. **może** - wymaganie o niskim priorytecie, którego spełnienie można odłożyć w czasie lub całkowicie zaniechać.

Wymagania opatrzono identyfikatorami metod weryfikacji:
- **A - Analiza** - weryfikacja na podstawie obliczeń / symulacji / z wykorzystaniem narzędzi,
- **D - Demonstracja** - weryfikacja na podstawie obserwacji wyników działania,
- **I - Inspekcja** - weryfikacja na podstawie przeglądu poszczególnych części systemu (np. kodu źródłowego, bazy danych),
- **T - Test** - weryfikacja na podstawie wyniku testu.

## Wykaz wymagań

| ID | Funkcja | Treść wymagania | Metoda weryfikacji |
| :--- | :--- | :--- | :--- |
| WP-01 | Pomiar temperatury | Węzeł pomiarowy ma mierzyć temperaturę z dokładnością do 1 st. Celsjusza. | T |
| WP-02 | Pomiar ciśnienia | Węzeł pomiarowy ma mierzyć ciśnienie atmosferyczne z dokładnością do 1 hPa. | T |
| WP-03 | Pomiar siły wiatru| Węzeł pomiarowy ma mierzyć siłę wiatru z dokładnością do 1 m/s. | T |
| WP-04 | Pomiar kierunku wiatru | Węzeł pomiarowy ma mierzyć kierunek wiatru z dokładnością do 1 st. kątowego. | T |
| WP-05 | Pomiar opadów deszczu | Węzeł pomiarowy ma mierzyć sumę dobową opadów deszczu z dokładnością do 1 l/m&sup2;.| T |
| WP-06 | Transmisja radiowa | Węzeł pomiarowy ma przesyłać sformatowane ramki danych do urządzenia centralnego drogą radiową | D |
| WP-07 | Autodiagnoza | Węzeł pomiarowy powinien wykrywać problemy ze swoim działaniem. | T |
| WP-08 | Zgłaszanie problemów | Węzeł pomiarowy może zgłaszać użytkownikowi problemy ze swoim działaniem. | D |
| UC-01 | Odbiór danych | Urządzenie centralne ma odbierać ramki danych nadawane przez węzły pomiarowe. | T |
| UC-02 | Formatowanie danych | Urządzenie centralne ma formatować dane w sposób zgodny z bazą danych. | I |
| UC-03 | Archiwizacja danych | Urządzenie centralne ma archiwizować zebrane dane w bazie danych.| I |
| UC-04 | Samostart | Urządzenie centralne powinno wznowić normalną pracę po utracie zasilania bez ingerencji człowieka w czasie krótszym niż 1 minuta. | T |
| INT-01 | Dostęp przez www | Strona internetowa ma udostępniać aktualne dane pogodowe użytkownikom. | D |
| INT-02 | Dostęp do danych przez GUI | Aplikacja graficzna ma pozwalać na dostęp do danych pomiarowych klientowi. | D |
| INT-03 | Administracja przez GUI | Aplikacja graficzna ma pozwalać administratorowi na zarządzanie kontami użytkowników. | D |
| INT-04 | Generowanie raportów | Aplikacja graficzna powinna generować raporty okresowe na podstawie danch z bazy. | D |
| INT-05 | Ostrzeganie o zmianie warunków | Strona internetowa może pozwalać na zdefiniowanie zakresów wartości pomiarowych, których przekroczenie spowoduje wysłanie ostrzeżenia drogą mailową do użytkownika. | D |