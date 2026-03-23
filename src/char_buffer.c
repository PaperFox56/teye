#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <teye/char_buffer.h>

extern void panic(const char *s);

typedef struct FileBuffer FileBuffer;
typedef struct CharBuffer CharBuffer;

int CharBuffer_init(struct CharBuffer *char_buffer) {
  void *new = malloc(16 * sizeof(char));
  if (new == NULL) {
    return -1;
  }

  char_buffer->buf = (char *)new;
  char_buffer->capacity = 16;
  char_buffer->len = 0;

  char_buffer->buf[0] = '\0';

  return 0;
}

int CharBuffer_grow(struct CharBuffer *char_buffer, size_t needed) {
  if (char_buffer->len + needed + 1 <= char_buffer->capacity) {
    // We are chill
    return 0;
  }

  if (char_buffer->capacity < 16) {
    panic("CharBuffer_grow: WTF?? ");
  }

  // grow by doubling the capacity
  size_t new_capacity = char_buffer->capacity;
  while (new_capacity < char_buffer->len + needed + 1)
    new_capacity *= 2;

  // the reallocation can fail
  char *new_buf = realloc(char_buffer->buf, new_capacity);
  if (!new_buf)
    return -1;

  char_buffer->buf = new_buf;
  char_buffer->capacity = new_capacity;

  return 0;
}

void CharBuffer_append_text(CharBuffer *char_buffer, const char *s,
                            size_t len) {
  if (CharBuffer_grow(char_buffer, len) == -1)
    panic("CharBuffer_append_text, increasing buffer capacity");

  memcpy(&char_buffer->buf[char_buffer->len], s, len);

  char_buffer->len += len;
  char_buffer->buf[char_buffer->len] = '\0';
}

void CharBuffer_insert_text(struct CharBuffer *char_buffer, const char *s,
                            size_t index, size_t len) {

  if (index > char_buffer->len)
    return;

  if (CharBuffer_grow(char_buffer, len) == -1)
    panic("CharBuffer_insert_text, increasing buffer capacity");

  // first make some room for the new text
  size_t end = index + len;
  if (end > char_buffer->len)
    end = char_buffer->len;

  size_t count = char_buffer->len - end;
  if (count > 0)
    memmove(&char_buffer->buf[end], &char_buffer->buf[index], count);

  // copy the text
  memcpy(&char_buffer->buf[index], s, len);

  char_buffer->len += len;
  char_buffer->buf[char_buffer->len] = '\0';
}

/**
 * Fast integer to string conversion
 */
void CharBuffer_append_int(struct CharBuffer *char_buffer, int n) {
  if (n == 0) {
    CharBuffer_append_text(char_buffer, "0", 1);
    return;
  }

  char tmp[12]; // Enough for a 32-bit int
  int i = 10;
  tmp[11] = '\0';

  while (n > 0 && i >= 0) {
    tmp[i--] = (n % 10) + '0';
    n /= 10;
  }

  // i+1 is the start of our string
  CharBuffer_append_text(char_buffer, &tmp[i + 1], 10 - i);
}

void CharBuffer_remove_chars(struct CharBuffer *char_buffer, size_t index,
                             size_t len) {
  /* We will simply move part of the character to fill the gap left by the
   * deleted text */

  // some bound checking to be safe
  if (index >= char_buffer->len)
    return;

  if (len > char_buffer->len - index)
    len = char_buffer->len - index;

  // number of bytes to be copied
  size_t count = char_buffer->len - (index + len);
  memmove(&char_buffer->buf[index], &char_buffer->buf[index + len], count);

  char_buffer->len -= len;
}

void CharBuffer_free(CharBuffer *char_buffer) {

  if (char_buffer == NULL)
    return;

  if (char_buffer->buf)
    free(char_buffer->buf); // free the internal buffer

  char_buffer->buf = NULL;
  char_buffer->len = 0;
}
