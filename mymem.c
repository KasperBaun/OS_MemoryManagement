#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/* The main structure for implementing memory allocation.
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


/* initmem must be called prior to mymalloc and myfree.

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

void initmem(strategies strategy, size_t sz)
{
	/* all implementations will need an actual block of memory to use */
	mySize = sz;
	myStrategy = strategy;

	/* 
	Release any memory previously allocated and assigned 
	in case this is not the first time initmem is called */
	if(myMemory!=NULL) free(myMemory);
	if(head!=NULL){
		memoryList *trav;
		for(trav=head; trav->next!=NULL; trav=trav->next){
			if (trav->last != NULL)
			{
				free(trav->last);
			}
			
		}	
		free(trav);
	}
	


	/*  Assign memory space on heap for myMemory 
		and create new node head and attach*/
	myMemory = malloc(mySize);	
	head = (memoryList*) malloc(sizeof (memoryList));
	head->last = NULL; 
	head->next = NULL;
	head->size = sz; // First block size is equal to size_t sz given in initmem()
	head->alloc = 0;  // Not allocated
	head->ptr = myMemory;  // points to the same memory adress as the memory pool
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested)
{
	assert((int)myStrategy > 0);
	memoryList *trav = head;
	switch (myStrategy)
	  {
	  case NotSet: 
	            return NULL;
	  case First:
		while(trav!=NULL){
			if (trav->alloc==0 && trav->size >= requested)
			{
				//In this scope, we have found a block not allocated with enough size

				// Checking if trav(current block)==head
				if (trav->last == NULL && trav->size > requested)
				{	// Create new node with size = requested and append the remaining free memory to this block
					memoryList *new = malloc(sizeof(memoryList));
					new->size = trav->size - requested;
					new->alloc = 0;
					new->last = trav;
					if(trav->next==NULL){
						new->next=NULL;
					} else	new->next = trav->next;
					new->ptr = trav->ptr+requested;

					trav->alloc = 1;
					trav->size = requested;
					trav->next = new;
					return trav->ptr;

					break;
				}
				// Checking if trav(current block)==tail
				if(trav->next == NULL){
					if(trav->size == requested){
						trav->alloc = 1;
						return trav->ptr;
						break;
					}
					
					// Create new node with size = requested and append it to tail->last and update pointers to tail
					memoryList *new = malloc(sizeof(memoryList));
					new->size = trav->size-requested;
					new->alloc = 0;
					new->next = NULL;
					new->ptr = trav->ptr+requested;
					new->last = trav;

					trav->alloc = 1;
					trav->size = requested;
					trav->next = new;
					return trav->ptr;
					break;
				}

				// We found a free block in the middle of list - update ptr's accordingly
				if(trav->next != NULL && trav->last != NULL){
					
					// If the block is a perfect fit - we cant create a new block with free space next to it
					if(trav->size == requested){
					trav->alloc = 1;
					return trav->ptr;
					break;
					} 
										
					// Trav->size is larger than requested (see the if() loop @ line 91) so we need to create a new node with the remaining free space
					memoryList *new = malloc(sizeof(memoryList));
					new->size = trav->size-requested;
					new->alloc = 0;
					new->ptr = trav->ptr+requested;
					new->next = trav->next;
					new->last = trav;

					trav->alloc = 1;
					trav->size = requested;
					trav->next = new;
					return trav->ptr;						
				}				
			}
		trav = trav->next;
		}
	            return NULL;
	  case Best:
	            return NULL;
	  case Worst:
	            return NULL;
	  case Next:
	            return NULL;
	  }
	return NULL;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
	memoryList *trav = head;
	while(trav != NULL){
		// We found the block in our memoryList
		if(trav->ptr == block){
			
			// Checking for free adjacent block
			
			// Free adjacent blocks on both sides of trav
			if(trav->last != NULL && trav->last->alloc == 0 && trav->next != NULL && trav->next->alloc == 0){
				trav->last->alloc = 0;
				trav->last->size = trav->last->size+trav->size+trav->next->size;
				trav->last->next = trav->next->next;
				trav->next->next->last = trav->last;
				memoryList *temp = trav;
				trav = trav->last;
				free(temp->next);
				free(temp);
				break;
			}
			
			
			if(trav->last != NULL && trav->last->alloc == 0){
				//Block left of trav is not allocated - update size first then pointers, then free trav
				memoryList *temp = trav;
				trav = trav->last;
				trav->size = trav->size+trav->next->size;
				trav->next = trav->next->next;
				trav->next->next->last = trav;
				free(temp);

			
			}
			if(trav->next != NULL && trav->next->alloc == 0){
				//Block right of trav is not allocated
				trav->alloc = 0;
				trav->size = trav->size + trav->next->size;
				memoryList *temp = trav->next;
				if(trav->next->next!=NULL){
					trav->next->next->last = trav;
				}
				trav->next = trav->next->next;
				//println("Line 193 - Trav->next: %p \n",trav->next);
				free(temp);
			}
			
			//No free adjacent blocks
			trav->alloc = 0;
		}
		trav = trav->next;
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
	return;
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
	/*initmem(strat,500);
	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	d = mymalloc(101);
	e = mymalloc(99);
	//myfree(e);*/
	/*
	initmem(strat,100);
	a = mymalloc(10);
	b = mymalloc(1);
	myfree(b);
	c = mymalloc(1);
	print_memory();
	print_memory_status();

	initmem(strat,500);
	d = mymalloc(100);
	e = mymalloc(50);
	myfree(d);
	print_memory();
	myfree(e);
	a = mymalloc(150);
	b = mymalloc(150);
	print_memory();
	initmem(strat, 10000);
	print_memory();*/
	initmem(strat,500);
    a = mymalloc(30);
    b = mymalloc(50);
    myfree(a);
    c = mymalloc(120);
    free(c);
    d = mymalloc(100);
    free(b);
    e = mymalloc(20);
	print_memory();
}
