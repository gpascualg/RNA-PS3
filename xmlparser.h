#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

typedef struct xml_node_attr{
	char *name;
	char *value;
	
	struct xml_node_attr *next;
}xml_node_attr;

typedef struct xml_node{
	char *name;
	char *value;
	int level;
	long start_pos;
	long end_pos;
	char closed;


	struct xml_node_attr *attrs;
	struct xml_node *next;
	struct xml_node *last;
	struct xml_node *child;
	struct xml_node *parent;
}xml_node;

int XML_Open(char *path);

struct xml_node *XML_ParseAll();
struct xml_node *XML_SearchNodeFrom(char *name, struct xml_node *start);
struct xml_node *XML_SearchNode(char *name);
struct xml_node *XML_GetChild(struct xml_node *node);
struct xml_node *XML_GetNextNode(struct xml_node *node);

char XML_InsertNodeAfter(struct xml_node *level, struct xml_node* node);
char XML_InsertChildNodeFirst(struct xml_node *parent, struct xml_node* node);
char XML_InsertChildNodeLast(struct xml_node *parent, struct xml_node* node);

struct xml_node_attr *XML_GetNodeAttr(struct xml_node *node, char *name);
char *XML_GetNodeValue(struct xml_node *node);

char XML_Dump(char *file, xml_node *main);

void XML_EndParse(char free_tree);

#endif