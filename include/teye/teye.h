/**
 @file teye.h

Copyright (c) 2026 PaperFox56

This file is part of the teye library source code.
See the LICENCE section for details.

 */

#ifndef TEYE_H
#define TEYE_H

#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define TEYE_VERSION_MAJOR 0
#define TEYE_VERSION_MINOR 3

#define pixelcount(buffer) (buffer.width * buffer.height)

// Drawing modes
typedef enum { FitWidth, FitHeight, Stretch, FitBest } ScalingMode;
// Intended feature implementation
typedef enum {
  Normal,
  Average,
  FindAGoodNameForThisOne // You know, the one were you take the most present
                          // color in a region
} SamplingMode;

typedef int (*TEYE_ResizeCallback)(unsigned short, unsigned short);

/**
Note 1: Unless it is absolutely necessary, it is advised not to manage this
structure manually (except for writing into the buffer). Please use
the helper functions and macros instead.

Note 2: To prevent undefined behaviour, always initialize the structure to
{0} before requesting the first allocation.
*/
typedef struct {

  unsigned short width;
  unsigned short height;

  uint8_t *buffer;

  size_t capacity;

} TEYE_Buffer;

/* Initialize the terminal and the framebuffers */
void TEYE_init();

/* NON-IMPLEMENTED!
 * Gives Teye a callback that will be called on window resize events. That
 * function has to be of the type `TEYE_ResizeCallback`.
 * It should take as arguments the new size of the rendering window and return a
 * flag number dictating how the event will be handled. Each bit of the flag
 * control one particular behaviour. Please refer to the macros for more
 * information.*/
void TEYE_set_resize_callback(TEYE_ResizeCallback callback);

/*
Allocate a buffer with the given size.
If the buffer is already allocated, it reallocates it to fit the new size.

If the previous size was already sufficient, not allocation is needed.

Return: `0` if the allocation was succesful, `-1` otherwise
 */
int TEYE_allocate_buffer(TEYE_Buffer *buffer, int width, int height);

/** Fills a buffer with the provided number */
void TEYE_clear_buffer(TEYE_Buffer buffer, uint8_t color);

/* Deallocate a buffer. This function performs a check to prevent double free.
 * Calling it twice by mistake should not cause memory corruption.
 */
void TEYE_free_buffer(TEYE_Buffer *buffer);

/*Returns one of the internal frame buffer. Note that since this is not a
 * pointer, this buffer WILL become invalid after every frame redraw (or if the
 * framebuffers are manually updated, this is feature that will probably be
 * implemented for the 0.4.0 version).
 `0` - The front framebuffer (the one which is actively been drawn on and will
 be rendered on the next request)
 `1` - The back framebuffer (hold the previous frame)
 Any other argument is ignored and will return an invalid buffer.
 */
TEYE_Buffer TEYE_get_framebuffer(int index);

/**
This function draws a bitmap from the given buffer to the internal one.
This is a shortcut for
`TEYE_blit_custom_scale_to(TEYE_get_framebuffer(0), src, mode, x, y);`
*/
void TEYE_blit(const TEYE_Buffer src, ScalingMode mode, int x, int y);

/*
Copy a bitmap from the source buffer to the destination without applying any
scaling. This is the same as using `TEYE_blit_and_scale_to` with 1 for the
scaling arguments, except it's faster since no expensive calculation is done and
the memory can be copied using memcpy.
 */
void TEYE_blit_copy_to(TEYE_Buffer dest, const TEYE_Buffer src, int x, int y);

/*Draw a bitmap from the source buffer to the destination.
The bitmap is converted to a scaled version and then placed at the right
coordinates.*/
void TEYE_blit_and_scale_to(TEYE_Buffer dest, const TEYE_Buffer src, int x,
                            int y, float scale_x, float scale_y);

/** Draw a bitmap from the given buffer to the destination.

@param dest The destination buffer
@param src The source destination buffer
@param mode Determine how the given bitmap is copied onto the
buffer.
- `FitWidth`: The bitmap is scaled in a way that its width fits the screen.
- `FitHeight`: The bitmap is scaled in a way that its height fits the screen.
- `Stretch`: The bitmap is stretched in a way that both its height its width
fits.
- `FitBest`: The bitmap is scaled such that it take as much space as possible
while keeping the aspect ratio. the screen.
@param x
@param y The absolute coordinates in pixels in the destination buffer
*/
void TEYE_blit_custom_scale_to(TEYE_Buffer dest, const TEYE_Buffer src,
                               ScalingMode mode, int x, int y);

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
