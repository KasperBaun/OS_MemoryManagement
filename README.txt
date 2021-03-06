 Memory Management
===================

###################### OUR INFORMATION #############################

developed by:
Emil Johannes Lundqvist (s185131)
Kasper Baun             (s172483)
Rikke Galløe            (s185015)

Work done:
We have implemented the required functions in mymem.c for the memory allocation and freeing to pass all the tests.

The understanding questions for the assignment are answered after each question later in this document.


Results:
Our implementation passes all tests without problems.


#####################################################################

The problem will focus on memory.  You will implement your own 
version of malloc() and free(), using a variety of allocation strategies.

You will be implementing a memory manager for a block of memory.  You will
implement routines for allocating and deallocating memory, and keeping track of
what memory is in use.  You will implement one of the four strategies for selecting in
which block to place a new requested memory block:

  1) First-fit: select the first suitable block with smallest address.
  2) Best-fit: select the smallest suitable block.
  3) Worst-fit: select the largest suitable block.
  4) Next-fit: select the first suitable block after
     the last block allocated (with wraparound
     from end to beginning).


Here, "suitable" means "free, and large enough to fit the new data".

Here are the functions you will need to implement:

initmem():
  Initialize memory structures.

mymalloc():
  Like malloc(), this allocates a new block of memory.

myfree():
  Like free(), this deallocates a block of memory.

mem_holes():
  How many free blocks are in memory?

mem_allocated():
  How much memory is currently allocated?

mem_free():
  How much memory is NOT allocated?

mem_largest_free():
  How large is the largest free block?

mem_small_free():
  How many small unallocated blocks are currently in memory?

mem_is_alloc():
  Is a particular byte allocated or not?

A structure has been provided for use to implement these functions.  It is a
doubly-linked list of blocks in memory (both allocated and free blocks).  Every
malloc and free can create new blocks, or combine existing blocks.  You may
modify this structure, or even use a different one entirely.  However, do not
change function prototypes or files other than mymem.c.

IMPORTANT NOTE: Regardless of how you implement memory management, make sure
that there are no adjacent free blocks.  Any such blocks should be merged into
one large block.

A few functions are given to help you monitor what happens when you
call your functions.  Most important is the try_mymem() function.  If you run
your code with "mem -try <args>", it will call this function, which you can use
to demonstrate the effects of your memory operations.  These functions have no
effect on test code, so use them to your advantage.

Running your code:

After running "make", run

1) "mem" to see the available tests and strategies.
2) "mem -test <test> <strategy>" to test your code with provided tests.
3) "mem -try <args>" to run your code with your own tests
   (the try_mymem function).

You can also use "make test" and "make stage1-test" for testing.  "make
stage1-test" only runs the tests relevant to stage 1.

Running "mem -test -f0 ..." will allow tests to run even
after previous tests have failed.  Similarly, using "all" for a test or strategy
name runs all of the tests or strategies.  Note that if "all" is selected as the
strategy, the 4 tests are shown as one.

One of the tests, "stress", runs an assortment of randomized tests on each
strategy.  The results of the tests are placed in "tests.out" .  You may want to
view this file to see the relative performance of each strategy.


Stage 1
-------

Implement all the above functions, for the strategy you have been assigned in the google doc. 
Here is the link to the google doc : https://docs.google.com/spreadsheets/d/1uvyyiRBBHnfKZNq2iBMCqydAzAGVQtoNdcOpRmf5xlc/edit?usp=sharing
Use "mem -test all first" to test your implementation


Stage 2
-------

You will have to peer review(after you have submitted) the other three strategies out of: 
first-fit
worst-fit
best-fit
next-fit.  
You can from the start work in a group of 4 (remember each member should have seperate strategy)
who have to implement  in from the start and you should answer the 10 questions asked below together.
The strategy is passed to initmem(), and stored in the global variable "myStrategy".
Some of your functions will need to check this variable to implement the
correct strategy.

You can test your code with "mem -test all worst", etc., or test all 4 together
with "mem -test all all".  The latter command does not test the strategies
separately; your code passes the test only if all four strategies pass.


Answer the following questions as part of your report
=====================================================

