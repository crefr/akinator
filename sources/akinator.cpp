#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "akinator.h"
#include "binTree.h"
#include "logger.h"

const size_t BUF_LEN = 128;

void akinatorCtor(akinator_t * akinator, FILE * base_file)
{
    assert(akinator);
    assert(base_file);
    logPrint(LOG_DEBUG_PLUS, "akinator: constructing akinator...\n");
    akinator->root = akinatorReadFromFile(base_file, NULL);
    akinator->cur_node = akinator->root;
    logPrint(LOG_DEBUG_PLUS, "akinator: constructed akinator (root = %p, cur_node = %p)\n",
            akinator->root, akinator->cur_node);
}

void akinatorDtor(akinator_t * akinator)
{
    assert(akinator);
    logPrint(LOG_DEBUG_PLUS, "akinator: destructing akinator...\n");
    akinatorTreeDtor(akinator->root);
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

    char * node_str = (char *)calloc(BUF_LEN, sizeof(char));
    strcpy(node_str, buffer);
    node_t * node = newNode(&node_str, sizeof(char *));
    node->parent = parent;
    logPrint(LOG_DEBUG_PLUS, "\tadded node %p (str = \"%s\")\n", node, node_str);

    logPrint(LOG_DEBUG_PLUS, "\tadding left node to %p (str = \"%s\")...\n", node, node_str);
    node->left  = akinatorReadFromFile(base_file, node);
    logPrint(LOG_DEBUG_PLUS, "\tadding right node to %p (str = \"%s\")...\n", node, node_str);
    node->right = akinatorReadFromFile(base_file, node);
    return node;
}

void akinatorTreeDtor(node_t * root_node)
{
    if (root_node == NULL)
        return;
    char * node_str = *((char **)root_node->data);
    free(node_str);

    akinatorTreeDtor(root_node->left);
    akinatorTreeDtor(root_node->right);

    delNode(root_node);
}

void akinatorPtrToStr(char * str, void * str_ptr)
{
    char * akin_str = *((char **) str_ptr);
    printf("%s\n", akin_str);
    strcpy(str, akin_str);
}
