#ifndef H_DOCUMENT
#define H_DOCUMENT
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *buffer;
  // length is the position of '\0' in buffer, starts from 0
  size_t length;
  // capacity is the number of allocated bytes in buffer including '\0'
  size_t capacity;
} Document;

bool document_init(Document *doc, size_t initial_capacity);
void document_destroy(Document *doc);
void document_clear(Document *doc);
bool document_append_char(Document *doc, char c);
bool document_backspace(Document *doc);
bool document_backspace_at(Document *doc, size_t pos);
bool document_append_string(Document *doc, const char *string);
bool document_insert_char(Document *doc, char c, size_t pos);
bool document_insert_string(Document *doc, const char *string, size_t pos);

#endif