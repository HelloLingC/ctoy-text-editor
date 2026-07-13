#include "document.h"

bool document_init(Document *doc, size_t initial_capacity) {
  if (initial_capacity == 0) {
    return false;
  }
  doc->buffer = malloc(initial_capacity);
  if (!doc->buffer) {
    printf("Cannot malloc memory for document buffer!");
    return false;
  }
  doc->buffer[0] = '\0';
  doc->length = 0;
  doc->capacity = initial_capacity;

  return true;
}

void document_destroy(Document *doc) {
  free(doc->buffer);
  //  free releases the allocation, but it does not modify the pointer holding
  //  its old address.
  doc->buffer = NULL;
  doc->length = 0;
  doc->capacity = 0;
}

void document_clear(Document *doc) {
  doc->buffer[0] = '\0';
  doc->length = 0;
}

// args: c should not be '\0'
bool document_append_char(Document *doc, char c) {
  if (c == '\0') {
    return false;
  }

  if (doc->length == doc->capacity - 1) {
    // TODO: capacity*2 could overflow size_t
    char *temp = realloc(doc->buffer, doc->capacity * 2);
    if (!temp) {
      return false;
    }
    doc->capacity = doc->capacity * 2;
    doc->buffer = temp;
  }
  doc->buffer[doc->length] = c;
  doc->buffer[doc->length + 1] = '\0';
  doc->length += 1;

  return true;
}
// args: string should not be NULL
bool document_append_string(Document *doc, const char *string) {
  if (string[0] == '\0') {
    return true;
  }

  size_t str_len = strlen(string);
  if (str_len > SIZE_MAX - doc->length - 1) {
    return false;
  }

  size_t needed_capacity = doc->length + 1 + str_len;
  if (needed_capacity > doc->capacity) {

    char *temp = realloc(doc->buffer, needed_capacity);
    if (!temp) {
      return false;
    }
    doc->buffer = temp;
    doc->capacity = needed_capacity;
  }
  strcpy(doc->buffer + doc->length, string);
  doc->length += str_len;
  return true;
}

bool document_backspace(Document *doc) {
  if (doc->length == 0) {
    return false;
  }
  doc->buffer[doc->length - 1] = '\0';
  doc->length -= 1;
  return true;
}

bool document_backspace_at(Document *doc, size_t pos) {
  if (doc->length == 0) {
    return false;
  }
  if (pos == 0) {
    return false;
  }
  if (pos > doc->length) {
    return false;
  }
  memmove(doc->buffer + pos - 1, doc->buffer + pos, doc->length - pos + 1);
  doc->length -= 1;
  return true;
}

bool document_insert_char(Document *doc, char c, size_t pos) {
  if (c == '\0') {
    return false;
  }
  if (pos > doc->length) {
    return false;
  }
  if (doc->length + 2 > doc->capacity) {
    char *temp = realloc(doc->buffer, doc->length + 2);
    if (!temp) {
      return false;
    }
    doc->buffer = temp;
    doc->capacity = doc->length + 2;
  }

  memmove(doc->buffer + pos + 1, doc->buffer + pos, doc->length - pos + 1);
  doc->buffer[pos] = c;
  doc->length++;

  return true;
}

bool document_insert_string(Document *doc, const char *string, size_t pos) {
  if (string[0] == '\0') {
    return false;
  }
  if (pos > doc->length) {
    return false;
  }
  size_t str_len = strlen(string);

  // subtraction avoids unsigned arithmetic wrapping
  if (doc->length + 1 > SIZE_MAX - str_len) {
    return false;
  }

  size_t needed_cap = doc->length + str_len + 1;
  if (needed_cap > doc->capacity) {
    char *temp = realloc(doc->buffer, needed_cap);
    if (!temp) {
      return false;
    }
    doc->buffer = temp;
    doc->capacity = needed_cap;
  }
  memmove(doc->buffer + pos + str_len, doc->buffer + pos,
          doc->length - pos + 1);
  memcpy(doc->buffer + pos, string, str_len);
  doc->length += str_len;
  return true;
}