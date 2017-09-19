/*
#CODE: Circular Buffer Header File 
#LANGUAGE: C
#AUTHOR: Arundhathi Swami
#COURSE/PROJECT: Advanced Practical Embedded Systems
#DATE STARTED: Sept 4 2017
#LAST DATE MODIFIED: Sept 6 2017
*/


#ifndef CIRCULARBUFFER_H_INCLUDED
#define CIRCULARBUFFER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define LENGTH 10 //macro to adjust length of buffer 

/*This structure contains the variables for the buffer*/
typedef struct {
	uint32_t* base;		//pointer to the circular buffer
	uint32_t* head;			//pointer to the head of the buffer
	uint32_t* tail;			//pointer to the tail of the buffer
	uint32_t length;		//Total length of the buffer
	uint32_t no_of_items;		//number of items present in the buffer
}struct_buff;


/*Enumeration for the state of the buffer*/
typedef enum {
   BUFFER_FULL = 0,
   BUFFER_EMPTY = 1,
   BUFFER_NOT_FULL = 2,
   BUFFER_NOT_EMPTY = 3,
   ELEMENT_ADDED = 4,
   ELEMENT_REMOVED = 5,
   FULL_BUFFER_DUMBED = 6,
   ALL_ITEMS_DUMPED = 7,
   SUCCESS = 8,
   FAILED = 9,
   NULL_ENTITY = 10
}states;


/*This function initializes the circular buffer of a particular length*/
states Initialize_Buffer(struct_buff** bufferptr, uint32_t** buffer, uint32_t length);

/*This functions checks of the buffer is full or not*/
states Is_Buffer_Full(struct_buff* bufferptr);

/*This function check if the buffer is empty*/
states Is_Buffer_Empty(struct_buff* bufferptr);

/*This function is used to add an element in the buffer*/
states Add_Element(struct_buff* bufferptr, uint32_t new_data);

/*This function remove an item from the buffer*/
states Remove_Element(struct_buff* bufferptr, uint32_t *removed_element);

/*This function removes all the item from the buffer*/
states Buffer_Flush(struct_buff** bufferptr, uint32_t *removed_value);

/*This function prints the entire buffer*/
states Dump_Full_Buffer(struct_buff* bufferptr);

/*This function returns the size of circular buffer*/
uint32_t Buffer_Size(struct_buff* bufferptr);

/*This function frees the allocated space for buffer*/
states Buffer_Destroy(struct_buff* bufferptr, uint32_t* buffer );


#endif // CIRCULARBUFFER_H_INCLUDED
