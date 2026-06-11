# Testy kodu węzła pomiarowego

W tym miejscu znajdują się testy jednostkowe fragmentów kodu węzła pomiarowego.

## Testy

W celu zaprezentowania przykładu testów jednostkowych układu przygotowano skrypt `main.c` w którym znajdują się dwie funkcję odpowiedzialne za odliczanie oraz sprawdzanie czy zadana liczba jest liczbą pierwszą. Następnie wykorzystano bibliotekę `assert.h` w celu weryfikacji czy funkcję działają w poprawny sposób.

## Profiler

Jako profiler do analizy działania oprogramowania wykorzystano GNU Profiler. W pliku `analysis.txt` zawarte zostały wyniki niezoptymalizowanego programu. Wykonywanie programu trwało ponad 3 sekundy i znaczną większość czasu zajęła funkcja is_prime(). Po wykonaniu optymalizacji tej funkcji i przeprowadzeniu drugiej analizy profilerem, której wyniki zawarto w pliku `analysis_optimized.txt` udało się skrócić czas działania programu do około 0.1 sekundy.