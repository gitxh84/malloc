#include <stdlib.h>
#include <stdio.h>
#include "heaplib.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

// most updated version, 12/6/2017


/* -------------------- DEFINITIONS ----------------- */

/* to keep things simple, support only 5 blocks of memory at a time*/
#define SIZE_TO_SPLIT 16	// if after splitting have room for >= 16 more bytes of data then split

/* Useful shorthand: casts a pointer to a (char *) before adding */
#define ADD_BYTES(base_addr, num_bytes) (((char *)(base_addr)) + (num_bytes))
#define SUB_BYTES(base_addr, num_bytes) (((char *)(base_addr)) - (num_bytes))



typedef struct heap_header {
    unsigned int heap_size;		// always include meta data
    void *first_free;	// always point to block header, not actual data space

} heap_header_t ;


typedef struct linked_list_node {
	unsigned int block_size;	// always include meta data
	bool block_taken;
	struct linked_list_node *prev_free;
	struct linked_list_node *next_free;

} linked_list_node_t ;





/* ------------------- HELPER FUNCTIONS ----------------- */


/* Return a pointer to the first block in the heap. This function adds some paddle between the heap meta
 * data and the block meta data when necessary to ensure that the data of first block is 8-byte aligned.
 */
void *find_first_block(heap_header_t *header) {

	void *temp1 = ADD_BYTES(header, sizeof(heap_header_t));	// block header
	void *temp2 = ADD_BYTES(temp1, sizeof(linked_list_node_t));	// block data
	if (((uintptr_t)temp2) % ALIGNMENT != 0) {
		temp1 = ADD_BYTES(temp1, ALIGNMENT - ((uintptr_t)temp2) % ALIGNMENT);
	}

	return temp1;
}


/* Return the number of bytes the first block needs to paddle to make second block's data 8-byte aligned.
 * This function is called when split a single block into two in order to allocate for block_size space.
 */
unsigned int paddle_amount(linked_list_node_t *block, unsigned int block_size) {

	void *temp = ADD_BYTES(block, sizeof(linked_list_node_t) + block_size + sizeof(linked_list_node_t));
	if (((uintptr_t)temp) % ALIGNMENT != 0) {
		return (ALIGNMENT - ((uintptr_t)temp) % ALIGNMENT);
	}

	return 0;
}


/* Return actual size of heap, i.e total size subtract meta data size */
unsigned int heap_data_size(heap_header_t *header) {

	unsigned int total_size = header->heap_size;
	return (total_size - sizeof(heap_header_t));
}



/* Return actual size of block, i.e total size subtract meta data size */
unsigned int block_data_size(linked_list_node_t *block) {

	unsigned int total_size = block->block_size;
	return (total_size - sizeof(linked_list_node_t));
}



/* Change the prev_free field of blocks following block1 to block2, usually due to alloc. */
void update_prev_free(heap_header_t *heap, linked_list_node_t *block1, linked_list_node_t *block2) {

	void *stopping_point = ADD_BYTES(heap, heap->heap_size);	// end of heap
	if (block1->next_free != NULL) {
		stopping_point = ADD_BYTES(block1->next_free, block1->next_free->block_size);	// end of next_free
	}

	linked_list_node_t *next = (linked_list_node_t *)(ADD_BYTES(block1, block1->block_size));
	while ((uintptr_t)next < (uintptr_t)stopping_point) {
		next->prev_free = block2;
		next = (linked_list_node_t *)(ADD_BYTES(next, next->block_size));
	}

}


/* Change the next_free field of blocks before block1 to block2, usually due to alloc. */
void update_next_free(heap_header_t *heap, linked_list_node_t *block1, linked_list_node_t *block2) {

	void *starting_point = find_first_block(heap);	// very first block in heap
	void *stopping_point = (void *)block1;
	if (block1->prev_free != NULL) {
		starting_point = (void *)(block1->prev_free);		// start of prev_free
	}

	linked_list_node_t *next = (linked_list_node_t *)starting_point;
	while ((uintptr_t)next < (uintptr_t)stopping_point) {
		next->next_free = block2;
		next = (linked_list_node_t *)(ADD_BYTES(next, next->block_size));
	}
	
}




