#include <stdio.h>

#include "timer.h"
#include "teye.h"


int main(int args, char** argv) {


    ushort w = 300, h = 300;

    TEYE_Buffer buffer = TEYE_init(w, h);

    long start = currentTimeMillis();

    int frame_count = 10;

    for (int c = 0; c < frame_count; c++) {


        for (ushort i = 0; i < w; i++) {
            for (ushort j = 0; j < w; j++) {

                int d = i*i + j*j;
                int s = w*w;

                if (d < s/3) {
                    buffer.frame_buffer[i][j] = 1;
                }
                else if (d < s/2) {
                    buffer.frame_buffer[i][j] = 2;
                }
                else if (d < s) {
                    buffer.frame_buffer[i][j] = 3;
                }
            }
        }

        TEYE_render_frame();

    }

    long end = currentTimeMillis();
    
    double time_elapsed = (end - start) / 1000.;
    double fps = frame_count / time_elapsed;

    TEYE_free();

    printf("Time: %lf\n", time_elapsed);
    printf("FPS: %lf\n", fps);

    return 0;
}