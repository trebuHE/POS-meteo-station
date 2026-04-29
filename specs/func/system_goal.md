# Cel systemu

Głównym celem projektowanego systemu jest dostarczenie kompleksowego rozwiązania do automatycznego monitorowania, gromadzenia oraz analizy parametrów meteorologicznych na terenie Rodzinnych Ogródków Działkowych (ROD). System ma na celu wsparcie działkowiczów w optymalizacji upraw oraz ułatwienie zarządcy nadzoru nad warunkami środowiskowymi panującymi na terenie całego obiektu.

Do szczegółowych celów systemu należą:
- **Automatyzacja pomiarów** - zapewnienie ciągłego i bezobsługowego odczytu kluczowych parametrów pogodowych, takich jak temperatura, ciśnienie atmosferyczne, siła i kierunek wiatru oraz suma opadów.
- **Centralizacja i archiwizacja danych** - przesyłanie danych z rozproszonych węzłów pomiarowych drogą radiową do jednostki centralnej i ich trwałe składowanie w bazie danych.
- **Udostępnianie informacji** - prezentacja aktualnych danych w czasie rzeczywistym działkowiczom za pośrednictwem strony internetowej oraz udostępnienie narzędzi administracyjnych i raportowych zarządcy poprzez dedykowany interfejs graficzny.
- **Wsparcie procesów decyzyjnych** - generowanie raportów okresowych umożliwiających analizę trendów pogodowych i ocenę warunków wegetacji roślin.

System został zaprojektowany w architekturze rozproszonej, gdzie niskomocowe węzły pomiarowe oparte na mikrokontrolerach STM32 pełnią rolę dostawców danych, a jednostka centralna pełni funkcję bramy sieciowej, serwera bazy danych i serwera aplikacji.