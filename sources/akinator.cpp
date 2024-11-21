#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <wchar.h>

#include "akinator.h"
#include "binTree.h"
#include "logger.h"
#include "parser.h"

#define DEF_STR_(arg)   \
    static const char * arg = "";

#include "strs_to_load.h"

#undef DEF_STR_

/// @brief dumps tree to file (used in akinatorWriteBaseToFile)
static void dumpTreeToFile(node_t * root_node, FILE * base_file, elemtostr_func_t dataToStr, size_t tab_nums);

/// @brief reads boolean answer of user and returns true if it is positive
static bool answerIsYes();

/// @brief adds new element to the tree
static void addNewElement(akinator_t * akinator);

/// @brief prints property from definition
static void printProperty(property_t * prop);

/// @brief function to print strings from tree right
static void akinatorPtrToStr(char * str, void * str_ptr);

/// @brief function to compare nodes data fields
static int akinatorCmpStrs(void * first_ptr, void * second_ptr);

static json_obj_t * loadConfig(akinator_t * akinator, FILE * config_file);

static json_obj_t * config = NULL;

const size_t BUF_LEN = 128;

void akinatorCtor(akinator_t * akinator, const char * base_file_name)
{
    assert(akinator);
    assert(base_file_name);

    FILE * config_file = fopen("config.json", "r");
    config = loadConfig(akinator, config_file);
    fclose(config_file);

    logPrint(LOG_DEBUG_PLUS, "akinator: constructing akinator...\n");

    strcpy(akinator->base_file_name, base_file_name);

    FILE * base_file = fopen(akinator->base_file_name, "r");
    akinator->root = akinatorReadBaseFromFile(base_file, NULL);
    fclose(base_file);

    akinator->cur_node = akinator->root;
    akinator->dataToStr = akinatorPtrToStr;

    logPrint(LOG_DEBUG_PLUS, "akinator: constructed akinator (root = %p, cur_node = %p, dataToStr = %p)\n",
            akinator->root, akinator->cur_node, akinator->dataToStr);
}

akinator_mode_t akinatorGetMode(akinator_t * akinator)
{
    printf(MODE_QUESTION);

    while(1){
        char answer[BUF_LEN] = "";
        scanf("%s", answer);

        if (strcasecmp(PLAY_MODE_ANSWER, answer) == 0)
            return PLAY_MODE;

        if (strcasecmp(DEFINITION_MODE_ANSWER, answer) == 0)
            return DEFINITION_MODE;

        if (strcasecmp(DIFF_MODE_ANSWER, answer) == 0)
            return DIFF_MODE;

        else
            printf(BAD_MODE_ANSWER);
    }
    return PLAY_MODE;
}

void akinatorDtor(akinator_t * akinator)
{
    assert(akinator);

    logPrint(LOG_DEBUG_PLUS, "akinator: destructing akinator...\n");

    treeDestroy(akinator->root);
    akinator->root = NULL;
    akinator->cur_node = NULL;

    jsonObjDtor(config);

    logPrint(LOG_DEBUG_PLUS, "akinator: destructed akinator...\n");
}

void akinatorPlay(akinator_t * akinator)
{
    assert(akinator);

    logPrint(LOG_DEBUG_PLUS, "started playing akinator (root = %p, cur_node = %p)\n", akinator->root, akinator->cur_node);

    node_t * next_node = akinator->cur_node;
    bool last_answer = false;
    while (next_node != NULL){
        akinator->cur_node = next_node;
        printf(FORMAT_OF_QUESTION, (char *)(akinator->cur_node->data));
        if ((last_answer = answerIsYes()) == true)
            next_node = akinator->cur_node->left;
        else
            next_node = akinator->cur_node->right;
    }
    if (last_answer)
        printf(WINNING_PHRASE);
    else {
        printf(LOSING_PHRASE);
        addNewElement(akinator);
    }
    logPrint(LOG_DEBUG_PLUS, "ended playing akinator (root = %p, cur_node = %p)\n", akinator->root, akinator->cur_node);
}

static bool answerIsYes()
{
    while (1){
        char ans_buf[BUF_LEN] = "";
        scanf("%s", ans_buf);

        if (strcasecmp(ans_buf, YES_ANSWER) == 0)
            return true;

        if (strcasecmp(ans_buf, NO_ANSWER) == 0)
            return false;

        else
            printf(INCORRECT_YES_NO_ANS);
    }
}

