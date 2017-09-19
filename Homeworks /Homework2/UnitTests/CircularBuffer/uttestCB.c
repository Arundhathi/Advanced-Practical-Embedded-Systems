#include <math.h>
#include <stdlib.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "circularbuffer.h"

/*Test to Check for Invalid Buffer Length in Initialise Buffer Function*/
void Initialise_Buffer_Test_for_Length_Not_Positive(void **state)

{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = 0; 
	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);
	assert_int_equal(return_status, FAILED);
}

/*Test to check Apppropriate Allocation of Buffer with valid Length*/
void Initialise_Buffer_Test_for_Length_Positive(void **state)

{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = LENGTH; 
 	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);
	assert_int_equal(return_status, SUCCESS);
}

/*test to check Is_Buffer_Full Function with null pointer passed as an invalid paramter*/
void Is_Buffer_Full_Test_Null_BufferPtr(void **state)

{
	struct_buff* ptr_to_buffer = NULL ; 
	
	states return_status = Is_Buffer_Full(ptr_to_buffer); 
	assert_int_equal(return_status, NULL_ENTITY);

}

/*test to check Is_Buffer_Full Function returns the right value for a buffer that is not full*/
void Is_Buffer_Full_Test_Not_Full(void **state)

{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = LENGTH; 
 	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);
	return_status = Is_Buffer_Full(ptr_to_buffer); 
	assert_int_equal(return_status, BUFFER_NOT_FULL);

}

/*test to check Is_Buffer_Full Function returns the right value for a buffer that is full*/
void Is_Buffer_Full_Test_Full(void **state)

{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = LENGTH; 
	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);

	return_status = Add_Element(ptr_to_buffer, 10);
	return_status = Add_Element(ptr_to_buffer, 20);
	return_status = Add_Element(ptr_to_buffer, 30);
	return_status = Add_Element(ptr_to_buffer, 40);
	return_status = Add_Element(ptr_to_buffer, 50);
	return_status = Add_Element(ptr_to_buffer, 20);
	return_status = Add_Element(ptr_to_buffer, 30);
	return_status = Add_Element(ptr_to_buffer, 40);
	return_status = Add_Element(ptr_to_buffer, 46);
	return_status = Add_Element(ptr_to_buffer, 46);
 	
	return_status = Is_Buffer_Full(ptr_to_buffer); 
	assert_int_equal(return_status, BUFFER_FULL);

}

/*test to check Is_Buffer_Empty Function with null pointer passed as an invalid paramter*/
void Is_Buffer_Empty_Test_Null_BufferPtr(void **state)

{
	struct_buff* ptr_to_buffer = NULL ; 
	
	states return_status = Is_Buffer_Empty(ptr_to_buffer); 
	assert_int_equal(return_status, NULL_ENTITY);

}

/*test to check Is_Buffer_Empty Function returns the right value for a buffer that is Empty*/
void Is_Buffer_Empty_Test_Empty(void **state)

{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = LENGTH; 
 	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);
	return_status = Is_Buffer_Empty(ptr_to_buffer); 
	assert_int_equal(return_status, BUFFER_EMPTY);

}

/*test to check Is_Buffer_Empty Function returns the right value for a buffer that is Empty*/
void Is_Buffer_Empty_Test_Not_Empty(void **state)

{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = LENGTH; 
	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);

	return_status = Add_Element(ptr_to_buffer, 10);
	return_status = Add_Element(ptr_to_buffer, 20);
	return_status = Add_Element(ptr_to_buffer, 30);
	return_status = Add_Element(ptr_to_buffer, 40);
	return_status = Add_Element(ptr_to_buffer, 50);
	
 	
	return_status = Is_Buffer_Empty(ptr_to_buffer); 
	assert_int_equal(return_status, BUFFER_NOT_EMPTY);

}

/*test to check Add_Element Function with null pointer passed as an invalid paramter*/
void Add_Element_Null_BufferPtr(void **state)

