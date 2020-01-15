#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "heaplib.h"

#define HEAP_SIZE 1024
#define NUM_TESTS 24
#define NPOINTERS 100

// TODO: Add test descriptions as you add more tests...
const char* test_descriptions[] = {
    /* our SPEC tests */
    /* 0 */ "single init, should return without error",
    /* 1 */ "single init then single alloc, should pass",
    /* 2 */ "single alloc which should fail b/c heap is not big enough",
    /* 3 */ "multiple allocs, verifying no hard-coded block limit",
    /* your SPEC tests */
    /* 4  */ "verifying alloc never returns less than requested size",
    /* 5  */ "verifying alloc returns 8-byte aligned address",
    /* 6  */ "verifying alloc returns null is request not satisfied, and allocated blocks do not overlap",
    /* 7  */ "verifying release acts as NOP if block = null, and memory contents do not change",
    /* 8  */ "verifying resize returns a pointer to new block or 0 if the request cannot be satisfied",
    /* 9  */ "verifying resize acts as alloc if block = null, and returned address is 8-byte aligned",
    /* 10 */ "verifying resize should make contents of the block preserved",
    /* 11 */ "your description here",
    /* 12 */ "your description here",
    /* 13 */ "your description here",
    /* 14 */ "your description here",
    /* 15 */ "your description here",
    /* STRESS tests */
    /* 16 */ "alloc & free, stay within heap limits",
    /* 17 */ "resize stays within heap limit",
    /* 18 */ "your description here",
    /* 19 */ "your description here",
    /* 20 */ "your description here",
    /* 21 */ "your description here",
    /* 22 */ "your description here",
    /* 23 */ "your description here",
};

/* ------------------ COMPLETED SPEC TESTS ------------------------- */

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init
 * SPECIFICATION BEING TESTED:
 * hl_init should successfully complete (without producing a seg
 * fault) for a HEAP_SIZE of 1024 or more.
 *
 * MANIFESTATION OF ERROR:
 * A basic test of hl_init.  If hl_init has an eggregious programming
 * error, this simple call would detect the problem for you by
 * crashing.
 *
 * Note: this shows you how to create a test that should succeed.
 */
int test00() {

    // simulated heap is just a big array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    return SUCCESS;
}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * If there is room in the heap for a request for a block, hl_alloc
 * should sucessfully return a pointer to the requested block.
 *
 * MANIFESTATION OF ERROR:
 * The call to hl_alloc will return NULL if the library cannot find a
 * block for the user (even though the test is set up such that the
 * library should be able to succeed).
 */
int test01() {

    // simulated heap is just a big array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    // if this returns null, test21 returns FAILURE (==0)
    return (hl_alloc(heap, HEAP_SIZE/2) != NULL);
}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * If there is not enough room in the heap for a request for a block,
 * hl_alloc should return NULL.
 *
 * MANIFESTATION OF ERROR:
 * This test is designed to request a block that is definitely too big
 * for the library to find. If hl_alloc returns a pointer, the library is flawed.
 *
 * Notice that heaplame's hl_alloc does NOT return NULL. (This is one
 * of many bugs in heaplame.)
 *
 * Note: this shows you how to create a test that should fail.
 *
 * Surely it would be a good idea to test this SPEC with more than
 * just 1 call to alloc, no? 
 */
int test02() {

    // simulated heap is just an array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    // if this returns NULL, test22 returns SUCCESS (==1)
    return !hl_alloc(heap, HEAP_SIZE*2);

}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * There should be no hard-coded limit to the number of blocks heaplib
 * can support. So if the heap gets larger, so should the number of
 * supported blocks.
 *
 * MANIFESTATION OF ERROR:
 * See how many blocks are supported with heap size N. This number should
 * increase with heap size 2N. Otherwise fail!
 *
 * Note: unless it is fundamentally changed, heaplame will always fail
 * this test. That is fine. The test will have more meaning when run on
 * your heaplib.c
 */
