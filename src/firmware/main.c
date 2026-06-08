#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
bool is_prime(int n);
int count_primes(int limit);
void run_tests();

void run_tests() {
    printf("Uruchamianie testów jednostkowych...\n");
    
    // Testy dla funkcji is_prime
    assert(is_prime(2) == true);
    assert(is_prime(4) == false);
    assert(is_prime(13) == true);
    
    // Test dla funkcji count_primes
    // W przedziale do 10 liczby pierwsze to: 2, 3, 5, 7 (razem 4)
    assert(count_primes(10) == 4);
    
    printf("Wszystkie testy zakończone sukcesem! (Green phase)\n\n");
}

// bool is_prime(int n) {
//     if (n < 2) return false;
//     for (int i = 2; i < n; i++) {
//         if (n % i == 0) return false;
//     }
//     return true;
// }

bool is_prime(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    
    int limit = sqrt(n);
    
    for(int i = 3; i <= limit; i += 2) {
        if(n % i == 0) return false;
    }
    return true;
}

int count_primes(int limit) {
    int count = 0;
    for (int i = 2; i <= limit; i++) {
        if (is_prime(i)) {
            count++;
        }
    }
    return count;
}

int main(int argc, char *argv[]) {
    run_tests();

    int limit = 150000; 
    printf("Obliczanie liczby pierwszych do %d...\n", limit);
    int total_primes = count_primes(limit);
    printf("Znaleziono liczb pierwszych: %d\n", total_primes);

    return 0;
}