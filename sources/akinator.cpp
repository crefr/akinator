#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "akinator.h"
#include "binTree.h"
#include "logger.h"

static void akinatorDumpTreeToFile(node_t * root_node, FILE * base_file, elemtostr_func_t dataToStr, size_t tab_nums);

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

node_t * akinatorReadFromFile(FILE * base_file, node_t * parent)
{
    assert(base_file);
    logPrint(LOG_DEBUG_PLUS, "akinator: reading token (parent = %p)\n", parent);
    char buffer[BUF_LEN] = "";

    fscanf(base_file, "%s", buffer);

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
    akinatorDumpTreeToFile(akinator->root, base_file, akinator->dataToStr, 0);
    logPrint(LOG_DEBUG_PLUS, "akinator: dumped to file\n");
}

static void akinatorDumpTreeToFile(node_t * root_node, FILE * base_file, elemtostr_func_t dataToStr, size_t tab_nums)
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
    akinatorDumpTreeToFile(root_node->left , base_file, dataToStr, tab_nums + 1);
    akinatorDumpTreeToFile(root_node->right, base_file, dataToStr, tab_nums + 1);
}

void akinatorPtrToStr(char * str, void * str_ptr)
{
    char * akin_str = (char *) str_ptr;
    printf("%s\n", akin_str);
    strcpy(str, akin_str);
}
