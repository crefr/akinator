#ifndef AKINATOR_INCLUDED
#define AKINATOR_INCLUDED

#include "bintree.h"

/// @brief akinator structure itself
typedef struct
{
    node_t * root;
    node_t * cur_node;
    elemtostr_func_t dataToStr;
} akinator_t;

/// @brief constructs akinator structure
void akinatorCtor(akinator_t * akinator, FILE * base_file);

/// @brief destructs akinator structure
void akinatorDtor(akinator_t * akinator);

/// @brief launches guessing game
void akinatorPlay(akinator_t * akinator);

/// @brief reads akinator tree from file with database
node_t * akinatorReadFromFile(FILE * base_file, node_t * parent);

/// @brief dumps base to file
void akinatorDumpBaseToFile(akinator_t * akinator, FILE * base_file);

/// @brief function to print strings from tree right
void akinatorPtrToStr(char * str, void * str_ptr);

const char * const FORMAT_TO_READ_STR           = " %*[\";] %[^\"] %*[\";] ";
const char * const FORMAT_TO_WRITE_STR          = "\"%s\";\n";
const char * const NULL_ELEMENT_STR             = "_null";
const char * const FORMAT_TO_READ_CONSOLE_STR   = " %[^\n]";


const char * const FORMAT_OF_QUESTION = "Is it %s? (Y/N)\n";
const char * const WINNING_PHRASE     = "Oh, I guessed it again!\n";
const char * const LOSING_PHRASE      = "Please type the object you guessed\n";
const char * const FORMAT_OF_DIFF_Q   = "How is %s, differ from %s. It is ...\n";

const char * const YES_ANSWER = "Y";
const char * const  NO_ANSWER = "N";

#endif
