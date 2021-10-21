#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/** The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */
typedef struct memoryList
{
  // doubly-linked list
  struct memoryList *last;
  struct memoryList *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
} memoryList;
strategies myStrategy = NotSet;    // Current strategy
size_t mySize;
void *myMemory = NULL;
static memoryList *head;
static memoryList *currNode;    //Node for keeping track of the last allocated node for the nextFit algorithm.


/** initmem must be called prior to mymalloc and myfree.
 *
 * @param strategy  : The allocation strategy controlled by the algorithms firstFit, bestFit, worstFit
 * @param sz        : The memory size to be used for allocation atferwards

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/
void initmem(strategies strategy, size_t sz) {
	/* all implementations will need an actual block of memory to use */
	mySize = sz;
	myStrategy = strategy;

    /* Release any memory previously allocated and assigned
    in case this is not the first time initmem is called */
	if(myMemory)
	{ 
		free(myMemory);
	}

	if(head)
	{
		memoryList *trav;
		for (trav = head; trav->next != NULL; trav=trav->next) {    //traverse the list by iteratively updating trav
			if (trav->last)
			{
				free(trav->last);           //free the last node
			}
		}
		free(trav);
	}	

	/*  Assign memory space on heap for myMemory 
		and create new node head and attach*/
	myMemory = malloc(mySize);	
	head = malloc(sizeof(memoryList));
	head->last = NULL;      // Is initially the only node in the list
	head->next = NULL;
	head->size = sz;        // First block size is equal to size_t sz given in initmem()
	head->alloc = 0;        // Not allocated
	head->ptr = myMemory;   // points to the same memory address as the memory pool
	currNode = head;
}


/** Allocate a block in the DLL and update pointers accordingly
 *
 *  Used for when a new block needs to be created when a free block is split into
 *  two smaller blocks when the allocation doesn't completely match the free block size
 *
 * @param suitableBlock : The free block that is large enough, that is to be split
 * @param requested     : The size requirement for the allocation
 */
void allocateBlock(memoryList *suitableBlock, size_t requested){
	memoryList *new = malloc(sizeof(memoryList));   //The leftover free block

	/* Set connection for new and update for suitableBlock 
	and other affected blocks if they exist*/
	new->last = suitableBlock;

 	/* If suitableBlock->next == NULL 
	then new->next will just be set to NULL 
	this will NOT cause a seg-fault */
	new->next = suitableBlock->next;
	if(new->next)
	{
		// Condition is needed because if new->next does not exist
		//and we try to access its variables, we will cause seg-fault

		new->next->last = new;
	}

	suitableBlock->next = new;      //the leftover block is the next block in memory
	
	/* Set size for new and suitableBlock */
	new->size = suitableBlock->size - requested;
	suitableBlock->size = requested;
	new->ptr = suitableBlock->ptr + requested;
	currNode = new;
	new->alloc = 0;					
}

///__Strategies________________________________________________________________________________________________________

/** Find a suitable free block with memory algorithm First-fit
 *
 * @param requested : size of block to be allocated
 * @return          : returns the suitable block if found. Returns NULL if no suitable block is found.
 */
void *findFirst(size_t requested){
	memoryList *trav = head;
	while(trav!=NULL){
		if(trav->size >= requested && trav->alloc == 0){
			// Found a suitable block
			return trav;
		}
		trav = trav->next;	
	}
	return NULL;
}


/** Find a suitable free block with memory algorithm Best-fit
 *
 * @param requested : size of block to be allocated
 * @return          : returns the suitable block if found. Returns NULL if no suitable block is found.
 */
void *findBest(size_t requested){
	memoryList *trav = head;
	memoryList *bestFit = NULL;
	int sizeDiff = __INT32_MAX__;
	while(trav!=NULL){
		if(trav->size>=requested && trav->alloc==0){
			if (sizeDiff > trav->size-requested)
			{
				sizeDiff = trav->size-requested;
				bestFit = trav;
			}			
		}
		trav = trav->next;	
	}
	return bestFit;
}


