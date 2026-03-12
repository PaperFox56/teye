/**
 * teye.h

MIT License

Copyright (c) 2026 PaperFox56

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


 */

#ifndef TEYE_H
#define TEYE_H

#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define TEYE_VERSION_MAJOR 0
#define TEYE_VERSION_MINOR 2


#define pixelcount(buffer) (buffer.width * buffer.height)

#define set_buffer_pixel(buf, x, y, color) buf.buffer[(x) + buf.width * (y)] = color
#define get_buffer_pixel(buf, x, y) buffer.buf[(x) + buf.width * (y)]

// Drawing modes
typedef enum { Sample, FitWidth, FitHeight, Stretch } DrawingMode;

typedef unsigned short ushort;

/**
Note 1: Unless it is absolutely necessary, it is advised not to manage this structure
manually (except perhaps for writing into the buffer). Please use the helper
functions and macros instead.

Note 2: To prevent undefined behaviour, always initialize the structure to {0};
*/
typedef struct {

  ushort width;
  ushort height;

  uint8_t *buffer;

  size_t capacity;

} TEYE_Buffer;

/* Initialize the terminal and the framebuffers */
void TEYE_init();

/*
Allocate a buffer with the given size.
If the buffer is already allocated, it reallocates it to fit the new size.

If the previous size was already sufficient, not allocation is needed.

Return: `-1` if the allocation was succesful, `0` otherwise
 */
int TEYE_allocate_buffer(TEYE_Buffer *buffer, int width, int height);

/* Fills a buffer with the provided number */
void TEYE_clear_buffer(TEYE_Buffer buffer, uint8_t color);

/* Deallocate a buffer. This function performs a check to prevent double free.
 * Calling it twice by mistake should not cause memory corruption.
 */
void TEYE_free_buffer(TEYE_Buffer *buffer);

/*
TODO: Add an averaging mode.
TODO2: This function does too much work, create a new one that doesn't take
scaling arguments
*/
/* Draw a bitmap from the given buffer to the internal one
The drawing mode is used to determine how the given bitmap is copied onto the
buffer. The bitmap is converted to a scaled version (using the scaling factors.)
and then placed at the right coordinates.
- `Sample`: For each destination pixel in the screen buffer, the source
coordinate is calculated and a single pixel is sampled.
- `FitWidth`: The scale parameters are ignored. The bitmap is scaled in a way
that it's width fits the screen.
- `FitHeight`: The scale parameters are ignored. The bitmap is scaled in a way
that it's height fits the screen.
- `Stretch`: The scale parameters are ignored. The bitmap is scaled in a way
that it's width fits the screen.
*/
void TEYE_blit(const TEYE_Buffer buffer, DrawingMode mode, int x, int y,
               float scale_x, float scale_y);

/*
 Renders the internal buffer to the screen.
 Note that the screen resize interrupt (SINWINCH) is only handled at the end of
 the render. That means that the rendering buffer is still valid
*/
void TEYE_render_frame();

/*
Deallocate the frame buffers and restore the terminal's state.
 */
void TEYE_free();

// Prints an error message and quit the program
void panic(const char *s);

#ifdef __cplusplus
}
#endif
#endif
