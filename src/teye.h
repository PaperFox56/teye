/**
 * teye.h
 

 I made this library because I couldn't find a simple program that could print a
 specific bitmap pattern in the terminal and that was both easy to work with and
 customizable. This one might not suit your needs as it was specifically tailored
 for the tasks I wanted to do. But if you find a addition to make it more useful 
 for the most people, you're free to build upon my work, copy it and change it as 
 you like.

 In the current version, it only support monochromatic (shades of gray outputs)
 but the final goal is a 64 colors screen.

 */

#ifndef TEYE_H
#define TEYE_H

#include <stdint.h>

typedef unsigned short ushort;

typedef struct {

    ushort width;
    ushort height;
    uint8_t **frame_buffer;

} TEYE_Buffer;

// Effectively erase the old frame and bring the current frame's buffer to be drawn on the screen
TEYE_Buffer TEYE_init(ushort width, ushort height);
void TEYE_render_frame_mode_1();
void TEYE_render_frame_mode_2();
void TEYE_clear_buffer();
void TEYE_free();


#endif
