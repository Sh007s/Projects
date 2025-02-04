#include "inverted_index.h"

//Funtion to update new given file passed through CL to the data base
int update_DB(main_node_t **main_head, file_node_t *file_head, char *f_name)
{
	//befor updating validating that file
	if (validation_store_filenames(&file_head, f_name) == SUCCESS)
		read_datafile(main_head, f_name);
	return SUCCESS;
}
