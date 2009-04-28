#include "simple_vector.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct simple_vector {
	
	size_t size;
	size_t capacity;
	size_t elem_size;
	
	void *elements;

};

// {{{ simple_vector_t *simple_vector_new(size_t capacity, size_t elem_size)
simple_vector_t *
simple_vector_new(size_t capacity, size_t elem_size)
{
	simple_vector_t *sv;

	if (elem_size == 0) {
		errno = EFAULT;
		return NULL;
	}

	if ((sv = calloc(1, sizeof(simple_vector_t))) == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	sv->size = 0;
	sv->capacity = capacity == 0 ? 1 : capacity;
	sv->elem_size = elem_size;

	if ((sv->elements = calloc(sv->capacity, sv->elem_size)) == NULL) {
		simple_vector_free(sv, 0, NULL);
		errno = ENOMEM;
		return NULL;
	}

	return sv;

} // }}}
// {{{ void simple_vector_free(simple_vector_t *sv, int free_elems, free_fn fn)
void
simple_vector_free(simple_vector_t *sv, int free_elems, free_fn fn)
{
	void *elem;
	size_t i;

	if (sv != NULL) {

		if (free_elems) {
			if (fn == NULL) {
				for (i = 0; i < sv->size; i++) {
					if (simple_vector_get(sv, i, &elem) == 0) {
						free(elem);
					}
				}
			} else {
				for (i = 0; i < sv->size; i++) {
					if (simple_vector_get(sv, i, &elem) == 0) {
						fn(elem);
					}
				}
			}
		}

		free(sv->elements);
		free(sv);

	}

} // }}}

// {{{ int simple_vector_get(simple_vector_t *sv, size_t offset, void *elem)
int
simple_vector_get(simple_vector_t *sv, size_t offset, void *elem)
{

	if (offset >= sv->size) {
		return -1;
	}

	memcpy(elem, sv->elements + offset * sv->elem_size, sv->elem_size);
	return 0;

} // }}}
// {{{ int simple_vector_put(simple_vector_t *sv, size_t offset, void *elem)
int
simple_vector_put(simple_vector_t *sv, size_t offset, void *elem)
{

	if (offset >= sv->size + 1) {
		return -1;
	}

	memcpy(sv->elements + offset * sv->elem_size, elem, sv->elem_size);
	if (offset == sv->size) {
		sv->size++;
	}
	return 0;

} // }}}

// {{{ size_t simple_vector_size(simple_vector_t *sv)
size_t
simple_vector_size(simple_vector_t *sv)
{
	return sv->size;
} // }}}
// {{{ size_t simple_vector_capacity(simple_vector_t *sv)
size_t
simple_vector_capacity(simple_vector_t *sv)
{
	return sv->capacity;
} // }}}

// {{{ int simple_vector_clear(simple_vector_t *sv)
int simple_vector_clear(simple_vector_t *sv) {

	sv->size = 0;

} // }}}
// {{{ int simple_vector_resize(simple_vector_t *sv, size_t capacity)
int
simple_vector_resize(simple_vector_t *sv, size_t capacity)
{
	void *elements;

	if (capacity == 0) {
		capacity = 1;
	}

	if (capacity < sv->size) {
		return -1;
	}

	if ((elements = realloc(sv->elements, capacity * sv->elem_size)) == NULL) {
		return -1;
	}

	if (capacity > sv->capacity) {
		memset(elements + sv->capacity * sv->elem_size, 0, 
				(capacity - sv->capacity) * sv->elem_size);
	}

	sv->elements = elements;
	sv->capacity = capacity;

	return 0;

} // }}}

// {{{ int simple_vector_append(simple_vector_t *sv, void *elem)
int
simple_vector_append(simple_vector_t *sv, void *elem)
{

	if (sv->size == sv->capacity) {
		if (simple_vector_resize(sv, 2 * sv->capacity) == -1) {
			return -1;
		}
	}

	return simple_vector_put(sv, sv->size, elem);

} // }}}
