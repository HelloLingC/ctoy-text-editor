#ifndef H_DOCUMENT
#define H_DOCUMENT
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *buffer;
  // length is the position of '\0' in buffer
  size_t length;
  // capacity is the number of allocated bytes in buffer including '\0'
  size_t capacity;
} Document;

bool document_init(Document *doc, size_t initial_capacity);
void document_destroy(Document *doc);
bool document_append_char(Document *doc, char c);
bool document_backspace(Document *doc);
bool document_append_string(Document *doc, const char *string);

#endif