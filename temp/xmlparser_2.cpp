#include "xmlparser.h"


// ----- DEFINES ----- //
#define XML_UNKNOW				-1
#define XML_DUNNOHANDLE			-2
#define XML_OPEN_NODE			1
#define XML_CLOSE_NODE			2
#define XML_NODE_NAME			3
#define XML_SPECIAL_NODE		4
#define XML_NAME_DELIMITER		5
#define XML_ATTR_START			6
#define XML_ATTR_DELIMITER		7
#define XML_NODE_END			8
#define XML_CLOSE_END_NODE		9


// ----- STRUCTS ----- //
typedef struct xml_reader_{
	char reading_tag;
	char quote_read;
	char node_ending;

	int lvl;

	long cur_pos;

	char read_buffer[1024];
	char name_buffer[1024];
}xml_reader_;

xml_reader_ xml_reader = {0};

typedef struct ret_{
	int ret;
	char ch;
}ret_;


// ----- PROTOTYPES ----- //
void XML_FreeList( void );
struct ret_ XML_GetChar( void );
struct xml_node *XML_RecursiveSearchNode(char *name, struct xml_node *node);
void XML_RecursiveDump(struct xml_node *node);


// ----- VARIABLES -----//
char xml_path[1024];
FILE *dump_file = NULL;

char *xml_buffer = NULL;
long xml_size = 0;


struct xml_node *main_tree = NULL;
struct xml_node *current_node = NULL;

// ----- CODE ----- //

//! Abrir el archivo para parsear posteriormente
//! path: Ruta relativa o hard coded hacia el xml
//TODO: No guardar en buffer, leer char a char!
int XML_Open(char *path){
	FILE *fp = fopen(path, "r");
	if(!fp)
		return -1;

	strcpy(xml_path, path);

	fseek(fp, 0, SEEK_END);
	xml_size = ftell(fp);
	rewind(fp);

	xml_buffer = (char*)malloc(xml_size+1);
	if(!xml_buffer)
		return -2;

	fread(xml_buffer, 1, xml_size, fp);

	fclose(fp);

	return 0;
}


struct ret_ XML_GetChar(){
	char ch = xml_buffer[xml_reader.cur_pos++];
	struct ret_ ret;
	ret.ch = ch;

	if(xml_reader.quote_read == 1 && ch != '"'){
		ret.ret = XML_NODE_NAME;
		goto endfunc;
	}
	if(xml_reader.reading_tag == 0){
		switch(ch){
			case '<':
				xml_reader.reading_tag = 1;

				if(xml_buffer[xml_reader.cur_pos] == '/'){
					xml_reader.node_ending = 1;
					ret.ret = XML_NODE_END;
				}else{					
					xml_reader.node_ending = 0;
					ret.ret = XML_OPEN_NODE;
				}

				goto endfunc;
			default:
				ret.ret = XML_UNKNOW;
				goto endfunc;
		}
	}else{
		if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')){
			ret.ret = XML_NODE_NAME;
			goto endfunc;
		}else{
			switch(ch){
				case '>':		
					xml_reader.reading_tag = 0;
					if(xml_buffer[xml_reader.cur_pos-2] == '/')
						ret.ret = XML_CLOSE_END_NODE;
					else
						ret.ret = XML_CLOSE_NODE;
					goto endfunc;
				case '?':
					ret.ret = XML_SPECIAL_NODE;
					goto endfunc;					
				case ' ':
					ret.ret = XML_NAME_DELIMITER;
					goto endfunc;		
				case '=':
					ret.ret = XML_ATTR_START;
					goto endfunc;	
				case '"':
					xml_reader.quote_read = !xml_reader.quote_read;
					ret.ret = XML_ATTR_DELIMITER;
					goto endfunc;	
			}
		}
	}


endfunc:
	return ret;
}

struct xml_node *XML_ParseAll(){
	char action = 0;
	char ignore = 0;
	int buffer_pos = 0;

	memset(xml_reader.read_buffer, 0, 1024);
	memset(xml_reader.name_buffer, 0, 1024);
	xml_reader.reading_tag = 0;
	xml_reader.quote_read = 0;