{
	struct_buff* ptr_to_buffer = NULL ; 
	
	
	states return_status = Add_Element(ptr_to_buffer, 100); 
	assert_int_equal(return_status, NULL_ENTITY);

}

/*test to check Add_Element Function returns the right value when asked to add an element to an already full buffer*/
void Add_Element_Buffer_Full(void **state)

{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = LENGTH; 
	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);

	return_status = Add_Element(ptr_to_buffer, 10);
	return_status = Add_Element(ptr_to_buffer, 20);
	return_status = Add_Element(ptr_to_buffer, 30);
	return_status = Add_Element(ptr_to_buffer, 40);
	return_status = Add_Element(ptr_to_buffer, 50);
	return_status = Add_Element(ptr_to_buffer, 60);
	return_status = Add_Element(ptr_to_buffer, 70);
	return_status = Add_Element(ptr_to_buffer, 80);
	return_status = Add_Element(ptr_to_buffer, 90);
	return_status = Add_Element(ptr_to_buffer, 100);
	return_status = Add_Element(ptr_to_buffer, 1100);
 	
	return_status = Add_Element(ptr_to_buffer, 1200);
	assert_int_equal(return_status, BUFFER_FULL);

}

/*test to check Add_Element Function returns the right value when asked to add an element to an avalilable buffer*/
void Add_Element_Buffer_Success(void **state)

{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = LENGTH; 
	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);

	return_status = Add_Element(ptr_to_buffer, 10);
	return_status = Add_Element(ptr_to_buffer, 20);
	return_status = Add_Element(ptr_to_buffer, 30);
	return_status = Add_Element(ptr_to_buffer, 40);
	return_status = Add_Element(ptr_to_buffer, 50);
	
 	
	return_status = Add_Element(ptr_to_buffer, 60);
	assert_int_equal(return_status, ELEMENT_ADDED);

}

/*test to check Remove_Element Function with null pointer passed as an invalid paramter*/
void Remove_Element_Null_BufferPtr(void **state)
{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t removed_element; 
	
	
	states return_status = Remove_Element(ptr_to_buffer, &removed_element); 
	assert_int_equal(return_status, NULL_ENTITY);

}

/*test to check Remove_Element Function returns the right value when asked to remove an element from an already empty buffer*/
void Remove_Element_Buffer_Empty(void **state)
{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = LENGTH;
	uint32_t removed_element; 
	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);
	return_status = Remove_Element(ptr_to_buffer, &removed_element);
	assert_int_equal(return_status, BUFFER_EMPTY);

}

/*test to check Remove_Element Function returns the right value when asked to remove an element from an avalilable buffer*/
void Remove_Element_Buffer_Success(void **state)
{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = LENGTH; 
	uint32_t removed_element;
	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);

	return_status = Add_Element(ptr_to_buffer, 10);
	return_status = Add_Element(ptr_to_buffer, 20);
	return_status = Add_Element(ptr_to_buffer, 30);
	return_status = Add_Element(ptr_to_buffer, 40);
	return_status = Add_Element(ptr_to_buffer, 50);
	
 	
	return_status = Remove_Element(ptr_to_buffer,&removed_element);
	assert_int_equal(return_status, ELEMENT_REMOVED);

}

/*test to check Dump_Full_Buffer Function with null pointer passed as an invalid paramter*/
void Dump_Full_Buffer_Null_BufferPtr(void **state)

{
	struct_buff* ptr_to_buffer = NULL ; 
		
	states return_status = Dump_Full_Buffer(ptr_to_buffer); 
	assert_int_equal(return_status, NULL_ENTITY);

}

/*test to check Dump_Full_Buffer Function with non-null pointer passed as an valid paramter*/
void Dump_Full_Buffer_Success(void **state)
{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = LENGTH; 
	
	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);

	return_status = Add_Element(ptr_to_buffer, 10);
	return_status = Add_Element(ptr_to_buffer, 20);
	return_status = Add_Element(ptr_to_buffer, 30);
	return_status = Add_Element(ptr_to_buffer, 40);
	return_status = Add_Element(ptr_to_buffer, 50);
	
 	
	return_status = Dump_Full_Buffer(ptr_to_buffer);
	assert_int_equal(return_status, SUCCESS);
	
}
	