1) Why is it so important that adjacent free blocks not be left as such?  What
would happen if they were permitted?

Then they would be counted as several smaller free blocks of memory and after a while there
would not be any blocks big enough to hold normally sized processes anymore.
There would just be a lot of smaller, almost useless, fragments of free memory blocks adjacent to each other.
This is known as external fragmentation.


2) Which function(s) need to be concerned about adjacent free blocks?

The function(s) responsible for freeing the allocated memory as it when a block is freed that there
could potentially be free adjacent blocks which should be combined.
(e.g. myfree(...))


3) Name one advantage of each strategy.

FirstFit: Initial fast processing. Allocates to the next available free block of memory, without further checks.

BestFit: Good at fitting processes of specific sizes into exactly same sized free spaces in memory.

WorstFit: Controlled external fragmentation. Allocates to the largest fitting block so the "leftover" block
            would often be big enough to be used for other smaller processes.

NextFit: Continuous fast processing. Allocates to the next free fitting block relative to the last allocated.
            This will, unlike firstFit, prevent a "collection" of allocations in the start of the memory.
            which means that it won't have to repeatedly check a "busy" part of the memory that could occur in firstFit
            with longer running processes.


List of best performing algorithms in different cases (best, worse, worst):
 - Speed: NextFit, FirstFit, (BestFit/WorstFit)
 - Fragmentation: WorstFit, (NextFit, FirstFit), BestFit


End note: If memory compaction is implemented the difference between all the strategies are diminished and
            there would be very little to no difference in experience.


4) Run the stress test on all strategies, and look at the results (tests.out).
What is the significance of "Average largest free block"?  Which strategy
generally has the best performance in this metric?  Why do you think this is?

NextFit generally has the average largest free block. This makes sense because nextFit distributes the
processes more evenly on the available memory, which prevents a precess heavy front of the memory
which could quickly cause a lot fragmentation. This keeps helps prevent smaller free memory blocks from
appearing quite as often as in other strategies.


5) In the stress test results (see Question 4), what is the significance of
"Average number of small blocks"?  Which strategy generally has the best
performance in this metric?  Why do you think this is?

The less small blocks the better the memory is used. This is because bigger blocks are more likely to fit
most of the processes, big and small.
So the smallest average number of small blocks would be the best. This is best performed by worstFit, which intuitively
makes sense, as worstFit will leave bigger "leftover" blocks after most allocations.


6) Eventually, the many mallocs and frees produces many small blocks scattered
across the memory pool.  There may be enough space to allocate a new block, but
not in one place.  It is possible to compact the memory, so all the free blocks
are moved to one large free block.  How would you implement this in the system
you have built?

You could reallocate the processes to fit compactly, leaving more sequential free memory for future processes.
An example of a solution could be that when a block of memory is freed after a process is finished, and an allocated
block of memory is located in the next memory sequence, that following allocated memory is then moved up
to the freed memory's location. This prevents external fragmentation, meaning that the "too small" free memory
blocks between processes are removed by moving memory up to fill the freed memory locations.


7) If you did implement memory compaction, what changes would you need to make
in how such a system is invoked (i.e. from a user's perspective)?

It would be ideal to check to see if the processes are not actively being used (e.g. when minimated).
Then the compaction could be run with little to no annoyance for the user.


8) How would you use the system you have built to implement realloc?  (Brief
explanation; no code)

We could, instead of setting allocated to 0 or 1, set reallocate the freed blocks to move
the free memory to the end block instead. This could be done using realloc method to resize the end block with
the addition of freed block's size when the free blocks are removed.


9) Which function(s) need to know which strategy is being used?  Briefly explain
why this/these and not others.

The function(s) responsible for allocating the processes, as it is there the suitable block is to be identified.
(e.g. mymalloc(...))


10) Give one advantage of implementing memory management using a linked list
over a bit array, where every bit tells whether its corresponding byte is
allocated.

Easier traversal. Instead of having to traverse every bit, just every block can to be traversed with linked list.

There would also need to be additional tracking of when an allocated process
is part of one process and not the next subsequently allocated process.
This is because a single bit doesn't hold enough information for differentiating between multiple processes.

