// ============================================================================
// video_processing.cpp  --  Function DEFINITIONS (how each operation works)
// ============================================================================

#include <iostream>
#include <cstdlib>          // abs()
#include "video_processing.h"

using namespace std;

// ---------------------------------------------------------------------------
// Utilities
// ---------------------------------------------------------------------------

// Keep a value in the 8-bit range 0..255 and round it.
int clamp255(double value) {
    if (value < 0)   return 0;
    if (value > 255) return 255;
    return (int)(value + 0.5);
}

// Is this coordinate a real pixel in the frame? (prevents array overflow)
bool isInside(Point p) {
    return p.x >= 0 && p.x < FRAME_W && p.y >= 0 && p.y < FRAME_H;
}

// ---------------------------------------------------------------------------
// Color space conversion
// ---------------------------------------------------------------------------

// LOWEST LEVEL: one pixel, RGB -> YCbCr (a 3x3 matrix times a 3-value vector)
YCbCrPixel rgbToYcbcr(RGBPixel in, ColorStandard std) {
    double rgb[3]    = { (double)in.r, (double)in.g, (double)in.b };
    double offset[3] = { 0.0, 128.0, 128.0 };
    int    result[3];

    for (int row = 0; row < 3; row++) {
        double sum = offset[row];
        for (int col = 0; col < 3; col++) {
            sum += RGB_TO_YCBCR[std][row][col] * rgb[col];
        }
        result[row] = clamp255(sum);
    }
    YCbCrPixel out = { result[0], result[1], result[2] };
    return out;
}

// One pixel, YCbCr -> RGB (the inverse matrix)
RGBPixel ycbcrToRgb(YCbCrPixel in, ColorStandard std) {
    double ycc[3] = { (double)in.y, in.cb - 128.0, in.cr - 128.0 };
    int    result[3];

    for (int row = 0; row < 3; row++) {
        double sum = 0.0;
        for (int col = 0; col < 3; col++) {
            sum += YCBCR_TO_RGB[std][row][col] * ycc[col];
        }
        result[row] = clamp255(sum);
    }
    RGBPixel out = { result[0], result[1], result[2] };
    return out;
}

// MIDDLE LEVEL: a whole frame (calls rgbToYcbcr for every pixel)
void convertFrame(const RGBFrame& in, YCbCrFrame& out, ColorStandard std) {
    out.frameNumber = in.frameNumber;
    for (int y = 0; y < FRAME_H; y++) {
        for (int x = 0; x < FRAME_W; x++) {
            out.pix[y][x] = rgbToYcbcr(in.pix[y][x], std);
        }
    }
}

// Convert back to RGB and report the biggest difference in any channel.
// Shows students that rounding to whole numbers loses a little information.
int roundTripError(const RGBFrame& original, const YCbCrFrame& converted,
                   ColorStandard std) {
    int worst = 0;
    for (int y = 0; y < FRAME_H; y++) {
        for (int x = 0; x < FRAME_W; x++) {
            RGBPixel back = ycbcrToRgb(converted.pix[y][x], std);
            RGBPixel orig = original.pix[y][x];
            int diffs[3] = { abs(back.r - orig.r), abs(back.g - orig.g),
                             abs(back.b - orig.b) };
            for (int c = 0; c < 3; c++) {
                if (diffs[c] > worst) worst = diffs[c];
            }
        }
    }
    return worst;
}

// ---------------------------------------------------------------------------
// Measurements
// ---------------------------------------------------------------------------

double averageLuma(const YCbCrFrame& f) {
    long sum = 0;
    for (int y = 0; y < FRAME_H; y++)
        for (int x = 0; x < FRAME_W; x++)
            sum += f.pix[y][x].y;
    return (double)sum / (FRAME_W * FRAME_H);
}

// Scan every pixel and remember the coordinates of the brightest one.
Point findBrightest(const YCbCrFrame& f) {
    Point best = { 0, 0 };
    for (int y = 0; y < FRAME_H; y++) {
        for (int x = 0; x < FRAME_W; x++) {
            if (f.pix[y][x].y > f.pix[best.y][best.x].y) {
                best.x = x;
                best.y = y;
            }
        }
    }
    return best;
}

