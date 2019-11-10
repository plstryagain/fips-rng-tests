#include <fips_rng_tests.h>

#include <stdlib.h>
#include <string.h>

const int FIPS_RNG_MONOBIT_TEST = 0x00000001;
const int FIPS_RNG_POKER_TEST = 0x00000002;
const int FIPS_RNG_RUNS_TEST = 0x00000004;
const int FIPS_RNG_LONG_RUNS_TEST = 0x00000008;

const int FIPS_MONOBIT_LOW_LIMIT = 9725;
const int FIPS_MONOBIT_HIGH_LIMIT = 10275;

const int FIPS_POKER_LOW_LIMIT = 10800;
const int FIPS_POKER_HIGH_LIMIT = 230850;

const int FIPS_RUNS_LOW_LIMITS[] = { 2343, 1135, 542, 251, 111, 111 };
const int FIPS_RUNS_HIGH_LIMITS[] = { 2657, 1365, 708, 373, 201, 201 };

const int FIPS_LONG_RUNS_LIMIT = 25;

static int check_monobit_test_limits(const int sum)
{
    return (sum > FIPS_MONOBIT_LOW_LIMIT && sum < FIPS_MONOBIT_HIGH_LIMIT) ? 1 : 0;
}

static int check_poker_test_limits(const int* freq)
{
    int sum = 0;
    for(int i = 0; i < 16; ++i){
        sum += freq[i] * freq[i];
    }
    sum = 16 * sum - 25000000;
    return (sum > FIPS_POKER_LOW_LIMIT && sum < FIPS_POKER_HIGH_LIMIT) ? 1 : 0;
}

static int check_runs_test_limits(const int* one_runs, const int* null_runs)
{
    int res = 1;
    for(int i = 0; i < 6; ++i){
        if(!(one_runs[i] > FIPS_RUNS_LOW_LIMITS[i] &&
                one_runs[i] < FIPS_RUNS_HIGH_LIMITS[i] &&
                null_runs[i] > FIPS_RUNS_LOW_LIMITS[i] &&
                null_runs[i] < FIPS_RUNS_HIGH_LIMITS[i])) 
        {
            res = 0;
            break;
        }
    }
    return res;
}

struct fips_rng_ctx* fips_rng_ctx_new(void)
{
    return (struct fips_rng_ctx*)malloc(sizeof(struct fips_rng_ctx));
}

void fips_rng_ctx_init(struct fips_rng_ctx* ctx)
{
    memset(ctx, 0x00, sizeof(struct fips_rng_ctx));
    ctx->runs.slength = -1;
}

void fips_rng_ctx_free(struct fips_rng_ctx* ctx)
{
    free(ctx);
}

int fips_rng_tests_run(struct fips_rng_ctx* ctx, const unsigned char* rand_data, const int rand_data_size)
{
    for(int i = 0; i < rand_data_size; ++i){
        /* interpret one byte as two 4-bit segments */
        unsigned char low_segment = rand_data[i] & 0x0F;
        unsigned char high_segment = rand_data[i] >> 4;
        ctx->poker.freq[low_segment] += 1;
        ctx->poker.freq[high_segment] += 1;

        unsigned char shift = 0x01;
        while(shift){
            if(rand_data[i] & shift){
                ctx->mono_bit.sum += 1;
                ctx->runs.current_bit = 1;
            } else {
                ctx->runs.current_bit = 0;
            }
            /* just for the first bit */
            if(ctx->runs.slength == -1){
                ctx->runs.last_bit = ctx->runs.current_bit;
                ctx->runs.slength += 1;
                shift <<= 1;
                continue;
            }

            if(ctx->runs.current_bit != ctx->runs.last_bit){
                int idx = ctx->runs.slength < 5 ? ctx->runs.slength : 5;
                if(ctx->runs.last_bit){
                    ctx->runs.one_runs[idx] += 1;
                } else {
                    ctx->runs.null_runs[idx] += 1;
                }
                if(ctx->runs.slength >= FIPS_LONG_RUNS_LIMIT){
                    /* Long runs test failed */
                    ctx->failed_tests |= FIPS_RNG_LONG_RUNS_TEST;
                }
                ctx->runs.slength = 0;
                ctx->runs.last_bit = ctx->runs.current_bit;
            } else {
                ctx->runs.slength += 1;
            }
            shift <<= 1;
            if(i == rand_data_size - 1 && shift == 0x00){
                if(ctx->runs.slength < 5){
                    if(ctx->runs.current_bit){
                        ctx->runs.one_runs[ctx->runs.slength] += 1;
                    } else {
                        ctx->runs.null_runs[ctx->runs.slength] += 1;
                    }
                } else {
                    if(ctx->runs.current_bit){
                        ctx->runs.one_runs[5] += 1;
                    } else {
                        ctx->runs.null_runs[5] += 1;
                    }
                    if(ctx->runs.slength >= FIPS_LONG_RUNS_LIMIT){
                        /* Long runs test failed */
                        ctx->failed_tests |= FIPS_RNG_LONG_RUNS_TEST;
                    }
                }
            }
        }
    }

    /* now check limits for the tests */
    if(!check_monobit_test_limits(ctx->mono_bit.sum)){
        ctx->failed_tests |= FIPS_RNG_MONOBIT_TEST;
    }
    if(!check_poker_test_limits(ctx->poker.freq)){
        ctx->failed_tests |= FIPS_RNG_POKER_TEST;
    }
    if(!check_runs_test_limits(ctx->runs.one_runs, ctx->runs.null_runs)){
        ctx->failed_tests |= FIPS_RNG_RUNS_TEST;
    }
    return ctx->failed_tests ? 0 : 1;
}