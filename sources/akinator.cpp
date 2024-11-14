#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <wchar.h>

#include "akinator.h"
#include "binTree.h"
#include "logger.h"

/// @brief dumps tree to file (used in akinatorWriteBaseToFile)
static void dumpTreeToFile(node_t * root_node, FILE * base_file, elemtowcs_func_t dataToStr, size_t tab_nums);

/// @brief reads boolean answer of user and returns true if it is positive
static bool answerIsYes();

/// @brief adds new element to the tree
static void addNewElement(akinator_t * akinator);

/// @brief prints property from definition
static void printProperty(property_t * prop);

const size_t BUF_LEN = 128;

void akinatorCtor(akinator_t * akinator, FILE * base_file)
{
    assert(akinator);
    assert(base_file);

    wlogPrint(LOG_DEBUG_PLUS, L"akinator: constructing akinator...\n");

    akinator->root = akinatorReadBaseFromFile(base_file, NULL);
    akinator->cur_node = akinator->root;
    akinator->dataToStr = akinatorPtrToStr;

    wlogPrint(LOG_DEBUG_PLUS, L"akinator: constructed akinator (root = %p, cur_node = %p, dataToStr = %p)\n",
            akinator->root, akinator->cur_node, akinator->dataToStr);
}

akinator_mode_t akinatorGetMode(akinator_t * akinator)
{
    wprintf(MODE_QUESTION);

    while(1){
        wchar_t answer[BUF_LEN] = L"";
        wscanf(L"%ls", answer);

        if (wcscasecmp(PLAY_MODE_ANSWER, answer) == 0)
            return PLAY_MODE;

        if (wcscasecmp(DEFINITION_MODE_ANSWER, answer) == 0)
            return DEFINITION_MODE;

        if (wcscasecmp(DIFF_MODE_ANSWER, answer) == 0)
            return DIFF_MODE;

        else
            wprintf(BAD_MODE_ANSWER);
    }
    return PLAY_MODE;
}

void akinatorDtor(akinator_t * akinator)
{
    assert(akinator);

    wlogPrint(LOG_DEBUG_PLUS, L"akinator: destructing akinator...\n");

    treeDestroy(akinator->root);
    akinator->root = NULL;
    akinator->cur_node = NULL;

    wlogPrint(LOG_DEBUG_PLUS, L"akinator: destructed akinator...\n");
}

void akinatorPlay(akinator_t * akinator)
{
    assert(akinator);

    wlogPrint(LOG_DEBUG_PLUS, L"started playing akinator (root = %p, cur_node = %p)\n", akinator->root, akinator->cur_node);

    node_t * next_node = akinator->cur_node;
    bool last_answer = false;
    while (next_node != NULL){
        akinator->cur_node = next_node;
        wprintf(FORMAT_OF_QUESTION, (wchar_t *)(akinator->cur_node->data));
        if ((last_answer = answerIsYes()) == true)
            next_node = akinator->cur_node->left;
        else
            next_node = akinator->cur_node->right;
    }
    if (last_answer)
        wprintf(WINNING_PHRASE);
    else {
        wprintf(LOSING_PHRASE);
        addNewElement(akinator);
    }
    wlogPrint(LOG_DEBUG_PLUS, L"ended playing akinator (root = %p, cur_node = %p)\n", akinator->root, akinator->cur_node);
}

static bool answerIsYes()
{
    while (1){
        wchar_t ans_buf[BUF_LEN] = L"";
        wscanf(L"%ls", ans_buf);

        if (wcscasecmp(ans_buf, YES_ANSWER) == 0){
            return true;
        }
        if (wcscasecmp(ans_buf, NO_ANSWER) == 0){
            return false;
        }
    }
}

