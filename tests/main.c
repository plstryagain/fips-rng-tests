#include <openssl/rand.h>
#include <stdio.h>

#include <fips_rng_tests.h>


int main()
{
    unsigned char bytes[2500];
    int num = 2500;
    if(!RAND_bytes(bytes, num)){
        printf("failed to generate random bytes\n");
        return 0;
    }

    struct fips_rng_ctx* ctx = NULL;
    do{
        ctx = fips_rng_ctx_new();
        if(!ctx){
            break;
        }
        fips_rng_ctx_init(ctx);
        if(!fips_rng_tests_run(ctx, bytes, num)){
            if(ctx->failed_tests & FIPS_RNG_MONOBIT_TEST){
                printf("Monobit test failed\n");
            }
            if(ctx->failed_tests & FIPS_RNG_POKER_TEST){
                printf("Poker test failed\n");
            }
            if(ctx->failed_tests & FIPS_RNG_RUNS_TEST){
                printf("Runs test failed\n");
            }
            if(ctx->failed_tests & FIPS_RNG_LONG_RUNS_TEST){
                printf("Long runs test failed\n");
            }
        } else {
            printf("All tests succeed\n");
        }

        printf("The Monobit test sum: %d\n", ctx->mono_bit.sum);
        printf("The Poker test result:\n");
        for(int i = 0; i < 16; ++i){
            printf("%d: %d\n", i, ctx->poker.freq[i]);
        }
        printf("The Runs test result:\n");
        for(int i = 0; i < 6; ++i){
            printf("series of %d ones: %d\n", i + 1, ctx->runs.one_runs[i]);
            printf("series of %d zeros: %d\n", i + 1, ctx->runs.null_runs[i]);
        }
    } while (0);

    if(ctx){
        fips_rng_ctx_free(ctx);
    }
    

    return 1;
}