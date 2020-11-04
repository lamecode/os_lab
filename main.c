#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <malloc.h>
#include <assert.h>

#include "block.h"

static void* PTR;

static int SIZE_H = sizeof(Block_t);

void set_status(void* pointer, size_t status) {
	((Block_t*)pointer)->status = status;
}

bool get_status(void* pointer) {
	return ((Block_t*)pointer)->status;
}

void set_previous_size(void* pointer, size_t previous_size) {
	((Block_t*)pointer)->previous_size = previous_size;
}

size_t get_previous_size(void* pointer) {
	return ((Block_t*)pointer)->previous_size;
}

void set_size(void* pointer, size_t size) {
	((Block_t*)pointer)->size = size;
}

size_t get_size(void* pointer) {
	return ((Block_t*)pointer)->size;
}

void* get_next(void* pointer) {
	if ((uint8_t*)pointer + get_size(pointer) + SIZE_H == (uint8_t*)PTR + BUFFER_SIZE + SIZE_H) {
		return NULL;
	}
	return (uint8_t*)pointer + get_size(pointer) + SIZE_H;
}

void* get_previous(void* pointer) {
	if (pointer == PTR) {
		return NULL;
	}
	return (uint8_t*)pointer - get_previous_size(pointer) - SIZE_H;
}

void new_block(void* pointer, bool status, size_t previous_size, size_t size)
{
	Block_t a;
	a.status = status;
	a.previous_size = previous_size;
	a.size = size;
	*((Block_t*)pointer) = a;
}
void combine_blocks(void* pointer1, void* pointer2) {
	assert(get_status(pointer1) == 0 && get_status(pointer2) == 0);
	set_size(pointer1, get_size(pointer1) + get_size(pointer2) + SIZE_H);
	if (get_next(pointer2) != NULL) {
		set_previous_size(get_next(pointer2), get_size(pointer1));
	}
}

void* block(size_t size) {

	void* pointer = malloc(size);
    new_block(pointer, false, 0, size);

	return pointer;
}

void* get_first(size_t size) {
	void* pointer = PTR;
	void* first = NULL;
	while (pointer != NULL){
		if ((first == NULL) || get_status(first) || get_size(first)
                                                    < size) {
            first = pointer;
		}
		pointer = get_next(pointer);
	}
	return first;
}

void* mem_alloc(size_t size) {
	if (size % 4 != 0) {
		size = size - size % 4 + 4;
	}
	void* pointer = get_first(size);
	if (pointer == NULL) {
		return pointer;
	}

	if (get_size(pointer) > size + SIZE_H) {
        new_block((uint8_t *) pointer + size + SIZE_H, 0, size,
                  get_size(pointer) - size - SIZE_H);
		set_size(pointer, size);
	}
	set_status(pointer, true);
	return (uint8_t*)pointer + SIZE_H;
}

void mem_free(void* pointer) {
	pointer = (uint8_t*)pointer - SIZE_H;
	set_status(pointer, false);
	if (get_next(pointer) != NULL && get_status(get_next(pointer)) == 0) {
        combine_blocks(pointer, get_next(pointer));
	}
	if (get_previous(pointer) != NULL && get_status(get_previous(pointer)) == 0) {
        combine_blocks(get_previous(pointer), pointer);
	}
}

void* mem_realloc(void* pointer, size_t size) {
	pointer = (uint8_t*)pointer - SIZE_H;
	if (size % 4 != 0) {
		size = size - size % 4 + 4;
	}
	if (get_size(pointer) == size) {
		return pointer;
	}
	if (get_size(pointer) > size) {
		if (get_size(pointer) - size - SIZE_H >= 0) {
            new_block((uint8_t *) pointer + size + SIZE_H, false,
                      size, get_size(pointer) - size - SIZE_H);
			set_size(pointer, size);
			if (get_next(get_next(pointer)) != NULL &&
				get_status(get_next(get_next(pointer))) == 0) {
                combine_blocks(get_next(pointer), get_next(get_next(pointer)));
			}
		}
		return pointer;
	}
	if (get_next(pointer) != NULL && get_size(pointer) + get_size(get_next(pointer))
		>= size) {
        new_block((uint8_t *) pointer + size + SIZE_H, false, size,
                  get_size(get_next(pointer)) - (size - get_size(pointer)));
		set_size(pointer, size);
		return pointer;
	}
	if (get_next(pointer) != NULL && get_status(get_next(pointer)) == 0 &&
		get_size(pointer) + get_size(get_next(pointer)) + SIZE_H >= size) {
		set_size(pointer, get_size(pointer) + get_size(get_next(pointer)) + SIZE_H);
		return pointer;
	}
	void* first = mem_alloc(size);
	if (first == NULL) {
		return first;
	}
	mem_free((uint8_t*)pointer + SIZE_H);
	return first;
}
void mem_dump() {
	void* pointer = PTR;
	size_t size_h = 0;
	size_t size_b = 0;
	while (pointer != NULL) {
        printf("Address: % 15p, Status: % 2d, Pr_size: % 7ld, Size: % 15p, "
               "Previous: %15p, Next: %15d \n", pointer,
               get_status(pointer), get_previous_size(pointer), get_size(pointer),
               get_previous(pointer), get_next(pointer));
		size_h = size_h + SIZE_H;
		size_b = size_b + get_size(pointer);
		pointer = get_next(pointer);
	}
	printf("----------------------------------------------------------------------\n");
	printf("headers: %ld\nblocks : %ld\nsummary : %ld\n\n\n", size_h, size_b, size_h +
		size_b);
}

int main() {
	PTR = block(BUFFER_SIZE);
	mem_dump();
	void* x1 = mem_alloc(20);
	mem_dump();
	mem_realloc(x1, 10);
	mem_dump();
	mem_free(x1);
	mem_dump();
	void* x2 = mem_alloc(1);
	mem_dump();
	void* x3 = mem_alloc(3);
	mem_dump();
	void* x4 = mem_alloc(80);
	mem_dump();
	mem_free(x2);
	mem_free(x3);
	mem_dump();
	void* x5 = mem_alloc(20);
	void* x6 = mem_alloc(20);
	mem_dump();
	x1 = mem_realloc(x1, 30);
	mem_dump();
	return 0;
}