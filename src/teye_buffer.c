#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <teye/teye.h>


int TEYE_allocate_buffer(TEYE_Buffer *buffer, int width, int height) {

  if (width <= 0 || height <= 0) {
    const char *error_text =
        "Can't create a buffer with non-positive dimensions";
    write(STDERR_FILENO, error_text, strlen(error_text));
    return -1;
  }

  uint8_t *buf = NULL;

  if (buffer->buffer == NULL) {
    buf = (uint8_t *)malloc(sizeof(uint8_t) * width * height);
  } else {
    // I the buffer was already valid, we need to check it size. If the buffer
    // was already big enough, we don't need to allocate it again.
    if (width * height > buffer->width * buffer->height)
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

  return 0;
}

void TEYE_free_buffer(TEYE_Buffer *buffer) {
  if (buffer->buffer == NULL)
    return;

  free(buffer->buffer);
  buffer->buffer = NULL;
}

void TEYE_clear_buffer(TEYE_Buffer buffer, uint8_t color) {
  memset(buffer.buffer, color, pixelcount(buffer));
}