// 4:2:0 subsampling: average Cb and Cr over each 2x2 block of pixels.
void subsample420(const YCbCrFrame& f, ChromaPlanes420& out) {
    for (int by = 0; by < FRAME_H / 2; by++) {
        for (int bx = 0; bx < FRAME_W / 2; bx++) {
            int cbSum = 0, crSum = 0;
            for (int dy = 0; dy < 2; dy++) {
                for (int dx = 0; dx < 2; dx++) {
                    const YCbCrPixel& p = f.pix[by * 2 + dy][bx * 2 + dx];
                    cbSum += p.cb;
                    crSum += p.cr;
                }
            }
            out.cb[by][bx] = cbSum / 4;
            out.cr[by][bx] = crSum / 4;
        }
    }
}

// ---------------------------------------------------------------------------
// Coordinates and drawing
// ---------------------------------------------------------------------------

// Build a square box of 'size' pixels with 'center' roughly in the middle.
Rect boxAround(Point center, int size) {
    Rect r;
    r.topLeft.x = center.x - size / 2;
    r.topLeft.y = center.y - size / 2;
    r.width  = size;
    r.height = size;
    return r;
}

// Paint only the edges of the rectangle. Pixels off-screen are skipped.
void drawBox(RGBFrame& f, Rect box, RGBPixel color) {
    int left   = box.topLeft.x;
    int top    = box.topLeft.y;
    int right  = left + box.width  - 1;
    int bottom = top  + box.height - 1;

    for (int y = top; y <= bottom; y++) {
        for (int x = left; x <= right; x++) {
            bool onEdge = (x == left || x == right || y == top || y == bottom);
            Point p = { x, y };
            if (onEdge && isInside(p)) {
                f.pix[y][x] = color;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Test clip and display
// ---------------------------------------------------------------------------

// Scene 1 (frames 0-3): dark blue room, a white "ball" moves right.
// Scene 2 (frames 4-7): bright gray-green outdoors, the ball moves down.
void makeTestClip(VideoClip& clip) {
    clip.fps      = CLIP_FPS;
    clip.standard = BT709;

    RGBPixel darkBlue  = {  20,  30,  70 };
    RGBPixel brightBkg = { 140, 170, 120 };
    RGBPixel white     = { 255, 255, 255 };

    for (int i = 0; i < NUM_FRAMES; i++) {
        RGBFrame& f = clip.frames[i];
        f.frameNumber = i;

        bool sceneTwo = (i >= NUM_FRAMES / 2);
        RGBPixel background = sceneTwo ? brightBkg : darkBlue;

        // fill the background
        for (int y = 0; y < FRAME_H; y++)
            for (int x = 0; x < FRAME_W; x++)
                f.pix[y][x] = background;

        // place the ball: a single bright pixel whose position changes
        Point ball;
        if (!sceneTwo) { ball.x = 1 + i;             ball.y = 3; }
        else           { ball.x = 4; ball.y = 1 + (i - NUM_FRAMES / 2); }
        if (isInside(ball)) f.pix[ball.y][ball.x] = white;
    }
}

// Draw a frame as text: brightness becomes a character, red overlay becomes #.
void printFrameAscii(const RGBFrame& f, ColorStandard std) {
    const char ramp[] = " .:-=+*%@";   // dark -> bright
    const int  levels = 9;
    for (int y = 0; y < FRAME_H; y++) {
        cout << "    ";
        for (int x = 0; x < FRAME_W; x++) {
            RGBPixel p = f.pix[y][x];
            if (p.r == 255 && p.g == 0 && p.b == 0) {
                cout << "# ";                       // tracking box
            } else {
                int luma = rgbToYcbcr(p, std).y;
                cout << ramp[luma * (levels - 1) / 255] << ' ';
            }
        }
        cout << "\n";
    }
}
