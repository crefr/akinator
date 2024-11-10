#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <wchar.h>

// #include <unistd.h>

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
    logPrint(LOG_DEBUG_PLUS, "akinator: constructing akinator...\n");
    akinator->root = akinatorReadFromFile(base_file, NULL);
    akinator->cur_node = akinator->root;
    akinator->dataToStr = akinatorPtrToStr;
    logPrint(LOG_DEBUG_PLUS, "akinator: constructed akinator (root = %p, cur_node = %p, dataToStr = %p)\n",
            akinator->root, akinator->cur_node, akinator->dataToStr);
}

void akinatorDtor(akinator_t * akinator)
{
    assert(akinator);
    logPrint(LOG_DEBUG_PLUS, "akinator: destructing akinator...\n");
    treeDestroy(akinator->root);
    akinator->root = NULL;
    akinator->cur_node = NULL;
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
    logPrint(LOG_DEBUG_PLUS, "ended playing akinator (root = %p, cur_node = %p)\n", akinator->root, akinator->cur_node);
}

static bool answerIsYes()
{
    while (1){
        wchar_t ans_buf[BUF_LEN] = L"";
        wscanf(L"%s", ans_buf);

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
    logPrint(LOG_DEBUG_PLUS, "akinator: adding new element into the tree\n");
    wchar_t new_elem_buf[BUF_LEN] = L"";
    wscanf(FORMAT_TO_READ_CONSOLE_STR, new_elem_buf);
    logPrint(LOG_DEBUG_PLUS, "\tread %s\n", new_elem_buf);

    wprintf(FORMAT_OF_DIFF_Q, new_elem_buf, (wchar_t *)(akinator->cur_node->data));

    wchar_t new_question_buf[BUF_LEN] = L"";
    wscanf(FORMAT_TO_READ_CONSOLE_STR, new_question_buf);
    logPrint(LOG_DEBUG_PLUS, "\tdiff question %s\n", new_question_buf);

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
    logPrint(LOG_DEBUG_PLUS, "akinator: added new element into the tree\n");
}

node_t * akinatorReadFromFile(FILE * base_file, node_t * parent)
{
    assert(base_file);
    logPrint(LOG_DEBUG_PLUS, "akinator: reading token (parent = %p)\n", parent);
    wchar_t buffer[BUF_LEN] = L"";

    fwscanf(base_file, FORMAT_TO_READ_STR, buffer);
    // wprintf(L"%ls\n", buffer);
    // wprintf(L"%ls\n", NULL_ELEMENT_STR);
    // sleep(1);

    if (wcscmp(buffer, NULL_ELEMENT_STR) == 0){
        logPrint(LOG_DEBUG_PLUS, "\tno node (null)\n");
        return NULL;
    }
    node_t * node = newNode(buffer, sizeof(wchar_t) * BUF_LEN);
    node->parent = parent;
    logPrint(LOG_DEBUG_PLUS, "\tadded node %p (str = \"%s\")\n", node, buffer);

    logPrint(LOG_DEBUG_PLUS, "\tadding left node to %p (str = \"%s\")...\n", node, buffer);
    node->left  = akinatorReadFromFile(base_file, node);
    logPrint(LOG_DEBUG_PLUS, "\tadding right node to %p (str = \"%s\")...\n", node, buffer);
    node->right = akinatorReadFromFile(base_file, node);
    return node;
}

void akinatorDumpBaseToFile(akinator_t * akinator, FILE * base_file)
{
    assert(akinator);
    assert(base_file);
    logPrint(LOG_DEBUG_PLUS, "akinator: dumping to file...\n");
    dumpTreeToFile(akinator->root, base_file, akinator->dataToStr, 0);
    logPrint(LOG_DEBUG_PLUS, "akinator: dumped to file\n");
}

static void dumpTreeToFile(node_t * root_node, FILE * base_file, elemtowcs_func_t dataToStr, size_t tab_nums)
{
    assert(base_file);
    assert(dataToStr != NULL);
    for(size_t tab = 0; tab < tab_nums; tab++)
        fwprintf(base_file, L"\t");
    if (root_node == NULL){
        fwprintf(base_file, FORMAT_TO_WRITE_STR, NULL_ELEMENT_STR);
        return;
    }
    wchar_t str[BUF_LEN] = L"";
    dataToStr((wchar_t *)str, root_node->data);
    fwprintf(base_file, FORMAT_TO_WRITE_STR, str);
    dumpTreeToFile(root_node->left , base_file, dataToStr, tab_nums + 1);
    dumpTreeToFile(root_node->right, base_file, dataToStr, tab_nums + 1);
}

void akinatorPtrToStr(wchar_t * str, void * str_ptr)
{
    wchar_t * akin_str = (wchar_t *) str_ptr;
    wcscpy(str, akin_str);
}
