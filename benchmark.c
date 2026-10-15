#define _GNU_SOURCE
#include <inttypes.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern uint8_t b_130c1(uint64_t, uint16_t); extern uint8_t a_130c1(uint64_t, uint16_t);
extern uint8_t b_130c3(uint64_t, uint16_t); extern uint8_t a_130c3(uint64_t, uint16_t);
extern uint8_t b_130c4(uint64_t, uint16_t); extern uint8_t a_130c4(uint64_t, uint16_t);
extern uint8_t b_152(uint64_t, uint64_t, uint32_t, uint16_t); extern uint8_t a_152(uint64_t, uint64_t, uint32_t, uint16_t);
extern uint8_t b_203(uint64_t, uint16_t); extern uint8_t a_203(uint64_t, uint16_t);
extern uint8_t b_429(uint64_t, uint16_t); extern uint8_t a_429(uint64_t, uint16_t);
extern uint8_t b_432(uint64_t, uint8_t, uint16_t); extern uint8_t a_432(uint64_t, uint8_t, uint16_t);
extern uint8_t b_433(uint64_t, uint16_t); extern uint8_t a_433(uint64_t, uint16_t);

enum { DATA_N = 65536 };
static uint64_t data[DATA_N];
static volatile uint64_t sink;

static uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

static void init_data(void) {
    uint64_t x = 0x9e3779b97f4a7c15ULL;
    for (size_t i = 0; i < DATA_N; ++i) {
        x ^= x >> 12; x ^= x << 25; x ^= x >> 27;
        data[i] = x * 0x2545f4914f6cdd1dULL;
    }
}

#define LOOP2(NAME, FN) \
static uint64_t run_##NAME(uint64_t n) { \
    uint64_t s = 0, t0 = now_ns(); \
    for (uint64_t i = 0; i < n; ++i) { uint64_t x=data[i & (DATA_N-1)]; s += FN(x, (uint16_t)(x >> 32)) & 1; } \
    sink ^= s; return now_ns() - t0; \
}

LOOP2(b130c1, b_130c1) LOOP2(a130c1, a_130c1)
LOOP2(b130c3, b_130c3) LOOP2(a130c3, a_130c3)
LOOP2(b130c4, b_130c4) LOOP2(a130c4, a_130c4)
LOOP2(b203, b_203) LOOP2(a203, a_203)
LOOP2(b429, b_429) LOOP2(a429, a_429)
LOOP2(b433, b_433) LOOP2(a433, a_433)

static uint64_t run_b152(uint64_t n) { uint64_t s=0,t0=now_ns(); for(uint64_t i=0;i<n;++i){uint64_t x=data[i&(DATA_N-1)],y=data[(i+1)&(DATA_N-1)];s+=b_152(x&1,y,(uint32_t)(x>>7),(uint16_t)x)&1;} sink^=s;return now_ns()-t0; }
static uint64_t run_a152(uint64_t n) { uint64_t s=0,t0=now_ns(); for(uint64_t i=0;i<n;++i){uint64_t x=data[i&(DATA_N-1)],y=data[(i+1)&(DATA_N-1)];s+=a_152(x&1,y,(uint32_t)(x>>7),(uint16_t)x)&1;} sink^=s;return now_ns()-t0; }
static uint64_t run_b432(uint64_t n) { uint64_t s=0,t0=now_ns(); for(uint64_t i=0;i<n;++i){uint64_t x=data[i&(DATA_N-1)];s+=b_432(x,(uint8_t)(x&1),(uint16_t)x);} sink^=s;return now_ns()-t0; }
static uint64_t run_a432(uint64_t n) { uint64_t s=0,t0=now_ns(); for(uint64_t i=0;i<n;++i){uint64_t x=data[i&(DATA_N-1)];s+=a_432(x,(uint8_t)(x&1),(uint16_t)x);} sink^=s;return now_ns()-t0; }

struct test { const char *name; uint64_t (*before)(uint64_t); uint64_t (*after)(uint64_t); };

static int verify(void) {
    for (size_t i=0;i<DATA_N;++i) {
        uint64_t x=data[i],y=data[(i+1)&(DATA_N-1)]; uint16_t z=(uint16_t)(x>>32);
#define CHECK(ID, EXPR) do { if (!(EXPR)) { fprintf(stderr,"mismatch=%s index=%zu x=%" PRIu64 "\n",ID,i,x); return 0; } } while (0)
        CHECK("130c1", (b_130c1(x,z)&1)==(a_130c1(x,z)&1));
        CHECK("130c3", (b_130c3(x,z)&1)==(a_130c3(x,z)&1));
        CHECK("130c4", (b_130c4(x,z)&1)==(a_130c4(x,z)&1));
        CHECK("152", (b_152(x&1,y,(uint32_t)(x>>7),(uint16_t)x)&1)==(a_152(x&1,y,(uint32_t)(x>>7),(uint16_t)x)&1));
        CHECK("203", (b_203(x,z)&1)==(a_203(x,z)&1));
        CHECK("429", (b_429(x,z)&1)==(a_429(x,z)&1));
        CHECK("432", b_432(x,(uint8_t)(x&1),(uint16_t)x)==a_432(x,(uint8_t)(x&1),(uint16_t)x));
        CHECK("433", (b_433(x & INT64_MAX,z)&1)==(a_433(x & INT64_MAX,z)&1));
#undef CHECK
    }
    return 1;
}

int main(int argc, char **argv) {
    uint64_t iterations = argc > 1 ? strtoull(argv[1], NULL, 10) : 20000000ULL;
    int rounds = argc > 2 ? atoi(argv[2]) : 21;
    cpu_set_t cpus; CPU_ZERO(&cpus); CPU_SET(0,&cpus); sched_setaffinity(0,sizeof(cpus),&cpus);
    init_data();
    if (!verify()) { fputs("correctness mismatch\n", stderr); return 2; }
    struct test tests[] = {
        {"rank130-c1",run_b130c1,run_a130c1},{"rank130-c3",run_b130c3,run_a130c3},
        {"rank130-c4",run_b130c4,run_a130c4},{"rank152",run_b152,run_a152},
        {"rank203",run_b203,run_a203},{"rank429",run_b429,run_a429},
        {"rank432",run_b432,run_a432},{"rank433",run_b433,run_a433}
    };
    puts("candidate,round,order,before_ns,after_ns,iterations");
    for (size_t t=0;t<sizeof(tests)/sizeof(tests[0]);++t) {
        tests[t].before(1000000); tests[t].after(1000000);
        for (int r=0;r<rounds;++r) {
            uint64_t b,a;
            if ((r&1)==0) { b=tests[t].before(iterations); a=tests[t].after(iterations); }
            else { a=tests[t].after(iterations); b=tests[t].before(iterations); }
            printf("%s,%d,%s,%" PRIu64 ",%" PRIu64 ",%" PRIu64 "\n",tests[t].name,r,(r&1)?"AB":"BA",b,a,iterations);
        }
    }
    fprintf(stderr,"correctness=PASS sink=%" PRIu64 "\n",sink);
    return 0;
}