static void addNewElement(akinator_t * akinator)
{
    wlogPrint(LOG_DEBUG_PLUS, L"akinator: adding new element into the tree\n");

    wchar_t new_elem_buf[BUF_LEN] = L"";
    wscanf(FORMAT_TO_READ_CONSOLE_STR, new_elem_buf);

    wlogPrint(LOG_DEBUG_PLUS, L"\tread %ls\n", new_elem_buf);

    wprintf(FORMAT_OF_DIFF_Q, new_elem_buf, (wchar_t *)(akinator->cur_node->data));

    wchar_t new_question_buf[BUF_LEN] = L"";
    wscanf(FORMAT_TO_READ_CONSOLE_STR, new_question_buf);

    wlogPrint(LOG_DEBUG_PLUS, L"\tdiff question %ls\n", new_question_buf);

    node_t * new_question_node = newNode(new_question_buf, BUF_LEN * (sizeof(wchar_t)));
    node_t * new_element_node  = newNode(new_elem_buf,     BUF_LEN * (sizeof(wchar_t)));

    if (akinator->cur_node == akinator->cur_node->parent->left)
        akinator->cur_node->parent->left = new_question_node;
    else
        akinator->cur_node->parent->right = new_question_node;

    new_question_node->parent = akinator->cur_node->parent;
    new_question_node->left   = new_element_node;
    new_question_node->right  = akinator->cur_node;

    akinator->cur_node->parent = new_question_node;
    new_element_node->parent   = new_question_node;

    wlogPrint(LOG_DEBUG_PLUS, L"akinator: added new element into the tree\n");
}

node_t * akinatorReadBaseFromFile(FILE * base_file, node_t * parent)
{
    assert(base_file);

    wlogPrint(LOG_DEBUG_PLUS, L"akinator: reading token (parent = %p)\n", parent);

    wchar_t      buffer[BUF_LEN] = L"";
    wchar_t type_buffer[BUF_LEN] = L"";

    fwscanf(base_file, FORMAT_TO_READ_STR, buffer, type_buffer);

    node_t * node = newNode(buffer, sizeof(wchar_t) * BUF_LEN);
    node->parent = parent;
    wlogPrint(LOG_DEBUG_PLUS, L"\tadded node %p (str = \"%ls\")\n", node, buffer);

    if (wcscmp(type_buffer, END_OF_QUESTION) == 0){
        wlogPrint(LOG_DEBUG_PLUS, L"\tadding left node to %p (str = \"%ls\")...\n", node, buffer);
        node->left  = akinatorReadBaseFromFile(base_file, node);

        wlogPrint(LOG_DEBUG_PLUS, L"\tadding right node to %p (str = \"%ls\")...\n", node, buffer);
        node->right = akinatorReadBaseFromFile(base_file, node);
    }
    return node;
}

void akinatorWriteBaseToFile(akinator_t * akinator, FILE * base_file)
{
    assert(akinator);
    assert(base_file);

    wlogPrint(LOG_DEBUG_PLUS, L"akinator: writing base to file...\n");

    dumpTreeToFile(akinator->root, base_file, akinator->dataToStr, 0);

    wlogPrint(LOG_DEBUG_PLUS, L"akinator: writed base to file\n");
}

static void dumpTreeToFile(node_t * root_node, FILE * base_file, elemtowcs_func_t dataToStr, size_t tab_nums)
{
    assert(base_file);
    assert(dataToStr != NULL);

    for(size_t tab = 0; tab < tab_nums; tab++)
        fwprintf(base_file, L"\t");

    wchar_t str[BUF_LEN] = L"";
    dataToStr((wchar_t *)str, root_node->data);

    if (root_node->left != NULL || root_node->right != NULL)
        fwprintf(base_file, FORMAT_TO_WRITE_STR, str, END_OF_QUESTION);
    else
        fwprintf(base_file, FORMAT_TO_WRITE_STR, str, END_OF_ANSWER);

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

    wlogPrint(LOG_DEBUG_PLUS, L"entered akinatorMakeDefinition str = \"%ls\"\n", (wchar_t *)node->data);

    definition_t definition = {};
    size_t def_count = 0;

    akinator->cur_node = node;

    while(akinator->cur_node->parent != NULL){
        definition.props[def_count].name = (wchar_t *)(akinator->cur_node->parent->data);
        wlogPrint(LOG_DEBUG_PLUS, L"\tfound characteristic for node (%ls)\n", definition.props[def_count].name);

        if (akinator->cur_node == akinator->cur_node->parent->right){
            definition.props[def_count].is_positive = false;

            wlogPrint(LOG_DEBUG_PLUS, L"\t\tthis characteristic is negative (%ls)\n", definition.props[def_count].name);
        }
        else {
            definition.props[def_count].is_positive = true;

            wlogPrint(LOG_DEBUG_PLUS, L"\t\tthis characteristic is positive (%ls)\n", definition.props[def_count].name);
        }
        def_count++;
        akinator->cur_node = akinator->cur_node->parent;
    }
    definition.num_of_props = def_count;

    wlogPrint(LOG_DEBUG_PLUS, L"found definition, exiting akinatorMakeDefinition str = \"%ls\"", (wchar_t *)node->data);

    return definition;
}