/*test to check Dump_Full_Buffer Function with buffer of 0 length i.e. no added elements passed */
void Dump_Full_Buffer_Failure(void **state)
{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = LENGTH; 
	
	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);

		
	return_status = Dump_Full_Buffer(ptr_to_buffer);
	assert_int_equal(return_status, FAILED);
	
}

/*test to check Buffer_Size Function with null pointer passed as an invalid paramter*/
void Buffer_Size_Null_BufferPtr(void **state)

{
	struct_buff* ptr_to_buffer = NULL ; 
		
	states return_status = Buffer_Size(ptr_to_buffer); 
	assert_int_equal(return_status, NULL_ENTITY);

}

/*test to check Buffer_Size Function with valid paramter*/
void Buffer_Size_Success(void **state)
{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = LENGTH; 
	uint32_t buffer_size = 0; 
	
	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);

	return_status = Add_Element(ptr_to_buffer, 10);
	return_status = Add_Element(ptr_to_buffer, 20);
	return_status = Add_Element(ptr_to_buffer, 30);
	return_status = Add_Element(ptr_to_buffer, 40);
	return_status = Add_Element(ptr_to_buffer, 50);
	
 	printf("%d", return_status);
	buffer_size = Buffer_Size(ptr_to_buffer);
	assert_int_equal(buffer_size, 5);
	
}
	

/*test to check Buffer_Destroy Function with valid paramters*/
void Buffer_Destroy_Success(void **state)
{
	struct_buff* ptr_to_buffer = NULL ; 
	uint32_t* buffer = NULL ; 
	uint32_t length_of_buffer = LENGTH; 
	
	states return_status = Initialize_Buffer(&ptr_to_buffer, &buffer, length_of_buffer);

	
	
 	return_status = Buffer_Destroy(ptr_to_buffer, buffer); //calls the resepctive function to obtain return value 
	assert_int_equal(return_status, SUCCESS); //cmocka function to compare return types(only compares integral return types
	
}




int main(int argc, char **argv)
{
  const struct CMUnitTest tests[] = {
   cmocka_unit_test(Initialise_Buffer_Test_for_Length_Not_Positive),
	cmocka_unit_test(Initialise_Buffer_Test_for_Length_Positive),
	cmocka_unit_test(Is_Buffer_Full_Test_Null_BufferPtr),
	cmocka_unit_test(Is_Buffer_Full_Test_Not_Full),  
	cmocka_unit_test(Is_Buffer_Full_Test_Full), 
	cmocka_unit_test(Is_Buffer_Empty_Test_Null_BufferPtr),
	cmocka_unit_test(Is_Buffer_Empty_Test_Not_Empty),  
	cmocka_unit_test(Is_Buffer_Empty_Test_Empty),  
	cmocka_unit_test(Add_Element_Null_BufferPtr), 
	cmocka_unit_test(Add_Element_Buffer_Full), 
	cmocka_unit_test(Add_Element_Buffer_Success), 
	cmocka_unit_test(Remove_Element_Null_BufferPtr), 
	cmocka_unit_test(Remove_Element_Buffer_Empty), 
	cmocka_unit_test(Remove_Element_Buffer_Success), 
	cmocka_unit_test(Dump_Full_Buffer_Success),
	cmocka_unit_test(Dump_Full_Buffer_Failure),
	cmocka_unit_test(Dump_Full_Buffer_Null_BufferPtr),
	cmocka_unit_test(Buffer_Size_Null_BufferPtr),
	cmocka_unit_test(Buffer_Size_Success),
	cmocka_unit_test(Buffer_Destroy_Success), //defines members of struture CMUnitTest with different unit tests initialised above 
 	};

  return cmocka_run_group_tests(tests, NULL, NULL); //runs group of tests 
}
