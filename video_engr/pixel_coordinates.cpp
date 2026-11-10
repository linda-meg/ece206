// ============================================================================
// Project 2: Working with Pixel Coordinates (Video Engineering)
// ----------------------------------------------------------------------------
// WHY THIS MATTERS:
//   Every video tool -- scaling 4K down to 1080p, cropping, picture-in-picture,
//   drawing a face-detection box -- is really just math on (x, y) coordinates.
//
//   Coordinate convention used in video/images:
//     (0,0) is the TOP-LEFT pixel
//     x grows to the RIGHT  (column index)
//     y grows DOWNWARD      (row index)
//     so a pixel is accessed as  frame[y][x]   <-- row first, then column!
//
// CONCEPTS PRACTICED:
//   - struct to group related values (Point, Rect)
//   - 2D arrays indexed by coordinates
//   - Mapping coordinates from one grid to another (scaling)
//   - Bounds checking so we never write outside the array
// ============================================================================

#include <iostream>

using namespace std;

const int SRC_H = 8, SRC_W = 8;   // source frame size
const int DST_H = 4, DST_W = 4;   // scaled-down frame size

// ---- Small building blocks --------------------------------------------------
struct Point {          // one location in the frame
    int x;              // column
    int y;              // row
};

struct Rect {           // a rectangle defined by its top-left corner + size
    Point topLeft;
    int width;
    int height;
};

// Is a point inside the frame? Prevents out-of-bounds array access.
bool isInside(Point p, int frameW, int frameH) {
    return p.x >= 0 && p.x < frameW && p.y >= 0 && p.y < frameH;
}

// ============================================================================
// Example A: Nearest-neighbor SCALING (8x8 -> 4x4)
// For every DESTINATION pixel, find which SOURCE pixel it "came from":
//   srcX = dstX * (SRC_W / DST_W)
//   srcY = dstY * (SRC_H / DST_H)
// This is the same idea a TV uses to fit a 4K signal on a 1080p panel.
// ============================================================================
void scaleFrame(const int src[SRC_H][SRC_W], int dst[DST_H][DST_W]) {
    for (int dstY = 0; dstY < DST_H; dstY++) {
        for (int dstX = 0; dstX < DST_W; dstX++) {
            Point srcPt;
            srcPt.x = dstX * SRC_W / DST_W;
            srcPt.y = dstY * SRC_H / DST_H;
            dst[dstY][dstX] = src[srcPt.y][srcPt.x];
        }
    }
}

// ============================================================================
// Example B: Draw a bounding box (like an object-detection overlay)
// Only the EDGES of the rectangle are painted with 'color'.
// ============================================================================
void drawBox(int frame[SRC_H][SRC_W], Rect box, int color) {
    int left   = box.topLeft.x;
    int top    = box.topLeft.y;
    int right  = left + box.width  - 1;
    int bottom = top  + box.height - 1;

    for (int y = top; y <= bottom; y++) {
        for (int x = left; x <= right; x++) {
            bool onEdge = (x == left || x == right || y == top || y == bottom);
            Point p = { x, y };
            if (onEdge && isInside(p, SRC_W, SRC_H)) {
                frame[y][x] = color;
            }
        }
    }
}

// ============================================================================
// Example C: Find the BRIGHTEST pixel and report its coordinates
// (used for auto-exposure, laser-spot tracking, etc.)
// ============================================================================
Point findBrightest(const int frame[SRC_H][SRC_W]) {
    Point best = { 0, 0 };
    for (int y = 0; y < SRC_H; y++) {
        for (int x = 0; x < SRC_W; x++) {
            if (frame[y][x] > frame[best.y][best.x]) {
                best.x = x;
                best.y = y;
            }
        }
    }
    return best;
}

// ---- Printing helpers -------------------------------------------------------
void printSrc(const char* title, const int f[SRC_H][SRC_W]) {
    cout << "\n" << title << "\n";
    for (int y = 0; y < SRC_H; y++) {
        for (int x = 0; x < SRC_W; x++) {
            cout << (f[y][x] < 10 ? "  " : (f[y][x] < 100 ? " " : "")) << f[y][x] << " ";
        }
        cout << "\n";
    }
}

void printDst(const char* title, const int f[DST_H][DST_W]) {
    cout << "\n" << title << "\n";
    for (int y = 0; y < DST_H; y++) {
        for (int x = 0; x < DST_W; x++) {
            cout << (f[y][x] < 10 ? "  " : (f[y][x] < 100 ? " " : "")) << f[y][x] << " ";
        }
        cout << "\n";
    }
}

int main() {
    // Build an 8x8 grayscale (luma only) test frame: a diagonal gradient.
    // brightness = (x + y) * 15  -> dark in the top-left, bright bottom-right
    int frame[SRC_H][SRC_W];
    for (int y = 0; y < SRC_H; y++) {
        for (int x = 0; x < SRC_W; x++) {
            frame[y][x] = (x + y) * 15;
        }
    }
    printSrc("Original 8x8 frame (brightness 0-255):", frame);

    // A: scale down
    int small[DST_H][DST_W];
    scaleFrame(frame, small);
    printDst("Scaled to 4x4 (nearest neighbor):", small);

    // C: brightest pixel
    Point hot = findBrightest(frame);
    cout << "\nBrightest pixel is at (x=" << hot.x << ", y=" << hot.y
         << ") with value " << frame[hot.y][hot.x] << "\n";

    // B: draw a box around a "detected object"
    Rect detection = { {2, 1}, 4, 3 };   // top-left (2,1), 4 wide, 3 tall
    drawBox(frame, detection, 255);
    printSrc("Frame with a detection box drawn (value 255):", frame);

    return 0;
}
