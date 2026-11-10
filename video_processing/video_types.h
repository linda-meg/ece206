// ============================================================================
// video_types.h  --  Custom data types for the ECE 206 Video Pipeline Project
// Author: Prof. Linda Megerdichian
// ----------------------------------------------------------------------------
// This header defines ONLY data: constants, enums, and structs.
// Every other file #includes it, so the whole project agrees on what a
// "pixel", "frame", or "video clip" looks like.
//
// DATA HIERARCHY (top to bottom):
//
//   VideoClip                       <- LEVEL 1: the whole clip
//     └── RGBFrame frames[]         <- LEVEL 2: one picture in time
//           └── RGBPixel pix[][]    <- LEVEL 3: one dot on the screen
//                 └── int r, g, b   <- LEVEL 4: one color channel
//
//   Helper types used while processing:
//     YCbCrFrame / YCbCrPixel  - the same picture in video color space
//     ChromaPlanes420          - color data after 4:2:0 subsampling
//     Point / Rect             - coordinates and regions inside a frame
//     FrameReport              - the results we measure for each frame
// ============================================================================

#ifndef VIDEO_TYPES_H      // "include guard": stops this file from being
#define VIDEO_TYPES_H      // pasted in twice if two files both include it

// ---- Global sizes -----------------------------------------------------------
const int FRAME_W    = 8;    // pixels across  (x direction)
const int FRAME_H    = 8;    // pixels down    (y direction)
const int NUM_FRAMES = 8;    // frames in the clip
const int CLIP_FPS   = 30;   // frames per second

// ---- Which color standard to use -------------------------------------------
// BT.601 = standard-definition TV, BT.709 = HDTV.  Same idea, different math.
enum ColorStandard {
    BT601 = 0,
    BT709 = 1
};

// ---- Conversion matrices, indexed by [standard][output row][input column] --
// RGB -> YCbCr   (add 128 to Cb and Cr afterwards)
const double RGB_TO_YCBCR[2][3][3] = {
    {   // BT.601
        {  0.2990,  0.5870,  0.1140 },
        { -0.1687, -0.3313,  0.5000 },
        {  0.5000, -0.4187, -0.0813 }
    },
    {   // BT.709
        {  0.2126,  0.7152,  0.0722 },
        { -0.1146, -0.3854,  0.5000 },
        {  0.5000, -0.4542, -0.0458 }
    }
};

// YCbCr -> RGB   (subtract 128 from Cb and Cr first)
// columns are: Y, Cb, Cr
const double YCBCR_TO_RGB[2][3][3] = {
    {   // BT.601
        { 1.0,  0.0000,  1.4020 },
        { 1.0, -0.3441, -0.7141 },
        { 1.0,  1.7720,  0.0000 }
    },
    {   // BT.709
        { 1.0,  0.0000,  1.5748 },
        { 1.0, -0.1873, -0.4681 },
        { 1.0,  1.8556,  0.0000 }
    }
};

// ---- LEVEL 4 / 3: pixels ----------------------------------------------------
struct RGBPixel {
    int r;      // 0..255
    int g;      // 0..255
    int b;      // 0..255
};

struct YCbCrPixel {
    int y;      // luma (brightness), 0..255
    int cb;     // blue-difference,   0..255 (128 = no color)
    int cr;     // red-difference,    0..255 (128 = no color)
};

// ---- Coordinates ------------------------------------------------------------
// (0,0) is the TOP-LEFT pixel. x goes right, y goes down.
// Arrays are indexed  pix[y][x]  -> row first, then column.
struct Point {
    int x;
    int y;
};

struct Rect {
    Point topLeft;
    int   width;
    int   height;
};

// ---- LEVEL 2: frames --------------------------------------------------------
struct RGBFrame {
    RGBPixel pix[FRAME_H][FRAME_W];
    int      frameNumber;
};

struct YCbCrFrame {
    YCbCrPixel pix[FRAME_H][FRAME_W];
    int        frameNumber;
};

// After 4:2:0 subsampling there is ONE Cb and ONE Cr per 2x2 block,
// so the chroma planes are half as wide and half as tall.
struct ChromaPlanes420 {
    int cb[FRAME_H / 2][FRAME_W / 2];
    int cr[FRAME_H / 2][FRAME_W / 2];
};

// ---- LEVEL 1: the whole clip ------------------------------------------------
struct VideoClip {
    RGBFrame      frames[NUM_FRAMES];
    int           fps;
    ColorStandard standard;
};

// ---- Measurements collected for each frame by the pipeline ------------------
struct FrameReport {
    int    frameNumber;
    double timeSeconds;
    double avgLuma;          // average brightness
    bool   sceneCut;         // did the brightness jump from the last frame?
    Point  brightest;        // where the tracked object is
    Rect   trackingBox;      // the box we drew around it
    int    maxRoundTripErr;  // worst error after RGB->YCbCr->RGB
};

#endif // VIDEO_TYPES_H
