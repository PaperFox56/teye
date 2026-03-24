# Teye

I have recently been working on many small projects where I needed to draw bitmap art in the terminal. I quickly realized that most of the fonctionality I implemented in those projects could be incorporated in a single library that could be maintained easily.
Here it is. I hope this can be useful to someone else. Feel free to use the code in your own project and if you encounter any issues, either open an issue in the corresponding session or change the code to your heart content and  open a PR so that everyone can benefit.

<img width="722" height="690" alt="image" src="https://github.com/user-attachments/assets/40613130-d6a7-46ad-bc4f-ef60f870ebf1" />

*A rendering of the mandelbrot set with Teye (see the corresponding test)*

![chip8](https://github.com/user-attachments/assets/480d3e84-f15f-4e0a-8993-610d68d74b1c)

*Visuals of a Chip8 emulator that I am currently working on, with Teye as the rendering engine*

## Version
0.3.1

### Changelog from 0.2
Refactored the API to give more control to the user
- The enum `DrawingMode` was renamed to `ScalingMode` for more clarity
- Add a collection of `blit` functions to give a more fine grained control to the drawing process
- The enum `SampleMode` was created in prevision for an upcoming feature
- The function `TEYE_get_framebuffer` was added to the public API. This function allows the user to write directly to the framebuffers or read from them.

### Objectives for 0.4
- Give more control to the user (restrict rendering region, not clear screen witout user permission)

## Contributing to the project
Please read [here](CONTRIBUTING.md) for information on how to help the project.

## Licence
See [LICENCE](LICENCE) for more information.

## Contents

- src/, include/ - core framebuffer and terminal render code (TEYE_* API)
- tests/ - ontains small programs used to test the features

## Requirements

- C compiler (gcc/clang)
- POSIX-compatible terminal (Linux, macOS). Windows may work but is untested.

## Build and run

Use Make:

   ```bash
   make
   sudo make install
   ```

will build the library and install it in /usr/local/.

To build the tests:

```bash
make tests
```

More about the build system [here](BUILD.md)

## Using the TEYE API
Feel free to see the examples in the tests/ sections. They are kept in sync with the library.

