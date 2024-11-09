#ifndef AKINATOR_INCLUDED
#define AKINATOR_INCLUDED

#include "bintree.h"

node_t * akinatorReadFromFile(FILE * base_file, node_t * parent);
void akinatorTreeDtor(node_t * root_node);

void akinatorPtrToStr(char * str, void * str_ptr);

const char * FORMAT_TO_READ_STR  = "";
const char * FORMAT_TO_WRITE_STR = "";

#endif