	struct xml_node_attr *last_attr = NULL;
	struct xml_node_attr *first_attr = NULL;

	while(xml_reader.cur_pos < xml_size){
		struct ret_ ret = XML_GetChar();
		
		if(ignore && ret.ret != XML_SPECIAL_NODE)
			continue;

		switch(ret.ret){
			case XML_SPECIAL_NODE:
				ignore = !ignore;
				break;
			case XML_OPEN_NODE:
				
				if(!main_tree){
					main_tree = current_node = (struct xml_node*)malloc(sizeof(struct xml_node));					
					memset(current_node, 0, sizeof(struct xml_node));
				}else if(current_node->closed){
					current_node->next = (struct xml_node*)malloc(sizeof(struct xml_node));
					memset(current_node->next, 0, sizeof(struct xml_node));
					current_node->next->parent = current_node->parent;
					current_node->next->last = current_node;
					current_node = current_node->next;
				}else{
					current_node->child = (struct xml_node*)malloc(sizeof(struct xml_node));
					memset(current_node->child, 0, sizeof(struct xml_node));
					current_node->child->parent = current_node;
					current_node = current_node->child;
				}

				action = 0;
				break;
			case XML_NODE_NAME:
				xml_reader.read_buffer[buffer_pos++] = ret.ch;
				break;
			case XML_NAME_DELIMITER:{
				if(buffer_pos > 0){
					
					if(action == 0)
						strcpy(xml_reader.name_buffer, xml_reader.read_buffer);

					memset(xml_reader.read_buffer, 0, 1024);
					buffer_pos = 0;

				}
			}break;

			case XML_ATTR_START:{
				struct xml_node_attr *temp_attr = (struct xml_node_attr*)malloc(sizeof(struct xml_node_attr));
				memset(temp_attr, 0, sizeof(struct xml_node_attr));

				temp_attr->name = (char*)malloc(buffer_pos);
				strcpy(temp_attr->name, xml_reader.read_buffer);
							
				if(last_attr != NULL)
					last_attr->next = temp_attr;
				last_attr = temp_attr;

				if(first_attr == NULL)
					first_attr = last_attr;

				memset(xml_reader.read_buffer, 0, 1024);
				buffer_pos = 0;
			}break;

			case XML_ATTR_DELIMITER:
				if(xml_reader.quote_read == 0){

					last_attr->value = (char*)malloc(buffer_pos);
					strcpy(last_attr->value, xml_reader.read_buffer);
				
					memset(xml_reader.read_buffer, 0, 1024);
					buffer_pos = 0;
				}
				break;

			case XML_CLOSE_END_NODE:
			case XML_CLOSE_NODE:
				if(xml_reader.node_ending == 0){
					current_node->name = (char*)malloc(buffer_pos);
					if(strlen(xml_reader.name_buffer) > 0)
						strcpy(current_node->name, xml_reader.name_buffer);
					else
						strcpy(current_node->name, xml_reader.read_buffer);

					current_node->level = xml_reader.lvl;
					current_node->attrs = first_attr;											
					current_node->start_pos = xml_reader.cur_pos;
				}					
												
				memset(xml_reader.name_buffer, 0, 1024);
				memset(xml_reader.read_buffer, 0, 1024);
				buffer_pos = 0;

				first_attr = NULL;
				last_attr = NULL;

				if(ret.ret == XML_CLOSE_END_NODE){
					current_node->closed = 1;
					current_node->end_pos = xml_reader.cur_pos;
				}

				break;

			case XML_NODE_END:
				while(current_node->closed == 1)
					current_node = current_node->parent;

				current_node->closed = 1;
				current_node->end_pos = xml_reader.cur_pos;

				memset(xml_reader.name_buffer, 0, 1024);
				memset(xml_reader.read_buffer, 0, 1024);
				buffer_pos = 0;

				if(!current_node->child)
					XML_GetNodeValue(current_node);

				break;

			default:
				break;
		}
	}
	
	return main_tree;
}

struct xml_node *XML_RecursiveSearchNode(char *name, struct xml_node *node){
	if(strcmp(name, node->name) == 0)
		return node;

