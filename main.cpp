// ============================================================================
// main.cpp  --  TOP LEVEL: the complete video processing pipeline
// ----------------------------------------------------------------------------
// This file doesn't do any pixel math itself. It CALLS the lower levels in
// order, the way a real video system chains its processing stages:
//
//   1. Load / generate the clip         makeTestClip()
//   2. For each frame:
//        a. RGB -> YCbCr                convertFrame()   -> rgbToYcbcr()
//        b. Check conversion accuracy   roundTripError() -> ycbcrToRgb()
//        c. Measure brightness          averageLuma()
//        d. Detect scene cuts           (compare with previous frame)
//        e. Track the brightest object  findBrightest()
//        f. Draw a box around it        boxAround() + drawBox()
//        g. Compress the color          subsample420()
//   3. Print a report
//
// BUILD:   g++ -std=c++11 -Wall main.cpp video_processing.cpp -o video_pipeline
// RUN:     ./video_pipeline
// ============================================================================

#include <iostream>
#include <iomanip>
#include <cmath>
#include "video_types.h"
#include "video_processing.h"

using namespace std;

int main() {
    // ---- Stage 1: get the video --------------------------------------------
    VideoClip clip;
    makeTestClip(clip);

    FrameReport report[NUM_FRAMES];         // one result record per frame
    const double CUT_THRESHOLD = 40.0;      // luma jump that counts as a cut
    const RGBPixel RED = { 255, 0, 0 };     // tracking-box color
    double previousLuma = 0.0;

    // ---- Stage 2: process every frame --------------------------------------
    for (int i = 0; i < NUM_FRAMES; i++) {
        RGBFrame& rgb = clip.frames[i];
        YCbCrFrame ycc;

        // a. color space conversion (top -> middle -> lowest level)
        convertFrame(rgb, ycc, clip.standard);

        // b. accuracy check
        report[i].maxRoundTripErr = roundTripError(rgb, ycc, clip.standard);

        // c. brightness
        report[i].frameNumber = rgb.frameNumber;
        report[i].timeSeconds = (double)rgb.frameNumber / clip.fps;
        report[i].avgLuma     = averageLuma(ycc);

        // d. scene-cut detection
        report[i].sceneCut = (i > 0 &&
                              fabs(report[i].avgLuma - previousLuma) > CUT_THRESHOLD);
        previousLuma = report[i].avgLuma;

        // e + f. object tracking with coordinates
        report[i].brightest   = findBrightest(ycc);
        report[i].trackingBox = boxAround(report[i].brightest, 3);
        drawBox(rgb, report[i].trackingBox, RED);

        // g. 4:2:0 chroma subsampling (result used for the data-size summary)
        ChromaPlanes420 chroma;
        subsample420(ycc, chroma);
    }

    // ---- Stage 3: report ----------------------------------------------------
    cout << "VIDEO PIPELINE REPORT  (" << NUM_FRAMES << " frames, "
         << FRAME_W << "x" << FRAME_H << ", " << clip.fps << " fps, "
         << (clip.standard == BT709 ? "BT.709" : "BT.601") << ")\n\n";

    cout << "Frame |  Time  | Avg Y | Object (x,y) | RT err | Note\n";
    cout << "------+--------+-------+--------------+--------+-----------\n";
    for (int i = 0; i < NUM_FRAMES; i++) {
        cout << setw(5) << report[i].frameNumber << " | "
             << fixed << setprecision(3) << setw(6) << report[i].timeSeconds << " | "
             << setprecision(1) << setw(5) << report[i].avgLuma << " | "
             << "    (" << report[i].brightest.x << "," << report[i].brightest.y << ")    | "
             << setw(6) << report[i].maxRoundTripErr << " | "
             << (report[i].sceneCut ? "SCENE CUT" : "") << "\n";
    }

    // Show the first and last frame of each scene with the tracking box.
    int show[] = { 0, NUM_FRAMES / 2 - 1, NUM_FRAMES / 2, NUM_FRAMES - 1 };
    for (int k = 0; k < 4; k++) {
        cout << "\nFrame " << show[k] << "  (# = tracking box, @ = brightest)\n";
        printFrameAscii(clip.frames[show[k]], clip.standard);
    }

    // Data savings from 4:2:0
    int full = FRAME_W * FRAME_H * 3;
    int sub  = FRAME_W * FRAME_H + 2 * (FRAME_W / 2) * (FRAME_H / 2);
    cout << "\n4:2:0 subsampling: " << full << " -> " << sub
         << " samples per frame (" << 100 - sub * 100 / full << "% smaller)\n";
    return 0;
}
