#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define REPS 1000
#define MREPS 10

static __inline__ uint64_t start(void) {
  unsigned cycles_low, cycles_high;
  asm volatile("CPUID\n\t"
               "RDTSC\n\t"
               "mov %%edx, %0\n\t"
               "mov %%eax, %1\n\t"
               : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx",
                 "%rdx");
  return ((uint64_t)cycles_high << 32) | cycles_low;
}

static __inline__ uint64_t stop(void) {
  unsigned cycles_low, cycles_high;
  asm volatile("RDTSCP\n\t"
               "mov %%edx, %0\n\t"
               "mov %%eax, %1\n\t"
               "CPUID\n\t"
               : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx",
                 "%rdx");
  return ((uint64_t)cycles_high << 32) | cycles_low;
}

long op(long x, long y) { return x / y; }

void measure(long x, long y) {
  int j;
  uint64_t times[MREPS];
  volatile long result;
  for (j = 0; j < MREPS; j++) {
    uint64_t t1 = start();
    int i;
    for (i = 0; i < REPS; i++) {
      volatile long x_copy = x;
      volatile long y_copy = y;
      result = op(x_copy, y_copy);
    }
    times[j] = stop() - t1;
  }
  printf("%ld %ld : ", x, y);
  for (j = 0; j < MREPS; j++) {
    printf("%" PRIu64 " ", times[j]);
  }
  printf("\n");
}

uint64_t rand64(void)
{
  uint64_t a = rand() & 0xffff;
  uint64_t b = rand() & 0xffff;
  uint64_t c = rand() & 0xffff;
  uint64_t d = rand() & 0xffff;
  return (a << 48) | (b << 32) | (c << 16) | d;
}

uint64_t rand8(void)
{
  return rand() & 0xff;
}

int main(void) {
  srand(getpid()+time(0));
  int i;
  for (i=0; i<5; i++)
    measure(rand8(), rand8());
  for (i=0; i<5; i++)
    measure(rand64(), rand64());
  for (i=0; i<5; i++)
    measure(rand64(), rand8());
  for (i=0; i<5; i++)
    measure(rand8(), rand64());
  return 0;
}