static void addNewElement(akinator_t * akinator)
{
    logPrint(LOG_DEBUG_PLUS, "akinator: adding new element into the tree\n");

    char new_elem_buf[BUF_LEN] = "";
    scanf(FORMAT_TO_READ_CONSOLE_STR, new_elem_buf);

    logPrint(LOG_DEBUG_PLUS, "\tread %s\n", new_elem_buf);

    printf(FORMAT_OF_DIFF_Q, new_elem_buf, (char *)(akinator->cur_node->data));

    char new_question_buf[BUF_LEN] = "";
    scanf(FORMAT_TO_READ_CONSOLE_STR, new_question_buf);

    logPrint(LOG_DEBUG_PLUS, "\tdiff question %s\n", new_question_buf);

    node_t * new_question_node = newNode(new_question_buf, BUF_LEN * (sizeof(char)));
    node_t * new_element_node  = newNode(new_elem_buf,     BUF_LEN * (sizeof(char)));

    if (akinator->cur_node == akinator->cur_node->parent->left)
        akinator->cur_node->parent->left = new_question_node;
    else
        akinator->cur_node->parent->right = new_question_node;

    new_question_node->parent = akinator->cur_node->parent;
    new_question_node->left   = new_element_node;
    new_question_node->right  = akinator->cur_node;

    akinator->cur_node->parent = new_question_node;
    new_element_node->parent   = new_question_node;

    logPrint(LOG_DEBUG_PLUS, "akinator: added new element into the tree\n");
}

node_t * akinatorReadBaseFromFile(FILE * base_file, node_t * parent)
{
    assert(base_file);

    logPrint(LOG_DEBUG_PLUS, "akinator: reading token (parent = %p)\n", parent);

    char      buffer[BUF_LEN] = "";
    char type_buffer[BUF_LEN] = "";

    fscanf(base_file, FORMAT_TO_READ_STR, buffer, type_buffer);

    node_t * node = newNode(buffer, sizeof(char) * BUF_LEN);
    node->parent = parent;
    logPrint(LOG_DEBUG_PLUS, "\tadded node %p (str = \"%s\")\n", node, buffer);

    if (strcmp(type_buffer, END_OF_QUESTION) == 0){
        logPrint(LOG_DEBUG_PLUS, "\tadding left node to %p (str = \"%s\")...\n", node, buffer);
        node->left  = akinatorReadBaseFromFile(base_file, node);

        logPrint(LOG_DEBUG_PLUS, "\tadding right node to %p (str = \"%s\")...\n", node, buffer);
        node->right = akinatorReadBaseFromFile(base_file, node);
    }
    return node;
}

void akinatorWriteBaseToFile(akinator_t * akinator, FILE * base_file)
{
    assert(akinator);
    assert(base_file);

    logPrint(LOG_DEBUG_PLUS, "akinator: writing base to file...\n");

    dumpTreeToFile(akinator->root, base_file, akinator->dataToStr, 0);

    logPrint(LOG_DEBUG_PLUS, "akinator: writed base to file\n");
}

static void dumpTreeToFile(node_t * root_node, FILE * base_file, elemtostr_func_t dataToStr, size_t tab_nums)
{
    assert(base_file);
    assert(dataToStr != NULL);

    for(size_t tab = 0; tab < tab_nums; tab++)
        fprintf(base_file, "\t");

    char str[BUF_LEN] = "";
    dataToStr((char *)str, root_node->data);

    if (root_node->left != NULL || root_node->right != NULL)
        fprintf(base_file, FORMAT_TO_WRITE_STR, str, END_OF_QUESTION);
    else
        fprintf(base_file, FORMAT_TO_WRITE_STR, str, END_OF_ANSWER);

    if (root_node->left != NULL)
        dumpTreeToFile(root_node->left , base_file, dataToStr, tab_nums + 1);

    if (root_node->right != NULL)
        dumpTreeToFile(root_node->right, base_file, dataToStr, tab_nums + 1);
}

