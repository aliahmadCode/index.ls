/*
 *  an example code for compiler
 * */
#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 100

#define PLUS '+'
#define MINUS '-'
#define MUL '*'
#define DIV '/'
#define MOD '%'
#define POWER '^'

// types that just somehow supported
#define INTEGER "int"
#define STRING "string"

typedef struct Node {
  char *line;
  struct Node *next;
} Node;

typedef struct Entry {
  char *key;
  char *value;
  char *type;
  struct Entry *next;
} Entry;

typedef struct HashTable {
  Entry *tables[MAX_SIZE];
} HashTable;

void readFromFile(const char *filepath, char **filebuffer);
void printList(Node *node);
Node *createNode(char *line);
void pushNode(Node **node, char *line);
void nodeListFill(Node **head, char *filebuffer);

// HashTable implementation
HashTable *createHashTable();
Entry *createEntry(char *key, char *value, char *type);
unsigned int hash(const char *key);
void insertHash(HashTable *tb, char *key, char *value, char *type);
char *search(HashTable *tb, const char *key);
void deleteHash(HashTable *tb, char *key);
void freeHashTables(HashTable *tb);
char *popNode(Node **node);
const unsigned int KEY_MAX_DEFUALT = 8;
const unsigned int VALUE_MAX_DEFUALT = 6;

// utility - main one
int is_all_digits(char *str);




//  main driver code at home
int main(int argc, char *argv[]) {

  char *filebuffer = NULL;
  Node *head = NULL;
  // after this function filebuffer should have the contents of the file
  readFromFile("./index.ls", &filebuffer);
  // put all the individual lines in linked list of var *head
  nodeListFill(&head, filebuffer);

  // the hash table implementation, var *tb
  HashTable *tb = createHashTable();
  printList(head);

  freeHashTables(tb);
  return EXIT_SUCCESS;
}


// function implementation to read all the lines from the file
void readFromFile(const char *filepath, char **filebuffer) {
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  size_t buffer_size = 128;
  size_t buffer_used = 0;

  char *linebuffer = (char *)malloc(buffer_size);
  if (linebuffer == NULL) {
    perror("Can't allocate the space for this string\n");
    exit(EXIT_FAILURE);
  }

  fp = fopen(filepath, "r");
  if (fp == NULL) {
    perror("File is corrupted or not even exists");
    exit(EXIT_FAILURE);
  }

  while ((read = getline(&line, &len, fp)) != -1) {
    if (buffer_used + read > buffer_size) {
      buffer_size += 128;
      linebuffer = (char *)realloc(linebuffer, buffer_size);
      if (linebuffer == NULL) {
        perror("Can't allocate the space for this string\n");
        free(line);
        fclose(fp);
        exit(EXIT_FAILURE);
      }
    }
    memcpy(linebuffer + buffer_used, line, read);
    buffer_used += read;
  }
  if (buffer_used >= buffer_size) {
    linebuffer = (char *)realloc(linebuffer, buffer_used + 1);
    if (linebuffer == NULL) {
      perror("Can't allocate the space for this string\n");
      free(line);
      fclose(fp);
      exit(EXIT_FAILURE);
    }
  }

  linebuffer[buffer_used] = '\0';
  *filebuffer = strdup(linebuffer);
  free(linebuffer);
  free(line);
  fclose(fp);
}

void printList(Node *node) {
  if (node == NULL) {
    printf("Our list is emtpy\n");
    return;
  }
  Node *temp = node;
  while (temp != NULL) {
    printf("%s\n", temp->line);
    temp = temp->next;
  }
  return;
}

Node *createNode(char *line) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->line = strdup(line);
  newNode->next = NULL;
  return newNode;
}

void pushNode(Node **node, char *line) {
  if (*node == NULL) {
    *node = createNode(line);
    return;
  }
  Node *temp = *node;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = createNode(line);
  return;
}

