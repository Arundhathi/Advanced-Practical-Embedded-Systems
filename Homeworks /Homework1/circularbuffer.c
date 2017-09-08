/*
#CODE: Circular Buffer Source Code 
#LANGUAGE: C
#AUTHOR: Arundhathi Swami
#COURSE/PROJECT: Advanced Practical Embedded Systems
#DATE STARTED: Sept 4 2017
#LAST DATE MODIFIED: Sept 6 2017
#*/

#include <stdio.h>
#include <stdint.h>
#include "circularbuffer.h"


/*Input Parameters: struct_buff** bufferptr- struct type double pointer to the buffer, uint32_t** buffer - double pointerto buffer, uint32_t length - required length of buffer 
  Return: states - ref enum of return error codes 
  Function Description: Dynamically allots space in heap to both structure and the buffer array
*/
states Initialize_Buffer(struct_buff** bufferptr, uint32_t** buffer, uint32_t length)
{

   struct_buff* current = *bufferptr;

   *buffer = (uint32_t*)malloc(length*(sizeof(uint32_t))); //allots space to buffer array 
   current = (struct_buff*)malloc(1*(sizeof(struct_buff))); //allots space to struct pointer that pounts to buffer 

   /*corner case: length is zero or ptrs are null*/
   if (length == 0 || current == NULL || *buffer == NULL)
   {
		return FAILED;
   }

   current->base = *buffer;     //initializes values to members of struct poiunter pointing to buffer 
   current->head = current->base;
   current->tail = current->base;
   current->no_of_items = 0;
   current->length = length;

   *bufferptr = current; //allocates buffer pointer to head of buffer 

   return SUCCESS;
}


/*Input Parameters: struct_buff* bufferptr - structbuff type pointer to buffer 
  Return:  states - ref enum of return error codes 
  Function Description: Checks if number of items present in the buffer is equal to its length 
*/
states Is_Buffer_Full(struct_buff* bufferptr)
{

  /*corner case: NUll pointer passed or length assigned to buffer is 0*/
	if ( bufferptr == NULL || bufferptr->length == 0)
	{
		return NULL_ENTITY;
   }
   else if ( bufferptr->no_of_items == bufferptr->length) //if number of items in buffer is the same as length then the buffer is full
	{
		return BUFFER_FULL;
	}
	else
	{
		return BUFFER_NOT_FULL;
	}
}


/*Input Parameters: struct_buff* bufferptr - structbuff type pointer to buffer 
  Return:  states - ref enum of return error codes 
  Function Description: Checks if number of items present in the buffer is equal to 0 
*/
states Is_Buffer_Empty(struct_buff* bufferptr)
{
	 /*corner case: NUll pointer passed or length assigned to buffer is 0*/
	if ( bufferptr == NULL || bufferptr->length == 0)
	{
		return NULL_ENTITY;
   }
   else if ( bufferptr->no_of_items == 0)  //if number of items in the buffer is 0 the buffer is empty  
	{
		return BUFFER_EMPTY;
	}
	else
	{
		return BUFFER_NOT_EMPTY;
	}
}


/*Input Parameters: struct_buff* bufferptr - structbuff type pointer to buffer, uint32_t -new_data element to be added
  Return: states - ref enum of return error codes 
  Function Description: Adds an element within confines of the buffers size 
*/
states Add_Element(struct_buff* bufferptr, uint32_t new_data)
{

    /*corner case: NUll pointer passed or length assigned to buffer is 0*/
	if ( bufferptr == NULL || bufferptr->length == 0)
	{
		printf("\nhi");
		return NULL_ENTITY;
    }
	/*corner case: if the buffer is full, no more elements can be added*/
	else if ( Is_Buffer_Full(bufferptr) == BUFFER_FULL )
	{
		return BUFFER_FULL;
	}
	else
	{
		*(bufferptr->head) = new_data; //adds new element at head of the buffer 
		bufferptr->head += 1; //moves head to next position
		bufferptr->no_of_items += 1; //increments no of items 
		if (bufferptr->head == bufferptr->base + bufferptr->length) //wraparound
		{
			bufferptr->head = bufferptr->base; //if head is at the last element then it progresses to the base / 0th elemnt of buffer array
		}
		return ELEMENT_ADDED;
	}
}


