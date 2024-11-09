#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "akinator.h"
#include "binTree.h"

const size_t BUF_LEN = 128;
node_t * akinatorReadFromFile(FILE * base_file, node_t * parent)
{
    assert(base_file);
    char buffer[BUF_LEN] = "";

    fscanf(base_file, "%s", buffer);

    if (strcmp(buffer, "null") == 0)
        return NULL;

    char * node_str = (char *)calloc(BUF_LEN, sizeof(char));
    strcpy(node_str, buffer);
    node_t * node = newNode(&node_str, sizeof(char *));
    node->parent = parent;

    node->left  = akinatorReadFromFile(base_file, node);
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
