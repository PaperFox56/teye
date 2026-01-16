#include <signal.h>
#include <stdio.h>

#include "teye.h"
#include "timer.h"

int running = 1;
ushort FPS = 8;

void update_frame(TEYE_Buffer buffer);

void signalHandler(int sig) {
  printf("Aborting\n");

  running = 0;
}

const ushort w = 400, h = 150;

int main(int args, char **argv) {

  signal(SIGINT, signalHandler);

  TEYE_Buffer buffer = TEYE_init(w, h);

  int frame_count = 0;

  long frame_duration = 1000 / 2;
  long start = currentTimeMillis();
  long prev_time = start;

  while (running) {

    long current_time = currentTimeMillis();
    long dela_time = current_time - prev_time;

    if (dela_time < frame_duration) {
      sleep_ms(frame_duration - dela_time);
    }

    prev_time = current_time;

    update_frame(buffer);

    TEYE_render_frame_mode_2();

    // running = 0;
    frame_count++;
  }

  long end = currentTimeMillis();

  double time_elapsed = (end - start) / 1000.;
  double fps = frame_count / time_elapsed;

  TEYE_free();

  printf("Time: %lf\n", time_elapsed);
  printf("FPS: %lf\n", fps);

  return 0;
}

void get_time(ushort *hours, ushort *minutes, ushort *seconds) {
  long time = currentTimeMillis() / 1000;

  *seconds = time % 60;

  time /= 60;
  *minutes = time % 60;

  time /= 60;
  *hours = (time + 1) % 24; // TODO: let the program use the local time
}

typedef struct {
  ushort x;
  ushort y;
} Point;

/// some useful variables to draw the digits
const ushort thickness = w / 60;
const ushort segment_size = w / 10;

Point segments[][2] = {
    // shape (start and end point) of each segment in a digit
    {{0, 0}, {segment_size, thickness}},                           // top top
    {{0, 0}, {thickness, segment_size}},                           // top left
    {{segment_size - thickness, 0}, {segment_size, segment_size}}, // top right
    {{0, segment_size - (thickness / 2)},
     {segment_size, segment_size + (thickness / 2)}},   // middle
    {{0, segment_size}, {thickness, segment_size * 2}}, // bottom left
    {{segment_size - thickness, segment_size},
     {segment_size, segment_size * 2}}, // bottom right
    {{0, segment_size * 2 - thickness},
     {segment_size, segment_size * 2}}, // bottom bottom
};

int digits[][7] = {
    {1, 1, 1, 0, 1, 1, 1}, // 0
    {0, 0, 1, 0, 0, 1, 0}, // 1
    {1, 0, 1, 1, 1, 0, 1}, // 2
    {1, 0, 1, 1, 0, 1, 1}, // 3
    {0, 1, 1, 1, 0, 1, 0}, // 4
    {1, 1, 0, 1, 0, 1, 1}, // 5
    {1, 1, 0, 1, 1, 1, 1}, // 6
    {1, 0, 1, 0, 0, 1, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}, // 9
};

void update_frame(TEYE_Buffer buffer) {

  // We will show a very basic 7-seg type clock in the format hh:mm:ss
  // the first is to get the time right

  ushort hours, minutes, seconds;
  get_time(&hours, &minutes, &seconds);

  // printf("%2d:%2d:%2d\n", hours, minutes, seconds);

  TEYE_clear_buffer(0);

  int v_gap = (h - segment_size * 2) / 2;
  int h_gap = (w - segment_size * 8 - thickness * 5) / 2;

  int digit_x = h_gap;
  int digit_y = v_gap;

  for (int i = 0; i < 6; i++) {

    int k = 0;

    switch (i) {
    case 0:
      k = hours / 10;
      break;
    case 1:
      k = hours % 10;
      break;
    case 2:
      k = minutes / 10;
      break;
    case 3:
      k = minutes % 10;
      break;
    case 4:
      k = seconds / 10;
      break;
    case 5:
      k = seconds % 10;
      break;
    }

    int *digit = digits[k];

    for (int seg = 0; seg < 7; seg++) {
      if (digit[seg]) {
        for (int x = segments[seg][0].x; x < segments[seg][1].x; x++) {
          for (int y = segments[seg][0].y; y < segments[seg][1].y; y++) {
            buffer.frame_buffer[y + digit_y][x + digit_x] = 1;
          }
        }
      }
    }

    digit_x += thickness + segment_size;
    if (i % 2 == 1 && i < 5) {
      digit_x += segment_size / 2;

      int y0 = v_gap + 2 * segment_size / 3;
      // add the colons
      for (int j = 0; j < 2; j++) {

        for (int x = digit_x - thickness / 2; x < digit_x + thickness / 2;
             x++) {
          for (int y = y0 - thickness / 2; y < y0 + thickness / 2; y++) {
            buffer.frame_buffer[y][x] = 1;
          }
        }

        y0 += 2 * segment_size / 3;
      }

      digit_x += segment_size / 2;
    }
  }
}
