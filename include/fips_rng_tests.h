#ifndef __FIPS__RNG__TEST__H__
#define __FIPS__RNG__TEST__H__

extern const int FIPS_RNG_MONOBIT_TEST;
extern const int FIPS_RNG_POKER_TEST;
extern const int FIPS_RNG_RUNS_TEST;
extern const int FIPS_RNG_LONG_RUNS_TEST;

extern const int FIPS_MONOBIT_LOW_LIMIT;
extern const int FIPS_MONOBIT_HIGH_LIMIT;

extern const int FIPS_POKER_LOW_LIMIT;
extern const int FIPS_POKER_HIGH_LIMIT;

extern const int FIPS_RUNS_LOW_LIMITS[];
extern const int FIPS_RUNS_HIGH_LIMITS[];

extern const int FIPS_LONG_RUNS_LIMIT;

struct fips_rng_ctx{
    struct{
        unsigned int sum;
    } mono_bit;
    struct{
        unsigned int freq[16];
    } poker;
    struct{
        int one_runs[6];
        int null_runs[6];
        int slength;
        unsigned char last_bit;
        unsigned char current_bit;
        int long_runs;
    } runs;
    int failed_tests;
};

extern struct fips_rng_ctx* fips_rng_ctx_new(void);
extern void fips_rng_ctx_init(struct fips_rng_ctx*);
extern void fips_rng_ctx_free(struct fips_rng_ctx*);
extern int fips_rng_tests_run(struct fips_rng_ctx*, const unsigned char*, const int);


#endif // __FIPS__RNG__TEST__H__