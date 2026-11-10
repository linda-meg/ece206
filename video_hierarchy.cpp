// ============================================================================
// Project 3: Video Data Hierarchy -- Pixel -> Frame -> Video Clip
// ----------------------------------------------------------------------------
// WHY THIS MATTERS:
//   A video file is just arrays inside arrays:
//     Video  = an array of Frames        (e.g., 30 frames per second)
//     Frame  = a 2D array of Pixels      (rows x columns)
//     Pixel  = 3 numbers                 (Y, Cb, Cr) or (R, G, B)
//   Real video tools walk this hierarchy all the time. Here we build a tiny
//   "scene-change detector": it measures each frame's average brightness and
//   flags a cut when brightness jumps suddenly (like an editor's timeline).
//
// CONCEPTS PRACTICED:
//   - structs that CONTAIN arrays of other structs (levels of hierarchy)
//   - 4 levels of nested loops (frame -> row -> column -> channel)
//   - 4:2:0 chroma subsampling using 2x2 pixel blocks and coordinates
// ============================================================================

#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

const int W = 4;            // frame width
const int H = 4;            // frame height
const int NUM_FRAMES = 5;   // frames in our tiny clip

// ---- LEVEL 3: a single pixel ------------------------------------------------
struct Pixel {
    int Y;    // brightness
    int Cb;   // blue-difference color
    int Cr;   // red-difference color
};

// ---- LEVEL 2: a frame is a 2D array of pixels -------------------------------
struct Frame {
    Pixel pix[H][W];
    int   frameNumber;
};

// ---- LEVEL 1: a video clip is an array of frames ----------------------------
struct Video {
    Frame frames[NUM_FRAMES];
    int   fps;            // frames per second
};

// Fill one frame with a flat color plus a small gradient so it's not boring.
void fillFrame(Frame& f, int number, int baseY, int cb, int cr) {
    f.frameNumber = number;
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            f.pix[y][x].Y  = baseY + x * 2;   // slight left-to-right ramp
            f.pix[y][x].Cb = cb;
            f.pix[y][x].Cr = cr;
        }
    }
}

// Average brightness (luma) of one frame.
double averageLuma(const Frame& f) {
    long sum = 0;
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            sum += f.pix[y][x].Y;
    return (double)sum / (W * H);
}

// ============================================================================
// 4:2:0 chroma subsampling
// Keep Y for EVERY pixel, but keep only ONE Cb and ONE Cr per 2x2 block.
// That cuts the color data to 1/4 -- this is what H.264 / YouTube video uses.
// Output arrays are half the width and half the height of the frame.
// ============================================================================
void subsample420(const Frame& f, int cbOut[H / 2][W / 2], int crOut[H / 2][W / 2]) {
    for (int by = 0; by < H / 2; by++) {          // block row
        for (int bx = 0; bx < W / 2; bx++) {      // block column
            int cbSum = 0, crSum = 0;
            // visit the 4 pixels in this 2x2 block
            for (int dy = 0; dy < 2; dy++) {
                for (int dx = 0; dx < 2; dx++) {
                    int y = by * 2 + dy;          // block coords -> pixel coords
                    int x = bx * 2 + dx;
                    cbSum += f.pix[y][x].Cb;
                    crSum += f.pix[y][x].Cr;
                }
            }
            cbOut[by][bx] = cbSum / 4;            // average of the 4 pixels
            crOut[by][bx] = crSum / 4;
        }
    }
}

int main() {
    Video clip;
    clip.fps = 30;

    // Frames 0-2: a dark indoor shot. Frames 3-4: cut to a bright outdoor shot.
    fillFrame(clip.frames[0], 0,  40, 120, 135);
    fillFrame(clip.frames[1], 1,  42, 120, 135);
    fillFrame(clip.frames[2], 2,  41, 121, 134);
    fillFrame(clip.frames[3], 3, 190, 140, 110);
    fillFrame(clip.frames[4], 4, 192, 140, 111);

    // ---- Walk the hierarchy: Video -> Frame -> (average over pixels) --------
    const double CUT_THRESHOLD = 50.0;   // brightness jump that counts as a cut
    double previous = averageLuma(clip.frames[0]);

    cout << "Frame | Time (s) | Avg Y  | Note\n";
    cout << "------+----------+--------+-------------\n";
    for (int i = 0; i < NUM_FRAMES; i++) {
        double avg  = averageLuma(clip.frames[i]);
        double time = (double)clip.frames[i].frameNumber / clip.fps;

        cout << fixed << setprecision(3)
             << setw(5) << i << " | " << setw(8) << time
             << " | " << setw(6) << setprecision(1) << avg << " | ";
        if (i > 0 && fabs(avg - previous) > CUT_THRESHOLD) {
            cout << "<-- SCENE CUT";
        }
        cout << "\n";
        previous = avg;
    }

    // ---- 4:2:0 subsampling on one frame ------------------------------------
    int cb[H / 2][W / 2], cr[H / 2][W / 2];
    subsample420(clip.frames[3], cb, cr);

    cout << "\n4:2:0 on frame 3: Y stays " << W << "x" << H
         << ", Cb/Cr shrink to " << W / 2 << "x" << H / 2 << "\n";
    for (int by = 0; by < H / 2; by++) {
        for (int bx = 0; bx < W / 2; bx++) {
            cout << "  block(" << bx << "," << by << "): Cb=" << cb[by][bx]
                 << " Cr=" << cr[by][bx];
        }
        cout << "\n";
    }

    int fullSamples = W * H * 3;
    int subSamples  = W * H + 2 * (W / 2) * (H / 2);
    cout << "Samples per frame: " << fullSamples << " -> " << subSamples
         << " (saves " << 100 - subSamples * 100 / fullSamples << "%)\n";
    return 0;
}