definition_t akinatorMakeDefinition(akinator_t * akinator, node_t * node)
{
    assert(akinator);
    assert(akinator->root);
    assert(node);

    logPrint(LOG_DEBUG_PLUS, "entered akinatorMakeDefinition str = \"%s\"\n", (char *)node->data);

    definition_t definition = {};
    size_t def_count = 0;

    definition.obj_name = (char *)node->data;

    akinator->cur_node = node;

    while(akinator->cur_node->parent != NULL){
        definition.props[def_count].name = (char *)(akinator->cur_node->parent->data);
        logPrint(LOG_DEBUG_PLUS, "\tfound characteristic for node (%s)\n", definition.props[def_count].name);

        if (akinator->cur_node == akinator->cur_node->parent->right){
            definition.props[def_count].is_positive = false;

            logPrint(LOG_DEBUG_PLUS, "\t\tthis characteristic is negative (%s)\n", definition.props[def_count].name);
        }
        else {
            definition.props[def_count].is_positive = true;

            logPrint(LOG_DEBUG_PLUS, "\t\tthis characteristic is positive (%s)\n", definition.props[def_count].name);
        }
        def_count++;
        akinator->cur_node = akinator->cur_node->parent;
    }
    definition.num_of_props = def_count;

    logPrint(LOG_DEBUG_PLUS, "found definition, exiting akinatorMakeDefinition str = \"%s\"\n", definition.obj_name);

    return definition;
}


int akinatorGiveDefinition(akinator_t * akinator, definition_t * definition, char * sample)
{
    assert(akinator);
    assert(akinator->root);
    assert(sample);

    logPrint(LOG_DEBUG_PLUS, "entered akinatorGiveDefinition str = \"%s\"\n", sample);

    node_t * searched_node = treeFindNode(akinator->root, sample, akinatorCmpStrs);

    if (searched_node == NULL){
        logPrint(LOG_DEBUG_PLUS, "\tdid not find node\n");
        return 1;
    }

    akinator->cur_node = searched_node;
    *definition = akinatorMakeDefinition(akinator, searched_node);

    logPrint(LOG_DEBUG_PLUS, "exiting akinatorGiveDefinition\n");

    return 0;
}

void akinatorPrintDefinition(definition_t * definition)
{
    assert(definition);

    logPrint(LOG_DEBUG_PLUS, "printing definition (name = \"%s\")...\n", definition->obj_name);

    printf(FORMAT_OF_DEFINITION, definition->obj_name);

    for (ssize_t prop_index = (ssize_t)definition->num_of_props - 1; prop_index >= 0; prop_index--){
        property_t prop = definition->props[prop_index];

        if (prop.is_positive)
            printf(FORMAT_OF_POS_PROPERTY, prop.name);
        else
            printf(FORMAT_OF_NEG_PROPERTY, prop.name);

        if (prop_index != 0)
            printf(", ");
    }
    printf("\n");

    logPrint(LOG_DEBUG_PLUS, "printed definition (name = \"%ls\")\n", definition->obj_name);
}

definition_t akinatorGetDefinition(akinator_t * akinator)
{
    definition_t def = {};

    printf(DEF_TEXT_REQUEST_STR);

    bool def_is_given = false;
    while (!def_is_given){
        char def_text[BUF_LEN] = "";

        scanf(FORMAT_TO_READ_CONSOLE_STR, def_text);

        if (akinatorGiveDefinition(akinator, &def, def_text) == 0)
            def_is_given = true;
        else
            printf(INCORRECT_DEF_NAME_STR, def_text);
    }
    return def;
}

static bool isPropsEqual(property_t prop1, property_t prop2);

