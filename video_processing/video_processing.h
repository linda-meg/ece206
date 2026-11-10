// ============================================================================
// video_processing.h  --  Function DECLARATIONS (the "menu" of operations)
// ----------------------------------------------------------------------------
// A header tells other files WHAT functions exist and what types they use.
// The actual code (HOW they work) lives in video_processing.cpp.
// ============================================================================

#ifndef VIDEO_PROCESSING_H
#define VIDEO_PROCESSING_H

#include "video_types.h"

// ---- Utilities --------------------------------------------------------------
int  clamp255(double value);
bool isInside(Point p);

// ---- Color space conversion (pixel -> frame hierarchy) ----------------------
YCbCrPixel rgbToYcbcr(RGBPixel in, ColorStandard std);
RGBPixel   ycbcrToRgb(YCbCrPixel in, ColorStandard std);
void       convertFrame(const RGBFrame& in, YCbCrFrame& out, ColorStandard std);
int        roundTripError(const RGBFrame& original, const YCbCrFrame& converted,
                          ColorStandard std);

// ---- Measurements -----------------------------------------------------------
double averageLuma(const YCbCrFrame& f);
Point  findBrightest(const YCbCrFrame& f);
void   subsample420(const YCbCrFrame& f, ChromaPlanes420& out);

// ---- Coordinates and drawing ------------------------------------------------
Rect boxAround(Point center, int size);
void drawBox(RGBFrame& f, Rect box, RGBPixel color);

// ---- Test clip and display --------------------------------------------------
void makeTestClip(VideoClip& clip);
void printFrameAscii(const RGBFrame& f, ColorStandard std);

#endif // VIDEO_PROCESSING_H
