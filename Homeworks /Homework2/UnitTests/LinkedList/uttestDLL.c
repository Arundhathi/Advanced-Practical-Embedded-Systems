#include <math.h>
#include <stdlib.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "doublelinkedlist.h"


/*Test to Check if Add_Node works if you need to add the first node i.e. begin the linked list*/
void Add_First_Node(void **state)

{
	node* headref = NULL;	
	status ret;
 	ret = add_node(&headref, 0, 0); 
	assert_int_equal(ret, node_added);
}

/*Test to Check if Add_Node throws an error if a node higher that current length of the list is added*/
void Add_Node_Greater_Than_Length(void **state)
{
	node* headref = NULL;	
	status ret;
 	ret = add_node(&headref, 5, 5); //initialises the list 
	ret = add_node(&headref, 5, 5); 
	assert_int_equal(ret, out_of_range);
}

/*Test to Check if Add_Node works if you need to add a node at the top of the list i.e. 0th node */
void Add_Node_At_Top_Of_List(void **state)
{
	node* headref = NULL;	
	status ret;
 	ret = add_node(&headref, 5, 5); 
	ret = add_node(&headref, 5, 0); 
	assert_int_equal(ret, node_added);
}

/*Test to Check if Add_Node works if you need to add a node at the end of the list i.e. at length*/
void Add_Node_At_Bottom_Of_List(void **state)
{
	node* headref = NULL;	
	status ret;
 	ret = add_node(&headref, 5, 5); 
	ret = add_node(&headref, 5, 0); 
	ret = add_node(&headref, 5, 1); 
	ret = add_node(&headref, 5, 2); 
	ret = add_node(&headref, 5, 4); 
	assert_int_equal(ret, node_added);
}

/*Test to check if Remove_Node works when you pass an invalid pointer to removed nodes from */
void Remove_Node_Null_Nodes(void **state)

{
	node* headref = NULL;	
	status ret;
 	uint32_t data_removed = 0;
	ret = remove_node(&headref, 0, &data_removed); 
	assert_int_equal(ret, out_of_range);
}

/*Test to Check if Remove_Node throws an error if a node higher that current length of the list is removed*/
void Remove_Node_Greater_Than_Length(void **state)

{
	node* headref = NULL;	
	status ret;
 	uint32_t data_removed = 0;
	ret = add_node(&headref, 5, 5); 
	ret = add_node(&headref, 5, 0); 
	ret = add_node(&headref, 5, 1); 
	ret = add_node(&headref, 5, 2); 
	ret = add_node(&headref, 5, 4); 
	ret = remove_node(&headref, 6, &data_removed); 
	assert_int_equal(ret, out_of_range);
}

/*Test to Check if Remove_Node works if you need to remove a node at the top of the list i.e. 0th node */
void Remove_Node_At_Top_of_List(void **state)

{
	node* headref = NULL;	
	status ret;
 	uint32_t data_removed = 0;
	ret = add_node(&headref, 5, 5); 
	ret = add_node(&headref, 5, 0); 
	 
	ret = remove_node(&headref, 0, &data_removed); 
	assert_int_equal(ret, node_removed);
}

/*Test to Check if Remove_Node works if you need to remove a node at the end of the list i.e. at length*/
void Remove_Node_At_Bottom_of_List(void **state)

{
	node* headref = NULL;	
	status ret;
 	uint32_t data_removed = 0;
	ret = add_node(&headref, 5, 5); 
	ret = add_node(&headref, 5, 0); 
	ret = add_node(&headref, 5, 1); 
	ret = add_node(&headref, 5, 2); 
	ret = add_node(&headref, 5, 4); 
	ret = remove_node(&headref, 5, &data_removed); 
	assert_int_equal(ret, node_removed);
}

/*Test to see if Size Function returns the right size of the list*/
void Right_Size_of_List(void **state)

{
	node* headref = NULL;	
	status ret;
	uint32_t size_of_list; 
 	uint32_t data_removed = 0;
	ret = add_node(&headref, 5, 5); 
	ret = add_node(&headref, 5, 0); 
	ret = add_node(&headref, 5, 1); 
	ret = add_node(&headref, 5, 2); 
	ret = add_node(&headref, 5, 4); 
	ret = remove_node(&headref, 5, &data_removed); 
	printf("%d", ret);
	size_of_list = size(headref);
	assert_int_equal(size_of_list, 4);
}

/*Test to see if Size() function returns the right errorwhen its asked to find size of a list that is NULL initialised*/
void Size_of_List_with_Null_Nodes(void **state)

{
	node* headref = NULL;	
	
	size_t size_of_list; 
 	size_of_list = size(headref); 
	assert_int_equal(size_of_list, 0);
}

/*Test to see if Destroy Function throws the right error when asked to destroy a non existent list - Corner case*/
void Destroy_List_with_Null_Nodes(void **state)

{
	node* headref = NULL;	
	status ret;
	
 	ret = destroy(&headref); 
	assert_int_equal(ret, success);
}

/*Test to see if Destroy Function works with curroct successful return type*/
void Destroy_List(void **state)
{
	node* headref = NULL;	
	status ret;
	ret = add_node(&headref, 5, 5); 
	ret = add_node(&headref, 5, 0); 
	ret = add_node(&headref, 5, 1); 
	ret = add_node(&headref, 5, 2); 
	ret = destroy(&headref); 
	assert_int_equal(ret, success);
}

/*Test to see if  Print_list function works when list with 0 length is passed*/ 
void print_list_length_zero(void **state)
{
	node* headref = NULL; 
	status ret; 
	ret = print_list(&headref); 
	assert_int_equal(ret, list_empty); 
}

/*Test to see if  Print_list function works */ 
void print_list_length(void **state)
{
	node* headref = NULL; 
	status ret; 
	ret = add_node(&headref, 5, 5); 
	ret = add_node(&headref, 5, 0); 
	ret = add_node(&headref, 5, 1); 
	ret = add_node(&headref, 5, 2); 
	ret = print_list(&headref); //calls the resepctive function to obtain return value 
	assert_int_equal(ret, success); //cmocka function to compare return types(only compares integral return types
}


int main(int argc, char **argv)
{
  const struct CMUnitTest tests[] = {
   cmocka_unit_test(Add_First_Node),
	cmocka_unit_test(Add_Node_Greater_Than_Length),
	cmocka_unit_test(Add_Node_At_Top_Of_List),
	cmocka_unit_test(Add_Node_At_Bottom_Of_List),
	cmocka_unit_test(Remove_Node_Null_Nodes),
	cmocka_unit_test(Remove_Node_Greater_Than_Length),
	cmocka_unit_test(Remove_Node_At_Top_of_List),
	cmocka_unit_test(Remove_Node_At_Bottom_of_List),
	cmocka_unit_test(Right_Size_of_List),
	cmocka_unit_test(Size_of_List_with_Null_Nodes),
	cmocka_unit_test(Destroy_List_with_Null_Nodes),
	cmocka_unit_test(Destroy_List),
	cmocka_unit_test(print_list_length_zero),
	cmocka_unit_test(print_list_length),//defines members of struture CMUnitTest with different unit tests initialised above 
	
	
	};

  return cmocka_run_group_tests(tests, NULL, NULL); //runs group of tests 
}
