# Wymagania pozafunkcjonalne

Wymagania pozafunkcjonalne definiują ograniczenia oraz atrybytu jakościowe systemu. 

Wymagania podzielono, przydzielając im ID, zależne od kategorii:
- **PERF-x** - wydajność i zasoby,
- **REL-x** - niezawodność i dostępność,
- **SEC-x** - bezpieczeństwo i ochrona danych,
- **CONS-x** - ograniczenia techniczne i normatywne.

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

| ID | Kategoria | Treść wymagania | Metoda weryfikacji |
| :--- | :--- | :--- | :--- |
| PERF-01 | Wydajność strony internetowej | Czas odpowiedzi strony internetowej na zapytanie użytkownika nie powienien przekraczać 2 sekund. | A |
| PERF-02 | Kod źródłowy | Krytyczne moduły przetwarzania danych muszą zostać poddane profilowaniu w celu wykrycia *hot spotów*. | A |
| PERF-02 | Kod źródłowy | Krytyczne moduły oprogramowania muszą zostać poddane walidacji przy pomocy testów jednostkowych. | T |
| REL-01 | Dostępność strony inrenetowej | Webowy interfejst użytkowanika musi być dostępny przynajmniej przez 95% czasu w skali miesiąca. | T |
| REL-02 | Odporność na błędy odczytu | System musi poprawnie obsłużyć błąd odczytu z węzła pomiarowego poprzez zignorowanie błędnego odczytu. | T |
| SEC-01 | Poufność administracji | Dostęp do panelu administarcyjnego urządzenia centralnego musi być zabezpieczony hasłem. | D |
| SEC-02 | Poufność administracji | System powinien wymuszać zmianę hasła do panelu administracyjnego nie rzadziej niż raz na 3 miesiące. | A |
| SEC-03 | Integralność bazy danych | Baza danych musi zapewniać spójność rekordów w przypadku nagłej utraty zasilania. | T |
| CONS-01 | Komunikacja radiowa | Węzeł pomiarowy musi transmitować dane pomiarowe nie częściej niż raz na 5 minut, by minimalizować wykorzystanie pasma. | I |
| CONS-02 | Komunikacja radiowa | Komunikacja radiowa musi odbywać się w nielicencjonowanym paśmie zgodnie z obowiązującymi przepisami prawa. | I |
| CONS-03 | Dokumentacja kodu źródłowego | Kod źródłowy musi być udokumentowany zgodnie ze standardem [Doxygen](https://github.com/doxygen/doxygen). | I |