#include "timer.h"
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <teye/teye.h>

#define BLACK 232
#define WHITE 231

volatile atomic_int running = 1;

static int w = 300;
static int h = 200;

static void signal_handler() { running = 0; }

int main() {

  signal(SIGINT, signal_handler);

  TEYE_Buffer buffer = {0};

  TEYE_init();
  if (TEYE_allocate_buffer(&buffer, w, h) != 0) {
    perror("Couldn't initialize a buffer");
    TEYE_free();
    exit(1);
  }

  int pos_x = w / 2;
  int pos_y = h / 2;

  int v = 2;

  while (running) {
    pos_y += v;

    if (pos_y > w - 150)
      v = -2;
    else if (pos_y < 50)
      v = 2;

    for (int i = 0; i < h; i++) {
      int x0 = i * w;
      for (int j = 0; j < w; j++) {

        int x = j - pos_x;
        int y = i - pos_y;

        buffer.buffer[j + x0] = (x * x + y * y < 1000) ? WHITE : BLACK;
      }
    }

    TEYE_blit(buffer, FitWidth, 0, 0);
    TEYE_render_frame();

    sleep_ms(1000 / 60);
  }

  TEYE_free_buffer(&buffer);
  TEYE_free();
}