/** Find a suitable free block with memory algorithm Worst-fit
 *
 * @param requested : size of block to be allocated
 * @return          : returns the suitable block if found. Returns NULL if no suitable block is found.
 */
void *findWorst(size_t requested){
	memoryList *trav = head;
	memoryList *worstFit = NULL;
	int worst = 0;
	while(trav!=NULL){
		if(trav->size>=requested && trav->alloc==0){
			if (trav->size > worst)
			{
				worst = trav->size;
				worstFit = trav;
			}			
		}
		trav = trav->next;	
	}
	return worstFit;
}


/** Find the next suitable block of free memory relative to the last block allocated.
 *
 * @param requested : size of block to be allocated
 * @return          : returns the suitable block if found. Returns NULL if no suitable block is found.
 */
void *findNext(size_t requested) {

	memoryList *trav = currNode;
	while (trav != NULL)
	{
		if (trav->alloc == 0 && trav->size >= requested)
		{
			return trav;
		}
		trav = trav->next;
	}
    return NULL;
}

///_^Strategies^_______________________________________________________________________________________________________


/** Function responsible for freeing the adjacent blocks
 *  Made for seperating logic from allocation for simplicity
 *
 * @param blockToFree   : The block to be freed
 */
void freeAdjacentBlock(memoryList *blockToFree){
	blockToFree->last->size += blockToFree->size;
	
	if(blockToFree->next){                              // if not tail
		blockToFree->last->next = blockToFree->next;    //update pointers to point around note to be freed
		blockToFree->next->last = blockToFree->last;
	}
	else{
		blockToFree->last->next = NULL;                 // The previous block becomes tail if the block to be freed was tail
	}

	if (currNode == blockToFree){
		if(blockToFree->next != NULL){
			currNode = head;
		} else
		    currNode = blockToFree->last;
	}
	free(blockToFree);
}

/** Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */
void *mymalloc(size_t requested) {
	assert((int)myStrategy>0);
	memoryList *suitableBlock=NULL;
	switch (myStrategy)
	  {
	  case NotSet:
	      return NULL;
		  break;
	  case First:
	      suitableBlock = findFirst(requested);
	      break;
	  case Best:
	      suitableBlock = findBest(requested);
	      break;
	  case Worst:
	      suitableBlock = findWorst(requested);
	      break;
	  case Next:
	      suitableBlock = findNext(requested);
		  break;
	  }

	// There was no unallocated block large enough for requested.
	if(suitableBlock == NULL){ 
		//printf("No suitable block found for strategy %s \n",strategy_name(myStrategy)); 
		return NULL;
	}

	// If the block found is larger than requested we need to insert a new adjacent block with the leftover memory space	
	if(suitableBlock->size > requested){	
		allocateBlock(suitableBlock, requested);
	}

	else{
		/* This is only required and set if the size requested == suitableBlock->size */
		if (suitableBlock->next == NULL)
		{
			currNode = head;
		} else 	currNode = suitableBlock;
	}

	// If the block size found is == requested, we dont need to create a new block in our memory so we just set suitableBlock->alloc = 1;
	suitableBlock->alloc = 1;
	return suitableBlock->ptr;	  
}



/** Frees a block of memory previously allocated by mymalloc.
 * The adjacent blocks allocations are checked. The immediate previous or next block will be combined if also free.
 *
 * @param block : The block that is to be freed.
 */
void myfree(void* block)
{
	memoryList *trav = head;

	while (trav!=NULL)          //loops through the blocks
	{
		if (trav->ptr==block)
		{
			break;              //breaks the loop when block is found
		}		
		trav = trav->next;
	}
	/* Deallocate trav */ 
	trav->alloc = 0;

	/* Check for free adjacent block left side */
	if (trav->last && trav->last->alloc == 0){
		if (trav->next == NULL)
		{
			freeAdjacentBlock(trav);
			return;
		}
			
		memoryList *refHolder = trav->last;     //placeholder because of order of operations
		freeAdjacentBlock(trav);                //calling a function to handle freeing memory
		trav = refHolder;                       //updating node
	}

	/* Check for free adjacent block right side */
	if (trav->next && trav->next->alloc == 0){
		freeAdjacentBlock(trav->next);
	}
}


