#ifndef AKINATOR_INCLUDED
#define AKINATOR_INCLUDED

#include "bintree.h"

typedef struct
{
    node_t * root;
    node_t * cur_node;
} akinator_t;

void akinatorCtor(akinator_t * akinator, FILE * base_file);

void akinatorDtor(akinator_t * akinator);

node_t * akinatorReadFromFile(FILE * base_file, node_t * parent);

void akinatorTreeDtor(node_t * root_node);

void akinatorPtrToStr(char * str, void * str_ptr);

const char * const FORMAT_TO_READ_STR  = "";

const char * const FORMAT_TO_WRITE_STR = "";

#endif