void akinatorPrintDifference(definition_t * first_def, definition_t * second_def)
{
    assert( first_def);
    assert(second_def);

    logPrint(LOG_DEBUG_PLUS, "entered akinatorPrintDifference\n");

    ssize_t  first_index = (ssize_t) first_def->num_of_props - 1;
    ssize_t second_index = (ssize_t)second_def->num_of_props - 1;

    logPrint(LOG_DEBUG_PLUS, "\tprinting commom properties...");

    if (!isPropsEqual(first_def->props[first_index], second_def->props[second_index]))
        printf(NO_COMMON_IN_DEF_STR);
    else
        printf(COMMON_IN_DEFS_STR);

    while (isPropsEqual(first_def->props[first_index], second_def->props[second_index])){
        property_t prop = first_def->props[first_index];
        printProperty(&prop);

        printf(", ");

         first_index--;
        second_index--;
    }
    printf("\n");

    logPrint(LOG_DEBUG_PLUS, "\tprinting different properties...");

    printf(DIFFERENT_IN_DEFS_STR);
    printf("\"%s\" - ", first_def->obj_name);

    for(;first_index >= 0; first_index--){
        property_t prop = first_def->props[first_index];
        printProperty(&prop);

        if (first_index != 0)
            printf(", ");
    }

    printf(DIFF_MEDIUM_PART_STR);
    printf("\"%s\" - ", second_def->obj_name);

    for(;second_index >= 0; second_index--){
        property_t prop = second_def->props[second_index];
        printProperty(&prop);

        if (second_index != 0)
            printf(", ");
    }
    printf("\n");

    logPrint(LOG_DEBUG_PLUS, "quitting akinatorPrintDifference\n");
}

static bool isPropsEqual(property_t prop1, property_t prop2)
{
    bool names_are_equal = prop1.name        == prop2.name;
    bool signs_are_equal = prop1.is_positive == prop2.is_positive;

    return names_are_equal && signs_are_equal;
}

void akinatorLaunch(akinator_t * akinator, akinator_mode_t launch_mode)
{
    switch (launch_mode){
        case PLAY_MODE:{
            akinatorPlay(akinator);

            printf(SAVE_BASE_QUESTION);
            if (answerIsYes()){
                FILE * base_file = fopen(akinator->base_file_name, "w");
                akinatorWriteBaseToFile(akinator, base_file);
                fclose(base_file);
            }
            break;
        }
        case DEFINITION_MODE:{
            definition_t def = akinatorGetDefinition(akinator);
            akinatorPrintDefinition(&def);

            break;
        }
        case DIFF_MODE:{
            definition_t  first_def = akinatorGetDefinition(akinator);
            definition_t second_def = akinatorGetDefinition(akinator);

            akinatorPrintDifference(&first_def, &second_def);
            break;
        }
        default:{
            printf("не поддерживаю такой режим / его не существует\n");
            break;
        }
    }
}

#define DEF_STR_(arg)                       \
    arg = findObject(config, #arg)->value;

static json_obj_t * loadConfig(akinator_t * akinator, FILE * config_file)
{
    json_obj_t * config = parseJSON(config_file);

    #include "strs_to_load.h"

    return config;
}

#undef DEF_STR_

void akinatorDump(akinator_t * akinator)
{
    logPrint(LOG_DEBUG, "<h2>---------AKINATOR_DUMP---------</h2>");

    logPrint(LOG_DEBUG, "\troot      = %p\n", akinator->root);
    logPrint(LOG_DEBUG, "\tcur_node  = %p\n", akinator->cur_node);
    logPrint(LOG_DEBUG, "\tdataToStr = %p\n", akinator->dataToStr);

    if (akinator->root != NULL)
        treeDumpGraph(akinator->root, akinatorPtrToStr);

    logPrint(LOG_DEBUG, "<h2>-------AKINATOR_DUMP_END-------</h2>");
}


static void printProperty(property_t * prop)
{
    const char * format_of_property = (prop->is_positive) ? FORMAT_OF_POS_PROPERTY : FORMAT_OF_NEG_PROPERTY;
    printf(format_of_property, prop->name);
}

static void akinatorPtrToStr(char * str, void * str_ptr)
{
    assert(str);
    assert(str_ptr);

    const char * akin_str = (const char *)str_ptr;
    sprintf(str, "%s", akin_str);
}

static int akinatorCmpStrs(void * first_ptr, void * second_ptr)
{
    assert( first_ptr);
    assert(second_ptr);

    const char *  first_str = (const char *) first_ptr;
    const char * second_str = (const char *)second_ptr;

    wchar_t  first_wcs[BUF_LEN] = L"";
    wchar_t second_wcs[BUF_LEN] = L"";

    mbstowcs( first_wcs,  first_str, BUF_LEN);
    mbstowcs(second_wcs, second_str, BUF_LEN);

    return wcscasecmp(first_wcs, second_wcs);
}
