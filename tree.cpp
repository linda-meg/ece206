#include <iostream>   // Allows input and output operations (cin, cout)
using namespace std;  // Avoids writing std:: before cin, cout, etc.

int main() {

    int height;  // Variable to store the height of the tree

    // Prompt the user to enter the height
    cout << "Enter height of tree: ";
    cin >> height;  // Read user input

    int row = 0;  // Initialize row counter (starting from the top of the tree)

    // Outer loop: runs once for each row of the tree
    while (row < height) {

        // -------------------------------
        // Step 1: Print leading spaces
        // -------------------------------
        // These spaces center the tree
        // As row increases, spaces decrease
        int count = 0;

        while (count < height - row) {
            cout << " ";  // Print a space
            count++;      // Increment counter
        }

        // -------------------------------
        // Step 2: Print stars (*)
        // -------------------------------
        // Number of stars in each row:
        // Formula: (2 * row + 1)
        //
        // Example:
        // row = 0 → 1 star
        // row = 1 → 3 stars
        // row = 2 → 5 stars
        //
        // This creates a symmetric pyramid shape

        count = 0;  // Reset counter for star printing

        while (count < 2 * row + 1) {
            cout << "*";  // Print a star
            count++;      // Increment counter
        }

        // -------------------------------
        // Step 3: Move to next line
        // -------------------------------
        cout << endl;

        // -------------------------------
        // Step 4: Move to next row
        // -------------------------------
        row++;
    }

    return 0;  // Indicate successful program execution
}