	struct xml_node *ret = NULL;

	if(node->child)
		ret = XML_RecursiveSearchNode(name, node->child);

	if(ret)
		return ret;

	if(node->next)
		ret = XML_RecursiveSearchNode(name, node->next);
		
	if(ret)
		return ret;

	return NULL;
}

struct xml_node *XML_SearchNodeFrom(char *name, struct xml_node *start){
	return XML_RecursiveSearchNode(name, start);
}
struct xml_node *XML_SearchNode(char *name){
	return XML_RecursiveSearchNode(name, main_tree);
}

struct xml_node *XML_GetChild(struct xml_node *node){
	return node->child;
}

struct xml_node *XML_GetNextNode(struct xml_node *node){
	return node->next;
}

struct xml_node_attr *XML_GetNodeAttr(struct xml_node *node, char *name){
	struct xml_node_attr *iterator = node->attrs;
	while(iterator != NULL){
		if(strcmp(iterator->name, name) == 0)
			return iterator;
		iterator = iterator->next;
	}

	return NULL;
}


char XML_InsertNodeAfter(struct xml_node *level, struct xml_node* node){
	if(!level || !node)
		return -1;

	struct xml_node *temp = level->next;
	level->next = node;
	node->last = level;
	node->next = temp;
	node->parent = level->parent;

	return 0;
}

char XML_InsertChildNodeFirst(struct xml_node *parent, struct xml_node* node){
	if(!parent || !node)
		return -1;

	struct xml_node *temp = parent->child;
	node->parent = parent;
	node->next = temp;
	if(temp)
		temp->last = node;
	parent->child = node;

	return 0;
}

char XML_InsertChildNodeLast(struct xml_node *parent, struct xml_node* node){
	if(!parent || !node)
		return -1;

	struct xml_node *temp = parent->child;
	while(temp->next)
		temp = temp->next;
	
	XML_InsertNodeAfter(temp, node);

	return 0;
}

char *XML_GetNodeValue(struct xml_node *node){
	if(node->value == NULL){
		int len = node->end_pos - node->start_pos - 1;
		if(len <= 0) return NULL;

		char *buffer =(char*)malloc(len);
		memcpy(buffer, &xml_buffer[node->start_pos], len);
		buffer[len] = '\0';
		node->value = buffer;
	}
	return node->value;
}

void XML_RecursiveFreeNode(xml_node *node){
	if(node->child)
		XML_RecursiveFreeNode(node->child);

	if(node->next)
		XML_RecursiveFreeNode(node->next);

	free(node);
	node = NULL;
}

void XML_RecursiveDump(struct xml_node *node){
	char *buffer = (char*)malloc(1024);
	memset(buffer, 0, 1024);

	sprintf(buffer, "<%s", node->name);

	xml_node_attr *attr = node->attrs;
	
	while(attr){
		strcat(buffer, " ");
		strcat(buffer, attr->name);
		strcat(buffer, "=");
		strcat(buffer, "\"");
		strcat(buffer, attr->value);
		strcat(buffer, "\"");

		attr = attr->next;
	}
	strcat(buffer, ">");
	
	fputs(buffer, dump_file);
	memset(buffer, 0, 1024);

	if(node->child)
		XML_RecursiveDump(node->child);
	else if(node->value)
		strcat(buffer, node->value);

	strcat(buffer, "</");
	strcat(buffer, node->name);
	strcat(buffer, ">");

	fputs(buffer, dump_file);
	memset(buffer, 0, 1024);

	if(node->next)
		XML_RecursiveDump(node->next);
}

char XML_Dump(char *file, struct xml_node *main){
	dump_file = fopen(file, "w+");
	if(!dump_file)
		return -1;

	XML_RecursiveDump(main);

	fclose(dump_file);

	return 0;
}

//! Liberar la memoria ocupada por xml_buffer
void XML_EndParse(char free_tree){
	free(xml_buffer);

	if(free_tree)
		XML_RecursiveFreeNode(main_tree);

	main_tree = current_node = NULL;
}
