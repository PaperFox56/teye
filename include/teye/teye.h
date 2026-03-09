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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define TEYE_VERSION_MAJOR 0
#define TEYE_VERSION_MINOR 2

#define set_buffer_pixel(buffer, x, y, color) buffer.frame_buffer[x + buffer.width*y] = color
#define get_buffer_pixel(buffer, x, y) buffer.frame_buffer[x + buffer.width*y]

typedef unsigned short ushort;

/**
Please note that this buffer is managed internally by the library.
Never call `free` on it.
*/
typedef struct {

    ushort width;
    ushort height;
    // This buffer is written into by the user program
    uint8_t *frame_buffer; 

} TEYE_Buffer;


// Effectively erase the old frame and bring the current frame's buffer to be drawn on the screen
TEYE_Buffer TEYE_init(ushort width, ushort height);
void TEYE_render_frame_mode_1();

// Function to render the frame buffer, tries to fit the buffer's size to the
// screen's
// As buffer swapping is used, the function returns a pointer to the new buffer
TEYE_Buffer TEYE_render_frame_mode_2();
void TEYE_clear_buffer(uint8_t color);
void TEYE_free();

// Prints an error message and quit the program
void panic(const char *s);

#ifdef __cplusplus
}
#endif
#endif
