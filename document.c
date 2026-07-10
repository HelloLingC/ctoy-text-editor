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