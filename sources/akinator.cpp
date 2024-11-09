#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "akinator.h"
#include "binTree.h"
#include "logger.h"

static void dumpTreeToFile(node_t * root_node, FILE * base_file, elemtostr_func_t dataToStr, size_t tab_nums);

static bool answerIsYes();


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
        char new_elem_buf[BUF_LEN] = "";
        scanf("%s", new_elem_buf);
        printf(FORMAT_OF_DIFF_Q, new_elem_buf, (char *)(akinator->cur_node->data));

        char new_question_buf[BUF_LEN] = "";
        scanf("%s", new_question_buf);

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
    }
    logPrint(LOG_DEBUG_PLUS, "ended playing akinator (root = %p, cur_node = %p)\n", akinator->root, akinator->cur_node);
}

static bool answerIsYes()
{
    while (1){
        char ans_buf[BUF_LEN] = "";
        scanf("%s", ans_buf);

        if (strcasecmp(ans_buf, YES_ANSWER) == 0){
            return true;
        }
        if (strcasecmp(ans_buf, NO_ANSWER) == 0){
            return false;
        }
    }
}

node_t * akinatorReadFromFile(FILE * base_file, node_t * parent)
{
    assert(base_file);
    logPrint(LOG_DEBUG_PLUS, "akinator: reading token (parent = %p)\n", parent);
    char buffer[BUF_LEN] = "";

    fscanf(base_file, FORMAT_TO_READ_STR, buffer);

    if (strcmp(buffer, "null") == 0){
        logPrint(LOG_DEBUG_PLUS, "\tno node (null)\n");
        return NULL;
    }
    node_t * node = newNode(buffer, sizeof(char) * BUF_LEN);
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

static void dumpTreeToFile(node_t * root_node, FILE * base_file, elemtostr_func_t dataToStr, size_t tab_nums)
{
    assert(base_file);
    assert(dataToStr != NULL);
    for(size_t tab = 0; tab < tab_nums; tab++)
        fputc('\t', base_file);
    if (root_node == NULL){
        fprintf(base_file, "null\n");
        return;
    }
    char str[BUF_LEN] = "";
    dataToStr(str, root_node->data);
    fprintf(base_file, FORMAT_TO_WRITE_STR, str);
    dumpTreeToFile(root_node->left , base_file, dataToStr, tab_nums + 1);
    dumpTreeToFile(root_node->right, base_file, dataToStr, tab_nums + 1);
}

void akinatorPtrToStr(char * str, void * str_ptr)
{
    char * akin_str = (char *) str_ptr;
    strcpy(str, akin_str);
}