/*Input Parameters: struct_buff* bufferptr - structbuff type pointer to buffer, uint32_t* removed_element - pointer to store removed elements value 
  Return: states - ref enum of return error codes 
  Function Description: serially removes a single element from the buffer at a time  
*/
states Remove_Element(struct_buff* bufferptr, uint32_t* removed_element)
{
	/*corner case: NUll pointer passed or length assigned to buffer is 0*/
	if ( bufferptr == NULL || bufferptr->length == 0)
	{
		return NULL_ENTITY;
   }
	/*corner case: if the buffer is empty, no more elements can be removed*/
	else if ( Is_Buffer_Empty(bufferptr) == BUFFER_EMPTY )
	{
		return BUFFER_EMPTY;
	}
	else
	{
		*removed_element = *(bufferptr->tail); //passes value at buffers tail to the removed element parameter 
		bufferptr->tail++; //increments tail to point at new element 
		bufferptr->no_of_items--; //decrements no of items in buffer
		if (bufferptr->tail == bufferptr->base + bufferptr->length) //wraparound
		{
			bufferptr->tail = bufferptr->base;//if head is at the last element then it progresses to the base / 0th elemnt of buffer array
		}
		return ELEMENT_REMOVED;
	}
}


/*Input Parameters: struct_buff** bufferptr - structbuff type double pointer to buffer, uint32_t* removed_value - pointer to store removed elements value 
  Return: states - ref enum of return error codes 
  Function Description: removes all elements from the buffer at one go and stores value of last removed elemnt in parameter pointer to removed value 
*/
states Buffer_Flush(struct_buff** bufferptr, uint32_t *removed_value)
{

	struct_buff* current = *bufferptr; 	
	/*corner case: NUll pointer passed or length assigned to buffer is 0*/
	if ( bufferptr == NULL || current->length == 0)
	{
		return NULL_ENTITY;
   }
	/*corner case: if the buffer is full, no more elements can be added*/
	else if ( Is_Buffer_Empty(current) == BUFFER_EMPTY )
	{
		return BUFFER_EMPTY;
	}
	else 
	{
		states ret; 
		while (current->no_of_items >0 )
		{
			ret = Remove_Element (current, removed_value);  //calls function removed element to serially removed elements 
		}
	}
}
			


/*Input Parameters: struct_buff* bufferptr - structbuff type pointer to buffer
  Return: states - ref enum of return error codes 
  Function Description: Prints out full buffer 
*/
states Dump_Full_Buffer(struct_buff* bufferptr)
{
    /*corner case: NUll pointer passed or length assigned to buffer is 0*/
	if ( bufferptr == NULL || bufferptr->length == 0)
	{
		return NULL_ENTITY;
    }
    uint32_t i;
	uint32_t* current_position = bufferptr->tail; //points to tail end of the buffer 
	for (i = 0; i<bufferptr->no_of_items; i++) //iterates over number of elements in buffer 
	{
		printf("\nElement: %d ", *current_position);
		current_position++;
		if (current_position == bufferptr->base + bufferptr->length) //wraparound 
		{
			current_position = bufferptr->base;  //if head is at the last element then it progresses to the base / 0th elemnt of buffer array
		}
	}
	return SUCCESS;
}



/*Input Parameters: struct_buff* bufferptr - structbuff type pointer to buffer
  Return: size_t - number of elements currently present in buffer  
  Function Description: Returns the number of elemnts present in the buffer effectively showing size of buffer 
*/
size_t Buffer_Size(struct_buff* bufferptr)
{
	if ( bufferptr == NULL || bufferptr->length == 0)
	{
		return NULL_ENTITY;
    }
	return bufferptr->no_of_items; //returns number of items in buffer 
}



/*Input Parameters: struct_buff* bufferptr - structbuff type pointer to buffer , uint32_t* buffer - pointer to buffer array
  Return: states - ref enum of return error codes 
  Function Description: Frees memory allocated to structure and buffer 
*/
states Buffer_Destroy(struct_buff* bufferptr, uint32_t* buffer )
{

	free(bufferptr); //frees space allocated to struct type pointer pointing to buffer 
	free(buffer); //frees space allotted to buffer array 

	return SUCCESS;
}