/* -------------------- THE BIG FOUR FNS ----------------- */


/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * Sets up a new heap beginning at heap of size heap_size (in bytes). This function does not allocate the
 * heap (of size heap_size) pointed to by heap. That should be done already by the user calling this function.
 * Specifically, this function initializes the heap and the first single large block, which starts after the
 * heap header with possibly some paddle space to ensure that the data part 8-byte aligned.
 *
 * PRECONDITIONS:
 * 1. the allocated heap must be heap_size in bytes
 * 2. heap_size must be >= MIN_HEAP_SIZE (here, 1024)
 */
void hl_init(void *heap, unsigned int heap_size) {

	// initialize heap_header
	heap_header_t *header = (heap_header_t *)heap;
	header->heap_size = heap_size;
	header->first_free = find_first_block(header);

	// initialize first block, originally a large free block
	linked_list_node_t *temp = (linked_list_node_t *)(header->first_free);
	temp->block_size = heap_data_size(header);
	temp->block_taken = false;
	temp->prev_free = NULL;
	temp->next_free = NULL;

}


/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * Allocates a block of memory of size block_size bytes from the heap starting at heap. Returns a pointer to
 * the block on success; returns 0 (NULL) if the allocator cannot satisfy the request. Specifically, given a
 * request, we loop over all free blocks whose actual data size is at least the requested block size, then we
 * find the best fit, namely the block with fewest leftover. We decide whether to split that block or not based
 * on the following policy. If the best fit has a leftover large enough to contain the paddle amount, another
 * header, and extra SIZE_TO_SPLIT bytes of data then we split. Else, we do not and simply use that best fit block.
 *
 * PRECONDITIONS:
 * 1. hl_init must have been called exactly once for this heap
 */
void *hl_alloc(void *heap, unsigned int block_size) {

    heap_header_t *header = (heap_header_t *)heap;
    linked_list_node_t *next = (linked_list_node_t *)(header->first_free);
    unsigned int is_first_free = 0;

    // loop over all blocks to find the best fit
    linked_list_node_t *best_fit = NULL;
    unsigned int smallest_leftover = header->heap_size;
    while (next != NULL) {
    	if (next->block_taken == false) {
    		if (block_data_size(next) >= block_size) {
    			unsigned int leftover = block_data_size(next) - block_size;
    			if (leftover < smallest_leftover) {
    				smallest_leftover = leftover;
    				best_fit = next;
    			}
    		}
    	}

    	next = next->next_free;
    	is_first_free = is_first_free + 1;
    }


    // didn't find any qualified block
    if (best_fit == NULL) {
    	return FAILURE;
    }


    // if leftover is large enough such that after splitting we have room for the paddle amount, the second block
    // header, and at least SIZE_TO_SPLIT bytes of space for data, then split
    if (smallest_leftover >= paddle_amount(best_fit, block_size) + sizeof(linked_list_node_t) + SIZE_TO_SPLIT) {
    	// split

    	// update new block's content
    	unsigned int space_to_second = sizeof(linked_list_node_t) + block_size + paddle_amount(best_fit, block_size);
    	linked_list_node_t *second_block = (linked_list_node_t *)(ADD_BYTES(best_fit, space_to_second));
    	second_block->block_size = smallest_leftover - paddle_amount(best_fit, block_size);
    	second_block->block_taken = false;
    	second_block->prev_free = best_fit->prev_free;
    	second_block->next_free = best_fit->next_free;


    	// update this block's content
    	best_fit->block_taken = true;
    	best_fit->block_size = space_to_second;


    	// update other blocks' content and heap's content
    	update_prev_free(header, second_block, second_block);	// change the prev_free of blocks following second_block to itself
   		update_next_free(header, second_block, second_block);	// change the next_free of blocks before second_block to itself
    	if (is_first_free == 1) {
    		header->first_free = (void *)(second_block);	// if this was the first free, then it's not anymore
    	}

    	
    	return (ADD_BYTES(best_fit, sizeof(linked_list_node_t)));

    }


    // no split

    // update this block's content
    best_fit->block_taken = true;

    // udate other blocks' content and heap's content
    update_prev_free(header, best_fit, best_fit->prev_free);
    update_next_free(header, best_fit, best_fit->next_free);
    if (is_first_free == 1) {
    	header->first_free = (void *)(best_fit->next_free);	// if this was the first free, then it's not anymore
    }

    return (ADD_BYTES(best_fit, sizeof(linked_list_node_t)));

}



