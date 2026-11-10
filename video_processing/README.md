# ECE 206 Video Pipeline Project

**Author:** Prof. Linda Megerdichian

This is a small C++ project that shows how video engineers use **arrays**, **coordinates**, and **custom data types** to process video. It builds a short test clip, then runs every frame through these steps:

1. Color space conversion (RGB to YCbCr)
2. Accuracy check (converting back to RGB)
3. Brightness measurement
4. Scene-cut detection
5. Object tracking by coordinates
6. Drawing a box around the tracked object
7. Chroma subsampling (4:2:0)

---

## Files you need for the project

All four files must be in the **same folder**.

| File | Required? | Type | What it does |
|---|---|---|---|
| `video_types.h` | **Yes** | Header (data only) | Defines all the custom data types: pixels, frames, the video clip, coordinates (`Point`, `Rect`), and the color conversion matrices. Every other file includes it. |
| `video_processing.h` | **Yes** | Header (declarations) | Lists every processing function and what it takes in and returns. Think of it as the "menu" of operations. |
| `video_processing.cpp` | **Yes** | Source (definitions) | Contains the code for each function: color conversion, brightness, tracking, drawing, subsampling, and the text display. |
| `main.cpp` | **Yes** | Source (top level) | Runs the whole pipeline. It calls the other functions in order, collects the results, and prints the report. |
| `README.md` | No | Documentation | This file. |

The older standalone examples (`01_color_space_conversion.cpp`, `02_pixel_coordinates.cpp`, `03_video_hierarchy.cpp`) are **not** part of this project. Each one compiles and runs on its own, and they make a good warm-up before this project.

---

## How the files connect

```
main.cpp                  <- TOP LEVEL: runs the pipeline
  │  #include
  ├── video_types.h       <- data types (what things ARE)
  └── video_processing.h  <- function list (what we can DO)
            │  code lives in
            └── video_processing.cpp  <- how each function works
                    │  #include
                    └── video_types.h
```

Each `.h` file has an **include guard** (`#ifndef ... #define ... #endif`). Because of it, a header that gets included more than once is still only read once.

---

## The data hierarchy (from `video_types.h`)

```
VideoClip                        LEVEL 1  the whole clip (frames + fps + color standard)
  └── RGBFrame frames[8]         LEVEL 2  one picture in time
        └── RGBPixel pix[8][8]   LEVEL 3  one dot on the screen
              └── int r, g, b    LEVEL 4  one color channel
```

Helper types:

- `YCbCrPixel` / `YCbCrFrame`: the same picture stored as brightness (Y) plus two color channels (Cb, Cr)
- `ChromaPlanes420`: the color data after 4:2:0 subsampling, at half the width and half the height
- `Point` / `Rect`: positions and boxes inside a frame. **(0,0) is the top-left pixel.** Arrays are indexed `pix[y][x]`, row first.
- `FrameReport`: the measurements saved for each frame
- `ColorStandard`: the color standard to use, `BT601` (SD TV) or `BT709` (HDTV)

---

## Function levels (from `video_processing.cpp`)

| Level | Function | Calls |
|---|---|---|
| Top | `main()` | everything below |
| Frame | `convertFrame()` | `rgbToYcbcr()` for every pixel |
| Frame | `roundTripError()` | `ycbcrToRgb()` for every pixel |
| Frame | `averageLuma()`, `findBrightest()`, `subsample420()`, `drawBox()` | loop over the pixel array |
| Pixel | `rgbToYcbcr()`, `ycbcrToRgb()` | 3×3 matrix math |
| Utility | `clamp255()`, `isInside()`, `boxAround()` | — |
| Setup / display | `makeTestClip()`, `printFrameAscii()` | — |

---

## How to build and run

From a terminal in the project folder:

```bash
g++ -std=c++11 -Wall main.cpp video_processing.cpp -o video_pipeline
./video_pipeline
```

On Windows, run `video_pipeline.exe`.

**Note:** compile only the `.cpp` files. The `.h` files are pulled in automatically through `#include`.

**In an IDE** (Visual Studio, Code::Blocks, CLion, and so on): create an empty C++ project, add all four files, and build.

---

## What the output shows

- A table with one row per frame: the time, the average brightness, the (x, y) position of the tracked object, the round-trip error, and a **SCENE CUT** flag when the brightness jumps.
- Text drawings of four frames. `#` marks the tracking box, `@` is the bright object, and dots and symbols show the background brightness.
- How much 4:2:0 subsampling saves: 192 → 96 samples per frame (50% smaller).

---

## Things to try

- In `makeTestClip()` in `video_processing.cpp`, change `clip.standard = BT709;` to `BT601` and compare the round-trip errors.
- Change the colors or the path of the moving "ball" in `makeTestClip()`.
- Change `CUT_THRESHOLD` in `main.cpp` and see when the cut is no longer detected.
- Change `FRAME_W` and `FRAME_H` in `video_types.h` to make the frames bigger. Keep both values even so the 2×2 blocks for 4:2:0 still fit.
- Add a new step: write a function in `video_processing.cpp`, declare it in `video_processing.h`, and call it from the loop in `main.cpp`.
