#include "timer.h"
#include <signal.h>
#include <stdatomic.h>
#include <teye/teye.h>

volatile atomic_int running = 1;

static int w = 300;
static int h = 200;

static void signal_handler() {
    running = 0;
}

int main() {

    signal(SIGINT, signal_handler);

    TEYE_Buffer buffer = {0};

    TEYE_init();
    TEYE_allocate_buffer(&buffer, w, h);

    int pos_x = w/2;
    int pos_y = h/2;

    int v = 2;

    while (running) {
        TEYE_clear_buffer(buffer, 1);

        pos_y += v;

        if (pos_y > w - 150)
          v = -2;
        else if (pos_y < 50)
          v = 2;

        for (int i = 0; i < w; i++) {
            for (int j = 0; j < h; j++) {

                int x = i - pos_x;
                int y = j - pos_y;

                if (x*x + y*y < 1000) {
                    set_buffer_pixel(buffer, i, j, 3);
                }
            }
        }

        TEYE_blit(buffer, FitHeight, 0, 0, 1, 1);
        TEYE_render_frame();

        sleep_ms(1000/60);
    }


    TEYE_free_buffer(&buffer);
    TEYE_free();
}