void nodeListFill(Node **head, char *filebuffer) {
  size_t count = 0, countl = 0;
  char *line;
  const unsigned int MAX_LINESIZE = 128;
  line = (char *)malloc(sizeof(char) * MAX_LINESIZE);

  while (filebuffer[count] != '\0') {
    if (filebuffer[count] == '\n') {
      if (countl == 0) {
        count++;
        continue;
      }
      countl += 1;
      line[countl] = '\0';
      // best line ever written
      char *temp = strdup(line);
      pushNode(head, temp);
      countl = 0;
      free(line);
      line = (char *)malloc(sizeof(char) * MAX_LINESIZE);
      count++;
      continue;
    }
    if (countl > MAX_LINESIZE) {
      line = (char *)realloc(line, countl + MAX_LINESIZE);
    }
    line[countl] = filebuffer[count];
    countl++;
    count++;
  }

  free(filebuffer);
}
HashTable *createHashTable() {
  HashTable *hb = (HashTable *)malloc(sizeof(HashTable));
  if (hb == NULL) {
    perror("Can't allocate memory for HashTable\n");
    exit(1);
  }
  for (unsigned int i = 0; i < MAX_SIZE; i++) {
    hb->tables[i] = NULL;
  }
  return hb;
}

Entry *createEntry(char *key, char *value, char *type) {
  Entry *ent = (Entry *)malloc(sizeof(Entry));
  if (ent == NULL) {
    perror("Can't allocate memory for HashTable\n");
    exit(1);
  }
  ent->key = strdup(key);
  ent->value = strdup(value);
  if (strcmp(type, INTEGER) == 0) {
    ent->type = INTEGER;
  } else if (strcmp(type, STRING) == 0) {
    ent->type = STRING;
  }
  ent->next = NULL;

  return ent;
}

unsigned int hash(const char *key) {
  unsigned int hash = 0;
  while (*key) {
    hash = (hash * 31) + *key++;
  }
  return hash % MAX_SIZE;
}

void insertHash(HashTable *tb, char *key, char *value, char *type) {
  unsigned int index = hash(key);
  Entry *entry = tb->tables[index];

  // check whether that key already exists
  while (entry != NULL) {
    if (strcmp(entry->key, key) == 0) {
      free(entry->value);
      entry->value = value;
      return;
    }
    entry = entry->next;
  }

  entry = createEntry(key, value, type);
  entry->next = tb->tables[index];
  tb->tables[index] = entry;
}

char *search(HashTable *tb, const char *key) {
  unsigned int index = hash(key);
  Entry *entry = tb->tables[index];

  while (entry != NULL) {
    if (strcmp(entry->key, key) == 0) {
      return entry->value;
    }
    entry = entry->next;
  }

  return NULL;
}

void deleteHash(HashTable *tb, char *key) {
  unsigned int index = hash(key);
  Entry *entry = tb->tables[index];
  Entry *prev = NULL;
  while (entry != NULL) {
    if (strcmp(entry->key, key) == 0) {
      if (prev) {
        prev->next = entry->next;
      } else {
        tb->tables[index] = entry->next;
      }
      free(entry->key);
      free(entry->value);
      free(entry);
      return;
    }
    prev = entry;
    entry = entry->next;
  }
}

void freeHashTables(HashTable *tb) {
  for (unsigned int i = 0; i < MAX_SIZE; i++) {
    Entry *entry = tb->tables[i];
    while (entry != NULL) {
      Entry *temp = entry;
      entry = entry->next;
      free(temp->value);
      free(temp->key);
      free(temp);
    }
  }
}
char *popNode(Node **node) {
  if (*node == NULL) {
    return NULL;
  }
  Node *temp = *node;
  *node = (*node)->next;
  // chat added this line
  temp->next = NULL; // i don't need this
  return temp->line;
}

int is_all_digits(char *str) {
  unsigned int i = 0;
  while (str[i] != '\0') {
    if (!isdigit((unsigned char)str[i])) {
      return 0;
    }
    i++;
  }
  return 1;
}