int test03() {

    // now we simulate 2 heaps, once 2x size of the first
    char heap[HEAP_SIZE], heap2[HEAP_SIZE*2];
    int num_blocks = 0;
    int num_blocks2 = 0;

    hl_init(heap, HEAP_SIZE);

    while(true) {
        int *array = hl_alloc(heap, 8);
        if (array)
            num_blocks++;
        else
            break;
    }

    hl_init(heap2, HEAP_SIZE*2);

    while(true) {
        int *array = hl_alloc(heap2, 8);
        if (array)
            num_blocks2++;
        else
            break;
    }
#ifdef PRINT_DEBUG
    printf("%d blocks (n), then %d blocks (2n) allocated\n", num_blocks, num_blocks2);
#endif

    // hoping to return SUCCESS (==1)
    return (num_blocks2 > num_blocks);
}

/* ------------------ YOUR SPEC TESTS ------------------------- */

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: alloc
 * SPECIFICATION BEING TESTED:
 * The function allocates never less than the requested size
 *
 *
 * MANIFESTATION OF ERROR:
 * This test first tries to allocate a fixed size of memory and if the request is satisfied,
 * namely the returned pointer is null, then we try filling it up completely. If a seg fault
 * occurs then we didn't get the size we requested. The function must be flawed.
 *
 */
int test04() {
    int i;
    int j;

    char temp[HEAP_SIZE];
    void *heap = (char *)(temp) + 1;    // makek heap not 8 byte aligned

    hl_init(heap, HEAP_SIZE - 1);

    // request and write char in allocated space
    char *block1 = hl_alloc(heap, HEAP_SIZE/8);
    for (i = 0; i < HEAP_SIZE/8; i ++) {
        block1[i] = 'A';
    }

    // request again (known it will be satisfied), write int in allocated space
    char *block2 = hl_alloc(heap, HEAP_SIZE/8); // assume metadata < HEAP_SIZE/*3/4
    for (j = 0; j < HEAP_SIZE/(8*sizeof(int)); j ++) {
        block2[j] = j;
    }

    // if no seg fault occured meanwhile, return success
    return SUCCESS;

}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: alloc
 * SPECIFICATION BEING TESTED:
 * The returned address should be aligned at multiples of 8 bytes, no matter whether heap is aligned
 * at multiple of 8 byte or not.
 *
 *
 * MANIFESTATION OF ERROR: 
 * This test first creates a heap at an adress that is aligned at multiple of 8 byte, then it request
 * a block and check the returned pointer is aligned at multiple of 8 bytes, if not then the function
 * is flawed. In addition, it creates a heap at an address that is not aligned at multiple of 8 byte
 * and check the same thing.
 *
 */
int test05() {

    // four bool variables that must all be TRUE for correct implementation
    bool result1;
    bool result2;
    bool result3;
    bool result4;

    // 8-byte aligned heap
    char heap1[HEAP_SIZE];
    hl_init(heap1, HEAP_SIZE);

    // request and test twice
    char *block1 = hl_alloc(heap1, HEAP_SIZE/16);
    result1 = ((uintptr_t) block1) % 8 == 0;
    char *block2 = hl_alloc(heap1, HEAP_SIZE/16);
    result2 = ((uintptr_t) block2) % 8 == 0;

    // non 8-byte aligned heap, because added by 3
    char temp[HEAP_SIZE];
    void *heap2 = (char *)(temp) + 3;
    hl_init(heap2, HEAP_SIZE - 3);

    // request and test twice
    char *block3 = hl_alloc(heap2, 1);
    result3 = ((uintptr_t) block3) % 8 == 0;
    char *block4 = hl_alloc(heap2, 1);
    result4 = ((uintptr_t) block4) % 8 == 0;

    return (result1 && result2 && result3 && result4);

}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: alloc
 * SPECIFICATION BEING TESTED:
 * The function returns 0 (NULL) if request cannot be satisfied and allocated blocks should not overlap
 *
 *
 * MANIFESTATION OF ERROR:
 * This test first request a block that doesn't fill all space in the heap, and then request a second block
 * that would not be satisfied to check if the function returns 0 (NULL). If 0 (NULL) is not returned, then
 * the function is flawed. If 0 (NULL) is returned, then we proceed and request a third block that should be
 * satisfied, we then write 'A's on the first block and loop over third block to check none of the space
 * contains 'A'. If some space contains 'A' then it means the two blocks are overlapped and function is flawed.
 *
 */
