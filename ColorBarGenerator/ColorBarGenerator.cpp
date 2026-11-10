#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>

using namespace std;

// RGB color
struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// Write a 24-bit uncompressed BMP file
bool saveBMP(const string& filename, int width, int height)
{
    // 8 vertical color bars
    Color colors[8] =
    {
        {255, 255, 255},   // White
        {255, 255,   0},   // Yellow
        {  0, 255, 255},   // Cyan
        {  0, 255,   0},   // Green
        {255,   0, 255},   // Magenta
        {255,   0,   0},   // Red
        {  0,   0, 255},   // Blue
        {  0,   0,   0}    // Black
    };

    // Each BMP row must be aligned to 4 bytes
    int rowSize = (width * 3 + 3) & (~3);
    int pixelDataSize = rowSize * height;
    int fileSize = 54 + pixelDataSize;

    ofstream file(filename, ios::binary);

    if (!file)
    {
        cerr << "Error: Could not create BMP file.\n";
        return false;
    }

    // ------------------------------------------------
    // BMP FILE HEADER - 14 bytes
    // ------------------------------------------------

    uint8_t fileHeader[14] = {};

    fileHeader[0] = 'B';
    fileHeader[1] = 'M';

    fileHeader[2] = fileSize;
    fileHeader[3] = fileSize >> 8;
    fileHeader[4] = fileSize >> 16;
    fileHeader[5] = fileSize >> 24;

    // Pixel data begins after the 54-byte headers
    fileHeader[10] = 54;

    file.write(reinterpret_cast<char*>(fileHeader), 14);

    // ------------------------------------------------
    // BMP INFORMATION HEADER - 40 bytes
    // ------------------------------------------------

    uint8_t infoHeader[40] = {};

    infoHeader[0] = 40;

    // Width
    infoHeader[4] = width;
    infoHeader[5] = width >> 8;
    infoHeader[6] = width >> 16;
    infoHeader[7] = width >> 24;

    // Height
    infoHeader[8]  = height;
    infoHeader[9]  = height >> 8;
    infoHeader[10] = height >> 16;
    infoHeader[11] = height >> 24;

    // Number of color planes
    infoHeader[12] = 1;

    // Bits per pixel = 24
    infoHeader[14] = 24;

    file.write(reinterpret_cast<char*>(infoHeader), 40);

    // ------------------------------------------------
    // Create the image
    // ------------------------------------------------

    vector<uint8_t> row(rowSize, 0);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Determine which of the 8 bars this pixel belongs to
            int bar = (x * 8) / width;

            if (bar > 7)
                bar = 7;

            Color c = colors[bar];

            // BMP stores colors as BGR, not RGB
            row[x * 3]     = c.b;
            row[x * 3 + 1] = c.g;
            row[x * 3 + 2] = c.r;
        }

        file.write(reinterpret_cast<char*>(row.data()), rowSize);
    }

    file.close();

    return true;
}

int main()
{
    int choice;

    cout << "========================================\n";
    cout << "       COLOR BAR BMP GENERATOR\n";
    cout << "========================================\n\n";

    cout << "Select the desired resolution:\n\n";

    cout << "1. 720p   (1280 x 720)\n";
    cout << "2. 1080p  (1920 x 1080)\n";
    cout << "3. 1920p  (3840 x 1920)\n\n";

    cout << "Enter your choice (1-3): ";
    cin >> choice;

    int width;
    int height;
    string filename;

    switch (choice)
    {
    case 1:
        width = 1280;
        height = 720;
        filename = "CBAR720.bmp";
        break;

    case 2:
        width = 1920;
        height = 1080;
        filename = "CBAR1080.bmp";
        break;

    case 3:
        width = 3840;
        height = 1920;
        filename = "CBAR192.bmp";
        break;

    default:
        cout << "\nInvalid selection.\n";
        cout << "Please run the program again and select 1, 2, or 3.\n";
        return 1;
    }

    cout << "\nGenerating color bars...\n";
    cout << "Resolution: " << width
         << " x " << height << endl;

    if (saveBMP(filename, width, height))
    {
        cout << "\nBMP generated successfully!\n";
        cout << "Filename: " << filename << endl;
        cout << "Resolution: "
             << width << " x " << height << endl;
        cout << "Format: 24-bit uncompressed BMP\n";
    }
    else
    {
        cout << "\nFailed to generate BMP file.\n";
        return 1;
    }

    return 0;
}
