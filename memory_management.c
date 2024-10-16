#include "memory_management.h"


//  heap originally set to nothing
//  global variable containing first block of memory in heap
struct segment* heap = 0;


// function to request more memory for the heap
void expand()
{
    // make temporary segment to traverse heap
    struct segment * temp = heap;

    // loop to get last item pointed to
    while(temp->nextSeg != 0)
        temp = temp->nextSeg;

    // system call to request more memory into new segment
    struct segment * new = (struct segment *) sbrk(4096);
    // set values for new segment
    new->length = 4096 - sizeof(struct segment);
    new->free = 1;

    // make last segment point to new free segment
    temp->nextSeg = new;

    // if current last segment is free, coalesce:
    if(temp->free == 1)
    {
        temp->length = temp->length + new->length + sizeof(struct segment);
        temp->nextSeg = 0;
    }
}


// malloc function to allocate memory of size 'size', returns pointer to memory allocated
void* _malloc(int size)
{
    // make sure size is a valid integer over 0
    if(size <= 0)
        return 0;

    // if heap hasnt already been initialised, make it:
    if(heap == 0)
    {   // system call to request memory and set to heap
        heap = (struct segment *) sbrk(4096);
        // set heap values
        heap->length = 4096 - sizeof(struct segment);
        heap->free = 1;
        // end of linked segments points to 0:
        heap->nextSeg = 0;
    }

    // workout size required - size + size of struct header 
    // size of struct is size of 'length' int, 'free' uint8 and 'nextSeg' pointer to another segment.
    int requiredSize = size + (int) sizeof(struct segment);

    // make temporary segment to loop through linked list of segments
    struct segment * current = heap;
    while(current->nextSeg != 0)
    {
        if(current-> free == 1 && current->length>= requiredSize)
            // break out of loop when a free segment, that is greater than or equal to the required size, is found
            break;
        current = current->nextSeg;
    }

    // create new segment to be added
    struct segment * newSeg = 0;

    // check that a big enough free segment was found 
    if(current->length >= requiredSize && current->free == 1)
    {   
        // assign pointer to segment and cast to segment structure
        newSeg = (struct segment *)((void *) current + sizeof(struct segment) + size);
        // set values for new segment - it will be the second, free, half of the segment found
        newSeg->length = current->length - sizeof(struct segment) - size;
        newSeg->free = 1;
        newSeg->nextSeg = current->nextSeg;

        // current is the allocated memory
        // change current free to not free, and current length to the size requested
        current->length = size;
        current->free = 0;
        current->nextSeg = newSeg;
    }
    else 
    {   
        // if no free segment big enough is found, current is the last segment
        // if the last segment is not free, expand it and then set current to the next segment - the new one created in expand
        if(current->free == 0){
            expand();
            current = current->nextSeg;
        }
        // now current is the last free segment
        // keep calling expand until this last free segment is greater or equal to the required size:
        while(current->length < requiredSize)
            expand();
        
        // assign pointer to segment 
        newSeg = (struct segment *)((void *) current + sizeof(struct segment) + size);
        // set values for new segment - it will be the seconf, free half of the current segment
        newSeg->length = current->length - sizeof(struct segment) - size;
        newSeg->free = 1;
        newSeg->nextSeg = current->nextSeg;


        // current is the allocated memory
        // change current free to not free, and current length to the size requested
        current->length = size;
        current->free = 0;
        current->nextSeg = newSeg;
    }
    
    // current is the pointer to the allocated segment struct
    // return current + 1 - the pointer to the allocated memory after the header.
    return current + 1;
}


// function to free memory segment at pointer ptr
void _free(void *ptr)
{
    // ptr is a pointer to allocated memory, ptr - 1 is a pointer to the struct header
    // cast this to a segment
    struct segment * header = (struct segment *) ptr - 1;
    
    // if header == 0, or pointer == 0, this memory can't be free
    // if heap == 0 it means no memory has been allocated, so nothing can be freed
    if(header == 0 || ptr == 0|| heap == 0)
        return;

    // if the pointer is to a valid segment of memory, change segment to free
    header->free = 1;

    // make a temporary segment to traverse the heap
    struct segment * temp = heap;

    // check if first two segments in heap are both free
    if(temp->free == 1 && (temp->nextSeg)->free == 1)
    {
        // coallesce them if they are
        temp->length = temp->length + (temp->nextSeg)->length + sizeof(struct segment);
        temp->nextSeg = (temp->nextSeg)->nextSeg;
    }

    // traverse through rest of the heap until the last segment
    while(temp->nextSeg != 0)
    {
        // check if this segment and next one are both free
        if(temp->free == 1 && (temp->nextSeg)->free == 1)
        {
            // coallesce them if yes
            temp->length = temp->length + (temp->nextSeg)->length + sizeof(struct segment);
            temp->nextSeg = (temp->nextSeg)->nextSeg;
        }
        temp = temp->nextSeg;
    }
}


//  int main(int argc, char *argv[])
//  {
//      int* a = _malloc(100);
//      int* b = _malloc(100);
//      int* c = _malloc(2000);

//      printf("%d\n", a);
//      printf("%d\n", b);
//      printf("%d\n", c);
    
//      _free(a);
//      _free(b);

//      int* d = _malloc(150);
//      printf("%d\n", d);

//      _free(c);
//      int* f = _malloc(1000);
//      printf("%d\n", f);

//      exit(0);
//  }