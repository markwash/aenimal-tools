#ifndef SIMPLE_VECTOR_H
#define SIMPLE_VECTOR_H

#include <sys/types.h>

struct simple_vector;
typedef struct simple_vector simple_vector_t;

typedef void (free_fn)(void *);

simple_vector_t *simple_vector_new(size_t capacity, size_t elem_size);
void simple_vector_free(simple_vector_t *sv, int free_elems, free_fn fn);

int simple_vector_get(simple_vector_t *sv, size_t offset, void *elem);
int simple_vector_put(simple_vector_t *sv, size_t offset, void *elem);

size_t simple_vector_size(simple_vector_t *sv);
size_t simple_vector_capacity(simple_vector_t *sv);

int simple_vector_clear(simple_vector_t *sv);
int simple_vector_resize(simple_vector_t *sv, size_t capacity);

int simple_vector_append(simple_vector_t *sv, void *elem);

#endif