int test06() {

    int i;
    int j;

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    // request first block
    char *block1 = hl_alloc(heap, HEAP_SIZE*3/16 + 1);

    // check to make sure block2 is null
    if (hl_alloc(heap, HEAP_SIZE*13/16) != NULL) {
        return FAILURE;
    }

    // request third block
    char *block3 = hl_alloc(heap, HEAP_SIZE*1/16 - 1);

    // write data
    for (i = 0; i < HEAP_SIZE*3/16 + 1; i ++) {
        block1[i] = 'A';
    }

    // check for overlapping space
    for (j = 0; j < HEAP_SIZE*1/16 - 1; j ++) {
        if (block3[j] == 'A'){
            return FAILURE;
        }
    }

    return SUCCESS;

}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: release
 * SPECIFICATION BEING TESTED:
 * Releases the block of memory pointed to by block and acts as a NOP if block == 0 (NULL).
 * The release call should not change content of other blocks in memory.
 *
 *
 * MANIFESTATION OF ERROR:
 * This test first allocates two blocks of memory . We write 'A' on one of them and release
 * the other. The program should not crash, otherwise the function is flawed. We check the block still
 * contains the 'A' that were written previously. If not, then the function is flawed since it changes
 * content of other block in memory. Then, we call release using block = NULL. We check the block still
 * contains the 'A' and then write 'B' to check the release with block = NULL acts as a NOP.
 *
 */
int test07() {

    int i;
    int j;
    int h;
    int k;

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    // allocate, write 'A' and release
    char *block1 = hl_alloc(heap, HEAP_SIZE/8);
    char *block2 = hl_alloc(heap, HEAP_SIZE/8);

    for (i = 0; i < HEAP_SIZE/8; i ++) {
        block1[i] = 'A';
    }

    hl_release(heap, block2);


    // check still contains 'A'
    for (j = 0; j < HEAP_SIZE/8; j ++) {
        if (block1[j] != 'A') {
            return FAILURE;
        }
    }

    // release using block = NULL
    hl_release(heap, NULL);


    // check still contains 'A'
    for (h = 0; h < HEAP_SIZE/8; h ++) {
        if (block1[h] != 'A') {
            return FAILURE;
        }
    }

    // try to write 'B'
    for (k = 0; k < HEAP_SIZE/8; k ++) {
        block1[k] = 'B';
    }

    return SUCCESS;

}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: resize
 * SPECIFICATION BEING TESTED:
 * Changes the size of the block pointed to by block from its current size to size new_size
 * bytes, returning a pointer to the new block, or 0 if the request cannot be satisfied.
 *
 * MANIFESTATION OF ERROR:
 * This test allocates a block and does the following basic checks. First, we resize it to a smaller
 * size and if it returns null or crashes , it means the function is flawed. Second, we resize it to a larger
 * size that is supposed to be successful, write data on it and if there's segfault it means the function
 * is flawed. Third, resize it to a larger size that is supposed to be unsuccessful, check if it returns
 * null, if not then again the function is flawed.
 * 
 * 
 */
