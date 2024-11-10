#ifndef AKINATOR_INCLUDED
#define AKINATOR_INCLUDED

#include <wchar.h>

#include "bintree.h"

/// @brief akinator structure itself
typedef struct
{
    node_t * root;
    node_t * cur_node;
    elemtowcs_func_t dataToStr;
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
void akinatorPtrToStr(wchar_t * str, void * str_ptr);

const wchar_t * const FORMAT_TO_READ_STR           = L" %*[\";] %l[^\"] %*[\";] ";
const wchar_t * const FORMAT_TO_WRITE_STR          = L"\"%ls\";\n";
const wchar_t * const NULL_ELEMENT_STR             = L"_null";
const wchar_t * const FORMAT_TO_READ_CONSOLE_STR   = L" %[^\n]";


const wchar_t * const FORMAT_OF_QUESTION = L"Is it %ls? (Y/N)\n";
const wchar_t * const WINNING_PHRASE     = L"Oh, I guessed it again!\n";
const wchar_t * const LOSING_PHRASE      = L"Please type the object you guessed\n";
const wchar_t * const FORMAT_OF_DIFF_Q   = L"How is %ls, differ from %ls. It is ...\n";

const wchar_t * const YES_ANSWER = L"Y";
const wchar_t * const  NO_ANSWER = L"N";

#endif
