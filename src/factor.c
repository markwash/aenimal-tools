#include "factor.h"

#include <stdlib.h>
#include <stdio.h>

struct prime_ctx {

	simple_vector_t *primes;
	uint64_t highest_checked; // contiguous only
	uint64_t reach;

};

struct factor_ctx {

	prime_ctx_t *pctx;
	simple_vector_t *factors;
	uint64_t num;

};

// {{{ prime_ctx_t *prime_ctx_new()
prime_ctx_t *prime_ctx_new() {

	prime_ctx_t *ctx;

	if ((ctx = malloc(sizeof(prime_ctx_t))) == NULL) {
		return NULL;
	}

	if ((ctx->primes = simple_vector_new(16, sizeof(uint64_t))) == NULL) {
		prime_ctx_free(ctx);
		return NULL;
	}

	uint64_t i;
	i = 2;
	simple_vector_append(ctx->primes, &i);
	i = 3;
	simple_vector_append(ctx->primes, &i);

	ctx->highest_checked = 3;
	ctx->reach = 9;
	
	return ctx;

} // }}}
// {{{ void prime_ctx_free(prime_ctx_t *ctx)
void prime_ctx_free(prime_ctx_t *ctx) {

	if (ctx != NULL) {
		simple_vector_free(ctx->primes, 0, NULL);
		free(ctx);
	}

} // }}}

// {{{ static int prime_ctx_check_unsafe(prime_ctx_t *ctx, uint64_t num)
static int prime_ctx_check_unsafe(prime_ctx_t *ctx, uint64_t num) {

	size_t i;
	uint64_t p;

	for (i = 0; i < simple_vector_size(ctx->primes); i++) {
		simple_vector_get(ctx->primes, i, &p);

		if (p * p > num) {
			break;
		}

		if (num % p == 0) {
			return 0;
		}

	}

	return 1;
	
} // }}}
// {{{ int prime_ctx_check(prime_ctx_t *ctx, uint64_t num) 
int prime_ctx_check(prime_ctx_t *ctx, uint64_t num) {

	while (num > ctx->reach) {
		prime_ctx_check_next(ctx, NULL);
	}

	return prime_ctx_check_unsafe(ctx, num);
} // }}}
// {{{ int prime_ctx_check_next(prime_ctx_t *ctx, uint64_t *num_r) 
int prime_ctx_check_next(prime_ctx_t *ctx, uint64_t *num_r) {

	if (num_r != NULL) {
		*num_r = ctx->highest_checked + 1;
	}
	
	ctx->highest_checked += 1;
	ctx->reach = ctx->highest_checked * ctx->highest_checked;

	int result = prime_ctx_check_unsafe(ctx, ctx->highest_checked);

	if (result) {
		simple_vector_append(ctx->primes, &ctx->highest_checked);
	}

	return result;

} // }}}

// {{{ factor_ctx_t *factor_ctx_new(prime_ctx_t *pctx, uint64_t num)
factor_ctx_t *factor_ctx_new(prime_ctx_t *pctx, uint64_t num) {

	factor_ctx_t *ctx;

	if ((ctx = malloc(sizeof(factor_ctx_t))) == NULL) {
		return NULL;
	}

	if ((ctx->factors = simple_vector_new(16, sizeof(prime_factor_t))) == NULL) {
		factor_ctx_free(ctx);
		return NULL;
	}
	
	ctx->pctx = pctx;
	ctx->num = num;

	return ctx;

} // }}}
// {{{ void factor_ctx_free(factor_ctx_t *ctx) 
void factor_ctx_free(factor_ctx_t *ctx) {
	
	if (ctx != NULL) {
		simple_vector_free(ctx->factors, 0, NULL);
		free(ctx);
	}

} // }}}

// {{{ void factor_ctx_finish(factor_ctx_t *ctx)
void factor_ctx_finish(factor_ctx_t *ctx) {

	// FIXME: this shouldn't preset all of the primes
	// that is, we should make the prime iteration and the factorisation iterations concurrent

	// ensure the reach is good enough
	while (ctx->pctx->reach < ctx->num) {
		prime_ctx_check_next(ctx->pctx, NULL);
	}
	
	prime_factor_t pf;
	size_t i;

	uint64_t remaining = ctx->num;

	for (i = 0; i < simple_vector_size(ctx->pctx->primes); i++) {

		simple_vector_get(ctx->pctx->primes, i, &pf.prime);
		pf.power = 0;

		while (remaining % pf.prime == 0) {
			pf.power += 1;
			remaining /= pf.prime;
		}

		if (pf.power != 0) {
			simple_vector_append(ctx->factors, &pf);
			printf("%llu ^ %lu\n", pf.prime, pf.power);
		} else if (pf.prime * pf.prime > remaining) {
			break;
		}

	}

	if (remaining != 1) {
		pf.prime = remaining;
		pf.power = 1;
		simple_vector_append(ctx->factors, &pf);
	}

} // }}}

// {{{ void factor_ctx_print(factor_ctx_t *ctx)
void factor_ctx_print(factor_ctx_t *ctx) {

	printf("%llu =", ctx->num);
	
	size_t i;
	prime_factor_t pf;
	for (i = 0; i < simple_vector_size(ctx->factors) - 1; i++) {
		
		simple_vector_get(ctx->factors, i, &pf);

		if (pf.power == 1) {
			printf(" %llu *", pf.prime);
		} else {
			printf(" %llu^%lu *", pf.prime, pf.power);
		}

	}

	simple_vector_get(ctx->factors, i, &pf);
	if (pf.power == 1) {
		printf(" %llu\n", pf.prime);
	} else {
		printf(" %llu^%lu\n", pf.prime, pf.power);
	}

} // }}}
