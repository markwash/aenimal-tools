#ifndef FACTOR_H
#define FACTOR_H

#include <stdint.h>

#include "simple_vector.h"

struct prime_ctx;
typedef struct prime_ctx prime_ctx_t;

struct factor_ctx;
typedef struct factor_ctx factor_ctx_t;

struct prime_factor {
	uint64_t prime;
	uint32_t power;
};
typedef struct prime_factor prime_factor_t;

prime_ctx_t *prime_ctx_new();
void prime_ctx_free(prime_ctx_t *ctx);

int prime_ctx_check(prime_ctx_t *ctx, uint64_t num);
int prime_ctx_check_next(prime_ctx_t *ctx, uint64_t *num_r);

factor_ctx_t *factor_ctx_new(prime_ctx_t *pctx, uint64_t number);
void factor_ctx_free(factor_ctx_t *ctx);

void factor_ctx_finish(factor_ctx_t *ctx);

void factor_ctx_print(factor_ctx_t *ctx);

#endif