/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{	//TODO: Ask buphjit if this is supposed to calculate contiguos ares of free space? Is this to be used for compacting?
	int counter=0;
	memoryList *trav = head;
	while(trav!=NULL)
	{
		if (trav->alloc==0)
		{
			counter++;
		}
		trav = trav->next;		
	}	
	return counter;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	int counter=0;
	memoryList *trav = head;
	while(trav!=NULL)
	{
		if (trav->alloc==1)
		{
			counter = counter+trav->size;
		}
		trav = trav->next;		
	}	
	return counter;
}

/* Number of non-allocated bytes */
int mem_free()
{
	int counter=0;
	memoryList *trav = head;
	while(trav != NULL)
	{
		if (trav->alloc == 0)
		{
			counter = counter+trav->size;
		}
		trav = trav->next;		
	}	
	return counter;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{	
	int largestBlock = 0;
	memoryList *trav = head;
	while(trav != NULL)
	{
		if(trav->size > largestBlock && trav->alloc == 0){
		largestBlock = trav->size;
		}
		trav = trav->next;
	}
	return largestBlock;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
	int counter=0;
	memoryList *trav = head;
	while(trav!=NULL)
	{
		if (trav->alloc==0 && trav->size<=size)
		{
			counter++;
		}
		trav = trav->next;		
	}	
	return counter;
}      

char mem_is_alloc(void *ptr)
{
	memoryList *trav = head;
	while(trav!=NULL)
	{
		if (trav->ptr==ptr)
		{
			return trav->alloc;
		}
		trav = trav->next;		
	}	
	return 0;
}

void *findNodeByPtr(int *ptr){
	memoryList *trav = head;
	while (trav!=NULL)
	{
		if (trav->ptr == ptr)
		{
			return trav;
		}
		
		trav = trav->next;
	}	
	return NULL;
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy)
{
	switch (strategy)
	{
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 * vvv___________________________________________________________________________________________________________vvv
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{
	memoryList *trav = head;
	int counter = 0;
	printf("### Printing memory-list ###\n");
	while(trav!=NULL)
	{
		printf(" node number = %d\n size = %d\n last = %p\n next = %p\n this.ptr = %p\n", counter,trav->size, trav->last, trav->next, trav->ptr);
		printf(" Is the node allocated: %d\n\n",trav->alloc );
		counter++;
		trav = trav->next;
	}
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

void print_node(memoryList *node){
	memoryList *trav = head;
	int counter = 0;
	while(trav!=NULL)
	{	
		if(trav==node){
		printf("<<Printing node %d>>\n", counter);	
		printf(" node number = %d\n size = %d\n last = %p\n next = %p\n this.ptr = %p\n", counter,trav->size, trav->last, trav->next, trav->ptr);
		printf(" Is the node allocated: %d\n\n",trav->alloc );
		}
		counter++;
		trav = trav->next;
	}
}
/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
    strategies strat;
	void *a, *b, *c, *d, *e;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */

	   initmem(strat,500);
    printf("currNode 0:  %p \n", currNode);
	a = mymalloc(100);
	printf("A's ptr-> : %p \n",a);
	print_node(findNodeByPtr(a));
	

	printf("currNode 1:  %p \n", currNode);
	b = mymalloc(100);
	printf("B's ptr-> : %p \n",b);
	print_node(findNodeByPtr(b));

	printf("currNode 2:  %p \n", currNode);
	c = mymalloc(100);
	printf("C's ptr-> : %p \n",c);
	print_node(findNodeByPtr(c));

	printf("currNode 3:  %p \n", currNode);
	d = mymalloc(100);
	printf("D's ptr-> : %p \n",d);
	print_node(findNodeByPtr(d));
	
    myfree(b);
	myfree(d);
	print_memory();
	d = mymalloc(100);
	printf("currNode 4:  %p \n", currNode);
	printf("E's ptr-> : %p \n",d);
	print_node(findNodeByPtr(d));
}
