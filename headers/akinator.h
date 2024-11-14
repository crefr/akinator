#ifndef AKINATOR_INCLUDED
#define AKINATOR_INCLUDED

#include <wchar.h>
#include <stdbool.h>

#include "bintree.h"

/// @brief akinator structure itself
typedef struct {
    node_t * root;
    node_t * cur_node;
    elemtowcs_func_t dataToStr;
} akinator_t;

/// @brief structure for one property of the definition
typedef struct {
    wchar_t * name;
    bool is_positive;
} property_t;

const size_t MAX_DEFINITION_DEPTH = 128;

/// @brief structure for definitions
typedef struct {
    property_t props[MAX_DEFINITION_DEPTH];
    size_t num_of_props;
} definition_t;

/// @brief enum with akinator possible modess
typedef enum {
    PLAY_MODE,
    DEFINITION_MODE,
    DIFF_MODE
} akinator_mode_t;

/// @brief constructs akinator structure
void akinatorCtor(akinator_t * akinator, FILE * base_file);

/// @brief destructs akinator structure
void akinatorDtor(akinator_t * akinator);

/// @brief launches guessing game
void akinatorPlay(akinator_t * akinator);

/// @brief reads akinator tree from file with database
node_t * akinatorReadBaseFromFile(FILE * base_file, node_t * parent);

/// @brief dumps base to file
void akinatorWriteBaseToFile(akinator_t * akinator, FILE * base_file);

/// @brief function to print strings from tree right
void akinatorPtrToStr(wchar_t * str, void * str_ptr);

/// @brief function to compare nodes data fields
int akinatorCmpWcs(void * first_ptr, void * second_ptr);

/// @brief makes definition for node
definition_t akinatorMakeDefinition(akinator_t * akinator, node_t * node);

/// @brief makes definition by the name of the object
int akinatorGiveDefinition(akinator_t * akinator, definition_t * definition, wchar_t * sample);

/// @brief prints definition to console
void akinatorPrintDefinition(definition_t * definition);

/// @brief prints difference between two definitions
void akinatorPrintDifference(definition_t * first_def, definition_t * second_def);

/// @brief returns launching mode of akinator
akinator_mode_t akinatorGetMode(akinator_t * akinator);

const wchar_t * const FORMAT_TO_READ_STR           = L" %*l[\"]%l[^\"]%*l[\"] %l[^\n] ";
const wchar_t * const FORMAT_TO_WRITE_STR          = L"\"%ls\"%ls\n";
const wchar_t * const NULL_ELEMENT_STR             = L"_null";
const wchar_t * const FORMAT_TO_READ_CONSOLE_STR   = L" %l[^\n]";

const wchar_t * const END_OF_QUESTION   = L"?";
const wchar_t * const END_OF_ANSWER     = L";";

// const wchar_t * const FORMAT_OF_QUESTION = L"Is it %ls? (Y/N)\n";
// const wchar_t * const WINNING_PHRASE     = L"Oh, I guessed it again!\n";
// const wchar_t * const LOSING_PHRASE      = L"Please type the object you guessed\n";
// const wchar_t * const FORMAT_OF_DIFF_Q   = L"How is %ls differ from %ls. It is ...\n";

const wchar_t * const FORMAT_OF_QUESTION        = L"Это %ls? (Y/N)\n";
const wchar_t * const WINNING_PHRASE            = L"Ха! Опять угадал!\n";
const wchar_t * const LOSING_PHRASE             = L"Пожалуйста, введите, что вы загадали\n";
const wchar_t * const FORMAT_OF_DIFF_Q          = L"Чем %ls отличается от %ls. Он (она/оно) ...\n";

const wchar_t * const CANNOT_FIND_STR           = L"Не смог найти объект \"%ls\"...\n";
const wchar_t * const FORMAT_OF_DEFINITION      = L"Определение %ls: ";
const wchar_t * const FORMAT_OF_POS_PROPERTY    = L"%ls";
const wchar_t * const FORMAT_OF_NEG_PROPERTY    = L"не %ls";

const wchar_t * const COMMON_IN_DEFS_STR        = L"Эти объекты схожи тем, что они оба ";
const wchar_t * const DIFFERENT_IN_DEFS_STR     = L"Эти объекты различаются тем, что ";
const wchar_t * const DIFF_MEDIUM_PART_STR      = L"a ";

const wchar_t * const MODE_QUESTION             = L"В каком режиме хотите использовать акинатор (игра/опр/срав)?\n";
const wchar_t * const BAD_MODE_ANSWER           = L"Нет такого режима, трай хардер\n";
const wchar_t * const PLAY_MODE_ANSWER          = L"игра";
const wchar_t * const DEFINITION_MODE_ANSWER    = L"опр";
const wchar_t * const DIFF_MODE_ANSWER          = L"срав";

const wchar_t * const YES_ANSWER = L"Y";
const wchar_t * const  NO_ANSWER = L"N";

#endif
