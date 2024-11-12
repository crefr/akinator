#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <wchar.h>

#include "akinator.h"
#include "binTree.h"
#include "logger.h"

static void dumpTreeToFile(node_t * root_node, FILE * base_file, elemtowcs_func_t dataToStr, size_t tab_nums);

static bool answerIsYes();

static void addNewElement(akinator_t * akinator);

const size_t BUF_LEN = 128;

void akinatorCtor(akinator_t * akinator, FILE * base_file)
{
    assert(akinator);
    assert(base_file);
    wlogPrint(LOG_DEBUG_PLUS, L"akinator: constructing akinator...\n");
    akinator->root = akinatorReadFromFile(base_file, NULL);
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

node_t * akinatorReadFromFile(FILE * base_file, node_t * parent)
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
        node->left  = akinatorReadFromFile(base_file, node);
        wlogPrint(LOG_DEBUG_PLUS, L"\tadding right node to %p (str = \"%ls\")...\n", node, buffer);
        node->right = akinatorReadFromFile(base_file, node);
    }
    return node;
}

void akinatorDumpBaseToFile(akinator_t * akinator, FILE * base_file)
{
    assert(akinator);
    assert(base_file);
    wlogPrint(LOG_DEBUG_PLUS, L"akinator: dumping to file...\n");
    dumpTreeToFile(akinator->root, base_file, akinator->dataToStr, 0);
    wlogPrint(LOG_DEBUG_PLUS, L"akinator: dumped to file\n");
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

void akinatorGiveDefinition(akinator_t * akinator, wchar_t * sample)
{
    assert(akinator);
    assert(akinator->root);
    assert(sample);
    wlogPrint(LOG_DEBUG_PLUS, L"entered akinatorGiveDefinition\n");
    node_t * searched_node = treeFindNode(akinator->root, sample, akinatorCmpWcs);
    if (searched_node == NULL){
        wlogPrint(LOG_DEBUG_PLUS, L"\tdid not find node\n");
        wprintf(CANNOT_FIND_STR, sample);
        return;
    }
    akinator->cur_node = searched_node;
    wchar_t * pos_definition[MAX_DEFINITION_DEPTH] = {};
    wchar_t * neg_definition[MAX_DEFINITION_DEPTH] = {};
    size_t pos_def_count = 0;
    size_t neg_def_count = 0;
    while(akinator->cur_node->parent != NULL){
        if (akinator->cur_node == akinator->cur_node->parent->left){
            akinator->cur_node = akinator->cur_node->parent;
            pos_definition[pos_def_count] = (wchar_t *)(akinator->cur_node->data);
            wlogPrint(LOG_DEBUG_PLUS, L"\tfound positive characteristic for node\n");
            pos_def_count++;
        }
        else {
            akinator->cur_node = akinator->cur_node->parent;
            neg_definition[neg_def_count] = (wchar_t *)(akinator->cur_node->data);
            wlogPrint(LOG_DEBUG_PLUS, L"\tfound negative characteristic for node\n");
            neg_def_count++;
        }
    }
    wprintf(FORMAT_OF_DEFINITION, (wchar_t *)(searched_node->data));

    size_t def_index = pos_def_count;
    do {
        def_index--;
        wprintf(FORMAT_OF_POS_CHAR, pos_definition[def_index]);
    } while (def_index != 0);

    def_index = neg_def_count;
    do {
        def_index--;
        wprintf(FORMAT_OF_NEG_CHAR, neg_definition[def_index]);
    } while (def_index != 0);
    wprintf(L"\n");

    wlogPrint(LOG_DEBUG_PLUS, L"exiting akinatorGiveDefinition\n");
}

void akinatorPtrToStr(wchar_t * str, void * str_ptr)
{
    wchar_t * akin_str = (wchar_t *)str_ptr;
    swprintf(str, BUF_LEN,  L"%ls", akin_str);
}

int akinatorCmpWcs(void * first_ptr, void * second_ptr)
{
    wchar_t *  first_wcs = (wchar_t *) first_ptr;
    wchar_t * second_wcs = (wchar_t *)second_ptr;

    return wcscasecmp(first_wcs, second_wcs);
}