int test08() {

    int i;

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    char *block = hl_alloc(heap, HEAP_SIZE/8);

    // resize three times to smaller, make sure it doesn't return null or crash
    char *block1 = hl_resize(heap, block, HEAP_SIZE/16);
    if (block1 == NULL) {
        return FAILURE;
    }

    char *block2 = hl_resize(heap, block1, HEAP_SIZE/32);
    if (block2 == NULL) {
        return FAILURE;
    }

    char *block3 = hl_resize(heap, block2, 1);
    if (block3 == NULL) {
        return FAILURE;
    }

    // resize to reasonably larger, make sure it doesn't return null or crash
    char *block4 = hl_resize(heap, block3, HEAP_SIZE/4);
    if (block4 == NULL) {
        return FAILURE;
    }

    // write data, make sure no seg fault
    for (i = 0; i < HEAP_SIZE/4; i ++) {
        block4[i] = 'A';
    }

    // resize to too large, make sure returns null
    char *block5 = hl_resize(heap, block4, HEAP_SIZE);
    if (block5 != NULL) {
        return FAILURE;
    }

    return SUCCESS;

}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 * 
 * FUNCTIONS BEING TESTED: resize
 * SPECIFICATION BEING TESTED:
 * If block has 0 (NULL), the function behaves like hl_alloc. The returned address should be 8-byte
 * algined no matter the heap is 8-byte aligned or not.
 * 
 * 
 * MANIFESTATION OF ERROR:
 * This test first allcoates the memory by calling resize on block == NULL, if it crashes or does
 * not behave normally later then the function is flawed. Then, we check the returned address to
 * make sure it is a multiple of 8. Otherwise, the function is flawed.
 * 
 * 
 */
int test09() {

    bool result1;
    bool result2;
    bool result3;
    bool result4;
    bool result5;
    bool result6;

    // 8 byte aligned heap
    char heap1[HEAP_SIZE];
    hl_init(heap1, HEAP_SIZE);
    char *block1 = hl_resize(heap1, NULL, HEAP_SIZE/16);    // should behave like alloc
    result1 = ((uintptr_t) block1) % 8 == 0;

    char *block2 = hl_resize(heap1, block1, HEAP_SIZE/8 - 1);   // should return 8-byte aligned address
    result2 = ((uintptr_t) block2) % 8 == 0;

    char *block3 = hl_resize(heap1, block2, 1);   // should return 8-byte aligned address
    result3 = ((uintptr_t) block3) % 8 == 0;


    // non 8 byte aligned heap
    char temp[HEAP_SIZE];
    void *heap2 = (char *)(temp) + 3;
    hl_init(heap2, HEAP_SIZE - 3);
    char *block4 = hl_resize(heap2, NULL, HEAP_SIZE/16);    // should behave like alloc
    result4 = ((uintptr_t) block4) % 8 == 0;

    char *block5 = hl_resize(heap2, block4, HEAP_SIZE/8 - 1);   // should return 8-byte aligned address
    result5 = ((uintptr_t) block5) % 8 == 0;

    char *block6 = hl_resize(heap2, block5, 1);  // should return 8-byte aligned address
    result6 = ((uintptr_t) block6) % 8 == 0;

    return (result1 && result2 && result3 && result4 && result5 && result6);

}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: resize
 * SPECIFICATION BEING TESTED:
 * The contents of the block should be preserved (even if the location of the block changes -- this
 * will happen when it is not possible to increase the size of the current block but there is room
 * elsewhere on the heap to satisfy the request).
 *
 *
 * MANIFESTATION OF ERROR:
 * This test allocates a block, writes data on it and resizes it, we check the content is not changed.
 * If it is in fact changed, then the function is flawed.
 *
 */
