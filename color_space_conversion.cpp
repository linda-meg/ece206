// ============================================================================
// Project 1: RGB -> YCbCr Color Space Conversion (Video Engineering)
// ----------------------------------------------------------------------------
// WHY THIS MATTERS:
//   Cameras capture RGB (Red, Green, Blue), but almost every video system
//   (broadcast TV, H.264/HEVC streaming, Blu-ray) stores video as YCbCr:
//     Y  = luma (brightness)       -> human eyes are very sensitive to this
//     Cb = blue-difference chroma  -> eyes are less sensitive to color detail
//     Cr = red-difference chroma
//   Separating brightness from color lets encoders throw away some color
//   detail (chroma subsampling) and save bandwidth.
//
// CONCEPTS PRACTICED:
//   - 1D arrays  : one pixel = {R, G, B}
//   - 2D arrays  : the 3x3 conversion matrix
//   - 3D arrays  : a full frame = [rows][columns][channels]
//   - Nested loops and a hierarchy of functions:
//       convertFrame()  -> loops over every row
//         convertRow()  -> loops over every pixel in that row
//           convertPixel() -> does the actual math on one pixel
// ============================================================================

#include <iostream>
#include <iomanip>

using namespace std;

// ---- Frame size (tiny so the output fits on screen) -----------------------
const int HEIGHT   = 4;   // number of rows    (y direction)
const int WIDTH    = 4;   // number of columns (x direction)
const int CHANNELS = 3;   // R,G,B  or  Y,Cb,Cr

// ---- BT.709 conversion matrix (the HDTV standard) ---------------------------
// Each row of the matrix produces one output channel:
//   Y  = 0.2126*R + 0.7152*G + 0.0722*B
//   Cb = -0.1146*R - 0.3854*G + 0.5000*B
//   Cr = 0.5000*R - 0.4542*G - 0.0458*B
const double RGB_TO_YCBCR[3][3] = {
    {  0.2126,  0.7152,  0.0722 },   // Y  row
    { -0.1146, -0.3854,  0.5000 },   // Cb row
    {  0.5000, -0.4542, -0.0458 }    // Cr row
};

// Cb and Cr can be negative after the math, so we add 128 to shift them
// into the 0..255 range used by 8-bit video.
const double OFFSET[3] = { 0.0, 128.0, 128.0 };

// ---- Helper: keep a value inside 0..255 (an 8-bit pixel) --------------------
int clamp255(double value) {
    if (value < 0)   return 0;
    if (value > 255) return 255;
    return (int)(value + 0.5);       // round to nearest whole number
}

// ============================================================================
// LEVEL 3 (lowest): convert ONE pixel
//   in[]  = {R, G, B}
//   out[] = {Y, Cb, Cr}
// This is a matrix-times-vector multiply done with two nested loops.
// ============================================================================
void convertPixel(const int in[CHANNELS], int out[CHANNELS]) {
    for (int outCh = 0; outCh < CHANNELS; outCh++) {        // Y, then Cb, then Cr
        double sum = OFFSET[outCh];
        for (int inCh = 0; inCh < CHANNELS; inCh++) {       // R, G, B
            sum += RGB_TO_YCBCR[outCh][inCh] * in[inCh];
        }
        out[outCh] = clamp255(sum);
    }
}

// ============================================================================
// LEVEL 2: convert ONE ROW of pixels (calls convertPixel for each column)
// ============================================================================
void convertRow(const int rgbRow[WIDTH][CHANNELS], int ycbcrRow[WIDTH][CHANNELS]) {
    for (int x = 0; x < WIDTH; x++) {
        convertPixel(rgbRow[x], ycbcrRow[x]);
    }
}

// ============================================================================
// LEVEL 1 (top): convert a WHOLE FRAME (calls convertRow for each row)
// ============================================================================
void convertFrame(const int rgb[HEIGHT][WIDTH][CHANNELS],
                  int ycbcr[HEIGHT][WIDTH][CHANNELS]) {
    for (int y = 0; y < HEIGHT; y++) {
        convertRow(rgb[y], ycbcr[y]);
    }
}

// ---- Print a frame as a grid of (a,b,c) triplets ---------------------------
void printFrame(const char* title, const char* labels,
                const int frame[HEIGHT][WIDTH][CHANNELS]) {
    cout << "\n" << title << "  (each cell = " << labels << ")\n";
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            cout << "(" << setw(3) << frame[y][x][0] << ","
                        << setw(3) << frame[y][x][1] << ","
                        << setw(3) << frame[y][x][2] << ") ";
        }
        cout << "\n";
    }
}

int main() {
    // A 4x4 test frame, like the color bars engineers use to test equipment.
    // Row 0: white, yellow, cyan, green
    // Row 1: magenta, red, blue, black
    // Row 2: a left-to-right gray ramp
    // Row 3: a left-to-right red ramp
    int rgbFrame[HEIGHT][WIDTH][CHANNELS] = {
        { {255,255,255}, {255,255,  0}, {  0,255,255}, {  0,255,  0} },
        { {255,  0,255}, {255,  0,  0}, {  0,  0,255}, {  0,  0,  0} },
        { {  0,  0,  0}, { 85, 85, 85}, {170,170,170}, {255,255,255} },
        { {  0,  0,  0}, { 85,  0,  0}, {170,  0,  0}, {255,  0,  0} }
    };

    int ycbcrFrame[HEIGHT][WIDTH][CHANNELS];   // output goes here

    convertFrame(rgbFrame, ycbcrFrame);

    printFrame("INPUT  RGB frame",   "R,G,B",    rgbFrame);
    printFrame("OUTPUT YCbCr frame", "Y,Cb,Cr",  ycbcrFrame);

    // Things to notice in the output:
    //  * Grays (row 2) all have Cb = Cr = 128 -> "no color", only brightness.
    //  * Green looks brightest in Y because the eye is most sensitive to green
    //    (0.7152 is the biggest number in the Y row of the matrix).
    //  * Pure blue has a high Cb; pure red has a high Cr.
    return 0;
}
