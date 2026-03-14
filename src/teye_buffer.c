/**
 * teye_buffer.c

Copyright (c) 2026 PaperFox56

This file is part of the teye library source code.
See the LICENCE section for details.

 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <teye/teye.h>

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

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

void TEYE_blit_copy_to(TEYE_Buffer dest, const TEYE_Buffer src, int x, int y) {
  // Clip the bitmap so that only the pixels that are actually on the screen
  // space are considered
  int min_y = max(y, 0);
  int max_y = min(y + src.height, dest.height);
  int min_x = max(x, 0);
  int max_x = min(x + src.width, dest.width);

  int src_row_offset = max(0, -y) * src.width;
  int dest_row_offset = min_y * dest.width;

  for (int dest_y = min_y; dest_y < max_y; dest_y++) {

    // We can let the standard library do the copy for us and blame them if
    // there is an issue
    memcpy(dest.buffer + dest_row_offset, src.buffer + src_row_offset,
           max_x - min_x);

    src_row_offset += src.width;
    dest_row_offset += dest.width;
  }
}

void TEYE_blit_and_scale_to(TEYE_Buffer dest, const TEYE_Buffer src, int x,
                            int y, float scale_x, float scale_y) {

  // Calculate the target dimensions in the internal buffer
  int target_w = (int)(src.width * scale_x);
  int target_h = (int)(src.height * scale_y);

  // Clip the bitmap so that only the pixels that are actually on the screen
  // space are considered
  int min_y = max(y, 0);
  int max_y = min(y + target_h, dest.height);
  int min_x = max(x, 0);
  int max_x = min(x + target_w, dest.width);

  // Fixed-Point Setup (using 16.16 shift)
  int step_x = (int)((1.0f / scale_x) * 65536);
  int step_y = (int)((1.0f / scale_y) * 65536);

  // Calculate initial source positions based on clipping
  int initial_sx = (min_x - x) * step_x;
  int initial_sy = (min_y - y) * step_y;

  int sy_fixed = initial_sy;

  for (int actual_y = min_y; actual_y < max_y; actual_y++) {
    int isy = sy_fixed >> 16; // Shift right to get the integer part
    if (isy >= src.height)
      isy = src.height - 1;

    int src_row_offset = isy * src.width;
    int dest_row_offset = actual_y * dest.width;

    int sx_fixed = initial_sx;

    for (int actual_x = min_x; actual_x < max_x; actual_x++) {
      int isx = sx_fixed >> 16;
      if (isx >= src.width)
        isx = src.width - 1;

      uint8_t pixel = src.buffer[src_row_offset + isx];
      if (pixel != 0) {
        dest.buffer[dest_row_offset + actual_x] = pixel;
      }
      sx_fixed += step_x;
    }
    sy_fixed += step_y;
  }
}

void TEYE_blit_custom_scale_to(TEYE_Buffer dest, const TEYE_Buffer src,
                               ScalingMode mode, int x, int y) {

  float scale_x = 1.0;
  float scale_y = 1.0;

  float w_ratio = (float)dest.width / src.width;
  float h_ratio = (float)dest.height / src.height;

  switch (mode) {
  case FitWidth:
    scale_x = w_ratio;
    scale_y = w_ratio;
    break;
  case FitHeight:
    scale_y = h_ratio;
    scale_x = h_ratio;
    break;
  case Stretch:
    scale_x = w_ratio;
    scale_y = h_ratio;
    break;
  }

  TEYE_blit_and_scale_to(dest, src, x, y, scale_x, scale_y);
}