int akinatorGiveDefinition(akinator_t * akinator, definition_t * definition, wchar_t * sample)
{
    assert(akinator);
    assert(akinator->root);
    assert(sample);

    wlogPrint(LOG_DEBUG_PLUS, L"entered akinatorGiveDefinition str = \"%ls\"\n", sample);

    node_t * searched_node = treeFindNode(akinator->root, sample, akinatorCmpWcs);

    if (searched_node == NULL){
        wlogPrint(LOG_DEBUG_PLUS, L"\tdid not find node\n");
        return 1;
    }

    akinator->cur_node = searched_node;
    *definition = akinatorMakeDefinition(akinator, searched_node);

    wlogPrint(LOG_DEBUG_PLUS, L"exiting akinatorGiveDefinition\n");

    return 0;
}

void akinatorPrintDefinition(definition_t * definition)
{
    assert(definition);

    wlogPrint(LOG_DEBUG_PLUS, L"printing definition...\n");

    for (ssize_t prop_index = definition->num_of_props - 1; prop_index >= 0; prop_index--){
        property_t prop = definition->props[prop_index];

        if (prop.is_positive)
            wprintf(FORMAT_OF_POS_PROPERTY, prop.name);
        else
            wprintf(FORMAT_OF_NEG_PROPERTY, prop.name);

        if (prop_index != 0)
            wprintf(L", ");
    }
    wprintf(L"\n");

    wlogPrint(LOG_DEBUG_PLUS, L"printed definition\n");
}

void akinatorPrintDifference(definition_t * first_def, definition_t * second_def)
{
    assert( first_def);
    assert(second_def);

    wlogPrint(LOG_DEBUG_PLUS, L"entered akinatorPrintDifference\n");

    ssize_t  first_index =  first_def->num_of_props - 1;
    ssize_t second_index = second_def->num_of_props - 1;

    wlogPrint(LOG_DEBUG_PLUS, L"\tprinting commom properties...");

    wprintf(COMMON_IN_DEFS_STR);
    while (first_def->props[first_index].name        == second_def->props[second_index].name &&
           first_def->props[first_index].is_positive == second_def->props[second_index].is_positive){

        property_t prop = first_def->props[first_index];
        printProperty(&prop);
        wprintf(L", ");
         first_index--;
        second_index--;
    }
    wlogPrint(LOG_DEBUG_PLUS, L"\tprinting different properties...");

    wprintf(DIFFERENT_IN_DEFS_STR);
    wprintf(L"первый объект - "); // TODO: constants

    for(;first_index >= 0; first_index--){
        property_t prop = first_def->props[first_index];
        printProperty(&prop);
        wprintf(L", ");
    }

    wprintf(DIFF_MEDIUM_PART_STR);
    wprintf(L"второй объект - "); // TODO: constants

    for(;second_index >= 0; second_index--){
        property_t prop = second_def->props[second_index];
        printProperty(&prop);
        wprintf(L", ");
    }

    wlogPrint(LOG_DEBUG_PLUS, L"quitting akinatorPrintDifference\n");
}

static void printProperty(property_t * prop)
{
    const wchar_t * format_of_property = (prop->is_positive) ? FORMAT_OF_POS_PROPERTY : FORMAT_OF_NEG_PROPERTY;
    wprintf(format_of_property, prop->name);
}

void akinatorPtrToStr(wchar_t * str, void * str_ptr)
{
    assert(str);
    assert(str_ptr);

    wchar_t * akin_str = (wchar_t *)str_ptr;
    swprintf(str, BUF_LEN,  L"%ls", akin_str);
}

int akinatorCmpWcs(void * first_ptr, void * second_ptr)
{
    assert( first_ptr);
    assert(second_ptr);

    wchar_t *  first_wcs = (wchar_t *) first_ptr;
    wchar_t * second_wcs = (wchar_t *)second_ptr;

    return wcscasecmp(first_wcs, second_wcs);
}