/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * Releases the block of memory pointed to by block (which currently resides in the heap pointed to by heap).
 * Acts as a NOP if block == 0 (NULL). Specifically, we simply clear the "allocated" flag and update content
 * of neighboring blocks and the heap, we ignore the "false fragmentation" disadvanatge for now.
 *
 * PRECONDITIONS:
 * 1. block must have been returned to the user from a prior call to hl_alloc or hl_resize
 * 2. hl_release can only be called ONCE in association with that prior call to hl_alloc or hl_resize
 */
void hl_release(void *heap, void *block) {

	// IMPORTANT: User doesn't know about meta data, so "block" is starting address of data, not actual block
	if (block != NULL) {


		heap_header_t *header = (heap_header_t *)heap;
		linked_list_node_t *next = (linked_list_node_t *)(SUB_BYTES(block, sizeof(linked_list_node_t)));

		// update this block's content
		next->block_taken = false;


		// update other blocks' and heap's content
		update_prev_free(header, next, next);	// change the prev_free of blocks following next to itself
		update_next_free(header, next, next);	// change the next_free of blocks before next to itself
		if ((uintptr_t)(header->first_free) > (uintptr_t)next) {
			header->first_free = (void *)next;
		}
	}
}



/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * Changes the size of the block pointed to by block (that currently resides in the heap pointed to by heap) from
 * its current size to size new_size bytes, returning a pointer to the new block, or 0 if the request cannot be
 * satisfied. The contents of the block should be preserved (even if the location of the block changes -- this will
 * happen when it is not possible to increase the size of the current block but there is room elsewhere on the heap
 * to satisfy the request). If block has the value 0 (NULL), the function should behave like hl_alloc. The new block
 * size might be smaller than the current size of the block. As for hl_alloc, the return value should be 8-byte aligned.
 *
 * PRECONDITIONS:
 * 1. block must have been returned to the user from a prior call to hl_alloc or hl_resize
 */
void *hl_resize(void *heap, void *block, unsigned int new_size) {

	// IMPORTANT: User doesn't know about meta data, so "block" is starting address of data, not actual block

	if (block == NULL) {
		return hl_alloc(heap, new_size);
	}

	heap_header_t *header = (heap_header_t *)heap;
	linked_list_node_t *next = (linked_list_node_t *)(SUB_BYTES(block, sizeof(linked_list_node_t)));
	// change to a smaller or same size, for now just use original
	if (new_size <= block_data_size(next)) {


		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// if small enough so that we can split
		if (block_data_size(next) - new_size >= paddle_amount(next, new_size) + sizeof(linked_list_node_t) + SIZE_TO_SPLIT) {

			// update new block's content
			unsigned int space_to_second = sizeof(linked_list_node_t) + new_size + paddle_amount(next, new_size);
    		linked_list_node_t *second_block = (linked_list_node_t *)(ADD_BYTES(next, space_to_second));
    		second_block->block_size = block_data_size(next) - new_size - paddle_amount(next, new_size);
    		second_block->block_taken = false;
    		second_block->prev_free = next->prev_free;
    		second_block->next_free = next->next_free;

    		// update this block's content
    		next->block_size = space_to_second;

    		// update other blocks' content and heap's content
    		update_prev_free(header, second_block, second_block);	// change the prev_free of blocks following second_block to itself
   			update_next_free(header, second_block, second_block);	// change the next_free of blocks before second_block to itself
    		if ((uintptr_t)(header->first_free) > (uintptr_t)second_block) {
    			header->first_free = (void *)(second_block);
    		}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



		return block;
	}

	// change to a large size
	void *new_block = hl_alloc(heap, new_size);
	if (new_block != NULL) {

		memmove(new_block, block, block_data_size(next));	// copy the data
		hl_release(heap, block);	// release it
		return new_block;	// return the new one
	}

    return FAILURE;
}

