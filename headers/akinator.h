#ifndef AKINATOR_INCLUDED
#define AKINATOR_INCLUDED

#include <wchar.h>
#include <stdbool.h>

#include "bintree.h"

const size_t MAX_FILE_NAME_LEN = 128;

/// @brief akinator structure itself
typedef struct {
    char base_file_name[MAX_FILE_NAME_LEN];
    node_t * root;
    node_t * cur_node;
    elemtostr_func_t dataToStr;
} akinator_t;

/// @brief structure for one property of the definition
typedef struct {
    char * name;
    bool is_positive;
} property_t;

const size_t MAX_DEFINITION_DEPTH = 128;

/// @brief structure for definitions
typedef struct {
    char * obj_name;
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
void akinatorCtor(akinator_t * akinator, const char * base_file_name);

/// @brief destructs akinator structure
void akinatorDtor(akinator_t * akinator);

/// @brief main akinator launching function
void akinatorLaunch(akinator_t * akinator, akinator_mode_t launch_mode);

/// @brief dumps akinator info to log file
void akinatorDump(akinator_t * akinator);

/// @brief launches guessing game
void akinatorPlay(akinator_t * akinator);

/// @brief reads akinator tree from file with database
node_t * akinatorReadBaseFromFile(FILE * base_file, node_t * parent);

/// @brief dumps base to file
void akinatorWriteBaseToFile(akinator_t * akinator, FILE * base_file);

/// @brief makes definition for node
definition_t akinatorMakeDefinition(akinator_t * akinator, node_t * node);

/// @brief makes definition by the name of the object
int akinatorGiveDefinition(akinator_t * akinator, definition_t * definition, char * sample);

/// @brief gets definition from console
definition_t akinatorGetDefinition(akinator_t * akinator);

/// @brief prints definition to console
void akinatorPrintDefinition(definition_t * definition);

/// @brief prints difference between two definitions
void akinatorPrintDifference(definition_t * first_def, definition_t * second_def);

/// @brief returns launching mode of akinator
akinator_mode_t akinatorGetMode(akinator_t * akinator);

/*-------------------------SCANNING AND READING-------------------------*/
const char * const FORMAT_TO_READ_STR           = " \"%[^\"]\" %[^\n] ";
const char * const FORMAT_TO_WRITE_STR          = "\"%s\"%s\n";
const char * const NULL_ELEMENT_STR             = "_null";
const char * const FORMAT_TO_READ_CONSOLE_STR   = " %[^\n]";

const char * const END_OF_QUESTION   = "?";
const char * const END_OF_ANSWER     = ";";

const char * const YES_ANSWER = "Y";
const char * const  NO_ANSWER = "N";
/*----------------------------------------------------------------------*/

/*--------------------------TEXTS CONSOLE OUT---------------------------*/
// const char * const FORMAT_OF_QUESTION = "Is it %ls? (Y/N)\n";
// const char * const WINNING_PHRASE     = "Oh, I guessed it again!\n";
// const char * const LOSING_PHRASE      = "Please type the object you guessed\n";
// const char * const FORMAT_OF_DIFF_Q   = "How is %ls differ from %ls. It is ...\n";

const char * const FORMAT_OF_QUESTION        = "Это %s? (Y/N)\n";
const char * const WINNING_PHRASE            = "Ха! Опять угадал!\n";
const char * const LOSING_PHRASE             = "Пожалуйста, введите, что вы загадали\n";
const char * const FORMAT_OF_DIFF_Q          = "Чем %s отличается от %s. Он (она/оно) ...\n";
const char * const INCORRECT_YES_NO_ANS      = "Y или N нажми пж\n";

const char * const DEF_TEXT_REQUEST_STR      = "Введите название объекта: \n";
const char * const CANNOT_FIND_STR           = "Не смог найти объект \"%s\"...\n";
const char * const FORMAT_OF_DEFINITION      = "Определение %s: ";
const char * const INCORRECT_DEF_NAME_STR    = "Не получается дать определение объекта \"%s\" (его нет в базе), трай хардер\n";
const char * const FORMAT_OF_POS_PROPERTY    = "%s";
const char * const FORMAT_OF_NEG_PROPERTY    = "не %s";

const char * const COMMON_IN_DEFS_STR        = "Эти объекты схожи тем, что они оба ";
const char * const NO_COMMON_IN_DEF_STR      = "Эти объекты не имеют ничего общего.";
const char * const DIFFERENT_IN_DEFS_STR     = "Эти объекты различаются тем, что ";
const char * const DIFF_MEDIUM_PART_STR      = ",\na ";

const char * const MODE_QUESTION             = "В каком режиме хотите использовать акинатор (игра/опр/срав)?\n";
const char * const BAD_MODE_ANSWER           = "Нет такого режима, трай хардер\n";
const char * const PLAY_MODE_ANSWER          = "игра";
const char * const DEFINITION_MODE_ANSWER    = "опр";
const char * const DIFF_MODE_ANSWER          = "срав";

const char * const SAVE_BASE_QUESTION        = "Сохранить базу в файл? (Y/n)\n";
/*----------------------------------------------------------------------*/

#endif