int test10() {

    int i;
    int j;
    int k;
    int p;

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    char *block1 = hl_alloc(heap, 5);
    char *block2 = hl_alloc(heap, HEAP_SIZE/16);
    char *block3 = hl_alloc(heap, HEAP_SIZE/16);


    // write data
    for (j = 0; j < 5; j ++) {
        block1[j] = 'A';
    }

    for (i = 0; i < HEAP_SIZE/16; i ++) {
        block2[i] = 'B';
        block3[i] = 'C';
    }


    // resize
    char *block4 = hl_resize(heap, block1, HEAP_SIZE/16);
    char *block5 = hl_resize(heap, block2, 1);
    char *block6 = hl_resize(heap, block3, HEAP_SIZE/8);


    // check data is still there
    for (p = 0; p < 5; p ++) {
        if (block4[p] != 'A') {
            return FAILURE;
        }
    }

    if (block5[0] != 'B') {
        return FAILURE;
    }

    for (k = 0; k < HEAP_SIZE/16; k ++) {
        if (block6[k] != 'C') {
            return FAILURE;
        }
    }


    return SUCCESS;

}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test11() {

    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test12() {

    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test13() {

    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test14() {

    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test15() {

    return FAILURE;
}

/* ------------------ STRESS TESTS ------------------------- */

/* THIS TEST IS FINISHED. It is a stress test, but it does not
 * actually test to see whether the library or the user writes 
 * past the heap. You are encouraged to complete this test.
 * 
 * FUNCTIONS BEING TESTED: alloc, free
 * SPECIFICATION BEING TESTED:
 * The library should not give user "permission" to write off the end
 * of the heap. Nor should the library ever write off the end of the heap.
 *
 * MANIFESTATION OF ERROR:
 * If we track the data on each end of the heap, it should never be
 * written to by the library or a good user.
 *
 */
int test16() {
    int n_tries    = 10000;
    //int block_size = 16; 
    int i,j,k,s;
    // 1024 bytes of padding
    // --------------------
    // 1024*8 bytes of "heap"
    // --------------------  <-- heap_start
    // 1024 bytes of "padding"
    char memarea[HEAP_SIZE*10];

    char *heap_start = &memarea[1024]; // heap will start 1024 bytes in
    char *pointers[NPOINTERS];
    int len[NPOINTERS];
    // zero out the pointer array
    memset(pointers, 0, NPOINTERS*sizeof(char *));
    memset(memarea,0,sizeof(memarea));  //clear the heap to 0, if write across, the bytes before the start and after the end of heap will not be 0
    hl_init(heap_start, HEAP_SIZE*8);   // heap is 8*HEAP_SIZE  
    srandom(0);
    for ( i = 0; i < n_tries; i++) {
        int index = random() % NPOINTERS;
        if (pointers[index] == 0) {
            j=(random()%(HEAP_SIZE*20))+1;
            pointers[index] = hl_alloc(heap_start, j );
            if((long)pointers[index]&7)  // check if address is 8 bytes aligned 
            {
            #ifdef PRINT_DEBUG
                  printf("not aligned 8 error.\n");
            #endif                      
                 return FAILURE;
                 
            }
            if(pointers[index]) //fill the block with the given number, to see if we will cross the boundary
                memset(pointers[index],j&0xff,j);
        }
        else{
            hl_release(heap_start, pointers[index]);
            pointers[index] = 0;
        }
    }
    for(i=0;i<NPOINTERS;i++)  //free all the blocks
       if(pointers[i])
       {
             hl_release(heap_start, pointers[i]);
            pointers[i] = 0;        
       }
    
    for(i=0;i<HEAP_SIZE;i++)
    {   
        if(memarea[i])  //check if write before the start of heap
           return FAILURE;
         
        if(memarea[9*HEAP_SIZE+i])  // check if write acrcoss the end of heap
           return FAILURE;     
    }
    
    for(i=0;i<100;i++)
    {
        for(j=0;j<NPOINTERS;j++)
        {
            len[j]=((random()%256)+1)*8;        
            pointers[j]=hl_alloc(heap_start,len[j]); //malloc random length of blocks   
            if(pointers[j]==NULL)
            {
                #ifdef PRINT_DEBUG
                  printf("we stop at j:%d\n",j);
                #endif              
                break;
            }

            memset(pointers[j],j+1,len[j]); //fill the block with fixed number, different blocks are filled with diffrent numbers
        }
        for(k=0;k<j;k++)
        {
            for(s=0;s<len[k];s++)
                if(pointers[k][s]!=k+1)  //check if the filled number have been overwritten 
                    return FAILURE;
                    
        }
 
        for(k=0;k<HEAP_SIZE;k++)
        {   
            if(memarea[k])  //check if write before the start of heap
               return FAILURE;
             
            if(memarea[9*HEAP_SIZE+k]) // check if write across the end of heap
               return FAILURE;     
        }
        
        for(k=0;k<j;k++)  //release all the block 
             hl_release(heap_start, pointers[k]);
        
    }
    
    return SUCCESS;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED: realloc
 * the library should keep data unchanged after realloc
 *
 * MANIFESTATION OF ERROR:
* If we track the data on each end of the heap, it should never be
 * written to by the library or a good user.
 *
 */
int test17() {

    int i,j,k,s,T,w;
    // 1024 bytes of padding
    // --------------------
    // 1024*8 bytes of "heap"
    // --------------------  <-- heap_start
    // 1024 bytes of "padding"
    char memarea[HEAP_SIZE*10];

    char *heap_start = &memarea[1024]; // heap will start 1024 bytes in
    char *pointers[NPOINTERS];
    char *tmp;
    int len[NPOINTERS];
    // zero out the pointer array
    memset(pointers, 0, NPOINTERS*sizeof(char *));
    memset(memarea,0,sizeof(memarea));  //clear the heap to 0, if write across, the bytes before the start and after the end of heap will not be 0
    hl_init(heap_start, HEAP_SIZE*8);
    srandom(0);

    
    for(i=0;i<100;i++)
    {
        for(j=0;j<NPOINTERS;j++)
        {
            len[j]=((random()%32)+1)*8;     // malloc random length blocks
            pointers[j]=hl_alloc(heap_start,len[j]);    
            if(pointers[j]==NULL)
            {
            #ifdef PRINT_DEBUG
              printf("stop at j:%d\n",j);
            #endif
               T=j;    // we have malloc T blocks
               break;
            }
            memset(pointers[j],j+1,len[j]);  // filled with fixed number
        }
 
    for(j=0;j<T;j++)
    {
        w=rand()%32;
        tmp=hl_resize(heap_start,pointers[j],len[j]+w);  //try realloc
        if(tmp) // if success
        {
            pointers[j]=tmp;
                if((long)pointers[j]&7)   // check if the address is 8 bytes aligned 
                {
                   #ifdef PRINT_DEBUG
                     printf("not 8 aligned:%p\n",tmp);
                   #endif                       
                    return FAILURE;
                }
                memset(pointers[j]+len[j],j+1,w);  // fill the new space with the fixed number
                len[j]+=w;  
             }  
    }
    
        for(k=0;k<T;k++)
        {
            for(s=0;s<len[k];s++)
                if(pointers[k][s]!=k+1) // check if the fix number have been overwritten
                {
               #ifdef PRINT_DEBUG
                 printf("error in pointers[k][s]!=k+1...\n");
               #endif            
                
                    return FAILURE;
                }   
        }   
    
        for(k=0;k<HEAP_SIZE;k++)
        {   
            if(memarea[k])  //check if write before the start of heap
            {
               #ifdef PRINT_DEBUG
                 printf("error in memarea[k]...\n");
               #endif            
                
               return FAILURE;
             }
             
            if(memarea[9*HEAP_SIZE+k])  // check if write across the end of heap
             {
           #ifdef PRINT_DEBUG
                 printf("error in memarea[9*HEAP_SIZE+k]...\n");
               #endif                           
               return FAILURE;     
             }
        }
        
        for(k=0;k<T;k++)
             hl_release(heap_start, pointers[k]);
        
    }
    
    return SUCCESS;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test18() {

    return FAILURE;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test19() {

    return FAILURE;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test20() {

    return FAILURE;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test21() {

    return FAILURE;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test22() {

    return FAILURE;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test23() {

    return FAILURE;
}
