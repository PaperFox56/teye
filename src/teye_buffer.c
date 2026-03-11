#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <teye/teye.h>


#define max(a, b) (a > b ? a : b)


int TEYE_allocate_buffer(TEYE_Buffer *buffer, int width, int height) {

  if (width <= 0 || height <= 0) {
    const char *error_text =
        "Can't create a buffer with non-positive dimensions";
    write(STDERR_FILENO, error_text, strlen(error_text));
    return -1;
  }

  size_t needed = width * height;

  uint8_t *buf = NULL;

  if (buffer->buffer == NULL) {
    buffer->capacity = 0;
    buf = (uint8_t *)malloc(sizeof(uint8_t) * needed);
  } else {
    // I the buffer was already valid, we need to check it size. If the buffer
    // was already big enough, we don't need to allocate it again.
    if (buffer->capacity < needed)
      buf =
          (uint8_t *)realloc(buffer->buffer, sizeof(uint8_t) * width * height);
    else
      buf = buffer->buffer;
  }

  if (buf == NULL)
    return -1;

  buffer->width = width;
  buffer->height = height;
  buffer->buffer = buf;
  buffer->capacity = max(needed, buffer->capacity);

  return 0;
}

void TEYE_free_buffer(TEYE_Buffer *buffer) {
  if (buffer->buffer == NULL)
    return;

  free(buffer->buffer);
  buffer->buffer = NULL;
  buffer->capacity = 0;
}

void TEYE_clear_buffer(TEYE_Buffer buffer, uint8_t color) {
  memset(buffer.buffer, color, pixelcount(buffer));
}