/*
 *  an example code for compiler
 *  i am personally a very bad programmer, so i am trying to write some code
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
// the stupid core
void interpreter(Node **head, HashTable *tb);

int evaluate_answer(char operator, char * value_a, char *value_b, char *key,
                    char *line) {
  int answer = 0;
  switch (operator) {
  case PLUS:
    answer = atoi(value_a) + atoi(value_b);
    break;
  case MINUS:
    answer = atoi(value_a) - atoi(value_b);
    break;
  case MUL:
    answer = atoi(value_a) * atoi(value_b);
    break;
  case DIV:
    if (atoi(value_b) == 0) {
      fprintf(stderr, "Can't divide by 0\n");
      free(key);
      free(value_a);
      free(value_b);

      exit(EXIT_FAILURE);
    }

    answer = atoi(value_a) / atoi(value_b);
    break;
  case MOD:
    if (atoi(value_b) == 0) {
      fprintf(stderr, "Can't take a mod by 0\n");
      free(key);
      free(value_a);
      free(value_b);

      exit(EXIT_FAILURE);
    }
    answer = atoi(value_a) % atoi(value_b);
    break;

  case POWER:
    answer = pow((double)atoi(value_a), (double)atoi(value_b));
    break;

  default:
    fprintf(stderr, "Key length exceeds limit (%u): %s\n", KEY_MAX_DEFUALT,
            key);
    free(key);
    free(value_a);
    free(value_b);
    exit(EXIT_FAILURE);
  }
  return answer;
}

void interpreter(Node **head, HashTable *tb) {
  char *line = popNode(head);

  while (line != NULL) {
    // some stupid things
    unsigned int i = 0;
    unsigned int j = 0;
    // some values
    char *key = (char *)calloc(KEY_MAX_DEFUALT, sizeof(char));
    char *value_a = (char *)calloc(VALUE_MAX_DEFUALT, sizeof(char));
    char *value_b = (char *)calloc(VALUE_MAX_DEFUALT, sizeof(char));
    char operator= ' ';

    // flags
    int analyzer_print_flag = 0;
    int analyzer_flag = 0;
    int analyzer_flag_values = 0;

    while (line[i] != '\0') {
      if (line[i] == 'l' && line[i + 1] == 'e' && line[i + 2] == 't' &&
          line[i + 3] == ' ') {
        i += 4;
        continue;
      }

      if (line[i] == 'p' && line[i + 1] == 'r' && line[i + 2] == 'i' &&
          line[i + 3] == 'n' && line[i + 4] == 't' && line[i + 5] == ' ') {
        analyzer_print_flag = 1;
        i += 6;
        break;
      }
      if (line[i] == '=') {
        j = 0;
        analyzer_flag = 1;
        i++;
        continue;
      }
      if (line[i] == ';') {
        key[strlen(key)] = '\0';
        value_a[strlen(value_a)] = '\0';
        char *putting_result = (char *)malloc(sizeof(char) * 10);
        // printf("** %s %s %s %d\n", key, value_a, value_b,
        // analyzer_flag_values);
        if (analyzer_flag == 1) {
          value_b[strlen(value_b)] = '\0';
          int answer = 0;
          if (operator!= ' ' && analyzer_flag_values == 1) {
            char *var1 = search(tb, value_a);
            char *var2 = search(tb, value_b);
            // printf("%s %s\n", var1, var2);
            if (var1 && is_all_digits(var1) && var2 && is_all_digits(var2)) {
              answer = evaluate_answer(operator, var1, var2, key, line);
              sprintf(putting_result, "%d", answer);
              insertHash(tb, strdup(key), strdup(putting_result), INTEGER);
            } else if (is_all_digits(value_a) && is_all_digits(value_b)) {
              // printf("%s %s %c\n", value_a, value_b, operator);
              answer = evaluate_answer(operator, value_a, value_b, key, line);
              sprintf(putting_result, "%d", answer);
              // printf("%s %d %s\n", key, answer, putting_result);
              insertHash(tb, strdup(key), strdup(putting_result), INTEGER);
              answer = 0;
            } else {
              if (operator== PLUS) {
                strcat(value_a, value_b);
                insertHash(tb, strdup(key), strdup(value_a), STRING);
              } else {
                fprintf(stderr,
                        "You have to learn this greate programming language\n");
                free(key);
                free(value_a);
                free(value_b);
                exit(EXIT_FAILURE);
              }
            }
          } else {
            if (is_all_digits(value_a)) {
              insertHash(tb, strdup(key), strdup(value_a), INTEGER);
            } else {
              insertHash(tb, strdup(key), strdup(value_a), STRING);
            }
          }
        }
        free(putting_result);
        analyzer_flag = 0;
        analyzer_print_flag = 0;
        analyzer_flag_values = 0;
        operator= ' ';
        break;
      }

      if (line[i] == ' ') {
        i++;
        continue;
      }

      if (line[i] == PLUS || line[i] == MINUS || line[i] == MUL ||
          line[i] == DIV || line[i] == MOD || line[i] == POWER) {
        operator= line[i];
        i++;
        j = 0;
        analyzer_flag_values = 1;
        continue;
      }

      if (analyzer_flag == 0) {
        if (j < KEY_MAX_DEFUALT - 1) {
          key[j] = line[i];
          j++;
          i++;
        } else {
          fprintf(stderr, "Key length exceeds limit (%u): %s\n",
                  KEY_MAX_DEFUALT, key);

          free(key);
          free(value_a);
          free(value_b);
          exit(EXIT_FAILURE);
        }
      } else {
        if (analyzer_flag_values == 0) {
          if (j < VALUE_MAX_DEFUALT - 1) {
            value_a[j] = line[i];
            j++;
            i++;
          } else {
            fprintf(stderr, "Value length exceeds limit (%u): %s\n",
                    VALUE_MAX_DEFUALT, key);
            free(key);
            free(value_a);
            free(value_b);
            exit(EXIT_FAILURE);
          }
        } else {
          if (j < VALUE_MAX_DEFUALT - 1) {
            value_b[j] = line[i];
            j++;
            i++;
          } else {
            fprintf(stderr, "Value length exceeds limit (%u): %s\n",
                    VALUE_MAX_DEFUALT, key);
            free(key);
            free(value_b);
            exit(EXIT_FAILURE);
          }
        }
      }
    }

    // for print
    if (analyzer_print_flag == 1) {
      j = 0;
      while (line[i] != '\0') {
        if (line[i] == ';') {
          key[j] = '\0';
          char *result = search(tb, key);
          if (result) {
            printf(" %s\n", result);
          } else {
            fprintf(stderr, "Key not found: %s\n", key);
          }
          break;
        }
        if (line[i] != ' ') {
          key[j] = line[i];
        }
        i++;
        j++;
      }
    }
    free(key);
    free(value_a);
    free(value_b);
    line = popNode(head);
  }
}

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

  // printList(head);
  interpreter(&head, tb);

  freeHashTables(tb);

  return EXIT_SUCCESS;
}

// NOTE: Garbage starts from here

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
