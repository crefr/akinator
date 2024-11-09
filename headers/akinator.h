#ifndef AKINATOR_INCLUDED
#define AKINATOR_INCLUDED

#include "bintree.h"

typedef struct
{
    node_t * root;
    node_t * cur_node;
    elemtostr_func_t dataToStr;
} akinator_t;

void akinatorCtor(akinator_t * akinator, FILE * base_file);

void akinatorDtor(akinator_t * akinator);

void akinatorPlay(akinator_t * akinator);

node_t * akinatorReadFromFile(FILE * base_file, node_t * parent);

void akinatorDumpBaseToFile(akinator_t * akinator, FILE * base_file);

void akinatorPtrToStr(char * str, void * str_ptr);

const char * const FORMAT_TO_READ_STR  = "%s";
const char * const FORMAT_TO_WRITE_STR = "%s\n";

const char * const FORMAT_OF_QUESTION = "Is it %s? (Y/N)\n";
const char * const WINNING_PHRASE     = "Oh, I guessed it again!\n";
const char * const LOSING_PHRASE      = "Please type the object you guessed\n";
const char * const FORMAT_OF_DIFF_Q   = "How is %s, differ from %s. It is ...\n";


const char * const YES_ANSWER = "Y";
const char * const  NO_ANSWER = "N";

#endif
