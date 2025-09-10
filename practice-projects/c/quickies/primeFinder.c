#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>

// ===== Dynamic Array Setup =====

void* new_array(size_t size) {
    void* arr = malloc(size);
    if (!arr) {
      printf("Error: Couldn't allocate memory for new array");
      exit(1);
    }
    return arr;
}

uint64_t* combine_arrays(uint64_t** arrays, size_t* lengths, const int count) {
    size_t length = lengths[0];
    for (int i=1; i<count; i++) length += lengths[i];

    uint64_t* combined = new_array(length);
    size_t combIdx = 0;

    for (int i=0; i<count; i++)
      for (size_t j=0; j<lengths[i]; j++)
        combined[combIdx++] = arrays[i][j];

    return combined;
}

void free_arrays(uint64_t** arrays, const int count) {
    for (int i=0; i<count; i++) free(arrays[i]);
}

void print_array(uint64_t* array, size_t length) {
    printf("[");
    for (int i=0; i<length; i++) {
      printf("%lu", array[i]);
      if (i < length-1) printf(", ");
    }
    printf("]");

}

// ===== Timer Setup =====

#define _POSIX_C_SOURCE 199309L

double get_time() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

// ===== Prime Calculator Setup =====

bool isPrime(uint64_t n) {
    if (n <= 1) return false;
    if (n <= 3) return true;        // 2 and 3 are prime
    if (n % 2 == 0 || n % 3 == 0) return false;

    uint64_t limit = sqrt(n);
    for (uint64_t i = 5; i <= limit; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

// Helper: Estimates the maximum number of primes in a range
size_t prime_upper_bound(uint64_t a, uint64_t b) {
  if (b < 17) return b;
  double x = (double)a;
  double y = (double)b;
  return (size_t)(1.25506 * y / log(y)) - (size_t)(1.25506 * x / log(x)) + 1;
}

// Multi-threaded prime finder
void* primesInRange(void* args) {
    uint64_t* argList = (uint64_t*)args;
    uint64_t id = argList[0], start = argList[1], end = argList[2];
    uint64_t* primes = (uint64_t*)new_array(prime_upper_bound(start, end) * sizeof(uint64_t));
    uint64_t* length = (uint64_t*)malloc(sizeof(uint64_t));
    *length = 0; 

    for (uint64_t i = start; i <= end; i++) {
        if (isPrime(i)) primes[(*length)++] = i;
    }

    free(argList);  // Free the thread argument memory

    uint64_t** out = (uint64_t**)new_array(sizeof(uint64_t*) * 2);
    out[0] = primes; out[1] = length;
    return (void*)out;
}

int main(void) {
    uint64_t N;
    printf("Enter N: ");
    scanf("%ld", &N);

    double start = get_time();

    const int maxThreads = sysconf(_SC_NPROCESSORS_ONLN);
    uint64_t segment = N / maxThreads;
    pthread_t pids[maxThreads];
    for (int i = 0; i < maxThreads; i++) {
        uint64_t* args = malloc(3 * sizeof(uint64_t));
        args[0] = i;
        args[1] = segment * i + 1;
        args[2] = (i == maxThreads - 1) ? N : segment * (i + 1);
        printf("Thread %d: starting with range %ld—%ld\n", i, args[1], args[2]);
        pthread_create(pids + i, NULL, primesInRange, args);
    }

    uint64_t *prime_outs[maxThreads], lengths[maxThreads];
    for (int i = 0; i < maxThreads; i++) {
        void* ret;
        pthread_join(pids[i], &ret);
        printf("Thread %d: Done!\n", i+1);
        uint64_t** pair = (uint64_t**)ret;
        prime_outs[i] = pair[0];
        lengths[i] = *pair[1];
        free(pair[1]);
        free(pair);
    }

    double end = get_time();

    size_t totalPrimes = lengths[0];
    for (int i=1; i<maxThreads; i++) totalPrimes += lengths[i];

    printf("Complete! Found %ld primes upto %ld in %.9fs\n", totalPrimes, N, end - start);

    // Printing primes (uncomment to print (extremely time consuming))
    // uint64_t* primes = combine_arrays(prime_outs, lengths, maxThreads);
    free_arrays(prime_outs, maxThreads);

    // print_array(primes, totalPrimes);
    // printf("\n");

    // free(primes);
    return 0;
}
