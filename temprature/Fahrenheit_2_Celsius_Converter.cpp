
#include <iostream>
#include <iomanip>

using namespace std;

/*
    ============================================================
          FAHRENHEIT TO CELSIUS TEMPERATURE CONVERTER
    ============================================================

    This program asks the user to enter 5 temperatures
    in Fahrenheit.

    The temperatures are stored in an array.

    The program then:
        1. Reads the 5 Fahrenheit temperatures
        2. Stores them in an array
        3. Converts each temperature to Celsius
        4. Stores the Celsius temperatures in another array
        5. Displays the results

    Formula:

        Celsius = (Fahrenheit - 32) * 5 / 9

    Concepts demonstrated:

        - Arrays
        - for loops
        - User input using cin
        - Output using cout
        - Arithmetic calculations
        - Array indexing
        - Constants
*/


int main()
{
    // ---------------------------------------------------------
    // STEP 1: DEFINE THE NUMBER OF TEMPERATURES
    // ---------------------------------------------------------
    //
    // We will ask the user to enter exactly 5 temperatures.
    //
    // Using a constant makes the program easier to modify.
    // If we later want 10 temperatures instead of 5,
    // we only need to change this value.

    const int SIZE = 5;


    // ---------------------------------------------------------
    // STEP 2: CREATE THE ARRAYS
    // ---------------------------------------------------------
    //
    // The first array stores Fahrenheit temperatures.
    //
    // The second array stores the converted Celsius
    // temperatures.
    //
    // Both arrays contain 5 elements.

    double fahrenheit[SIZE];

    double celsius[SIZE];


    // ---------------------------------------------------------
    // STEP 3: DISPLAY THE PROGRAM TITLE
    // ---------------------------------------------------------

    cout << "==========================================" << endl;
    cout << "     FAHRENHEIT TO CELSIUS CONVERTER     " << endl;
    cout << "==========================================" << endl;

    cout << endl;


    // ---------------------------------------------------------
    // STEP 4: RECEIVE 5 TEMPERATURES FROM THE USER
    // ---------------------------------------------------------
    //
    // A for loop allows us to repeat the same instructions
    // multiple times.
    //
    // Instead of writing:
    //
    //     cin >> fahrenheit[0];
    //     cin >> fahrenheit[1];
    //     cin >> fahrenheit[2];
    //     cin >> fahrenheit[3];
    //     cin >> fahrenheit[4];
    //
    // we can use a loop.
    //
    // Notice that arrays begin at index 0.
    //
    // For an array containing 5 elements, the indexes are:
    //
    //     0   1   2   3   4
    //
    // There is NO fahrenheit[5] in this array.

    cout << "Enter 5 temperatures in Fahrenheit:" << endl;
    cout << endl;

    for (int i = 0; i < SIZE; i++)
    {
        cout << "Temperature " << (i + 1) << ": ";

        cin >> fahrenheit[i];
    }


    // ---------------------------------------------------------
    // STEP 5: CONVERT EACH TEMPERATURE TO CELSIUS
    // ---------------------------------------------------------
    //
    // We use another for loop to go through every element
    // in the Fahrenheit array.
    //
    // The conversion formula is:
    //
    //     Celsius = (Fahrenheit - 32) * 5 / 9
    //
    // Each Fahrenheit value is converted and stored at the
    // SAME index in the Celsius array.
    //
    // Example:
    //
    // Fahrenheit:
    //
    //     fahrenheit[0] = 32
    //
    // After conversion:
    //
    //     celsius[0] = 0
    //
    // Therefore, the two arrays correspond to each other.

    for (int i = 0; i < SIZE; i++)
    {
        celsius[i] = (fahrenheit[i] - 32.0) * 5.0 / 9.0;
    }


    // ---------------------------------------------------------
    // STEP 6: DISPLAY THE CONVERTED TEMPERATURES
    // ---------------------------------------------------------

    cout << endl;
    cout << "==========================================" << endl;
    cout << "              RESULTS                     " << endl;
    cout << "==========================================" << endl;

    cout << endl;


    // fixed and setprecision(2) display the Celsius
    // temperatures with two digits after the decimal point.
    //
    // Example:
    //
    //     37.00
    //
    // instead of:
    //
    //     37

    cout << fixed << setprecision(2);


    // ---------------------------------------------------------
    // Loop through the arrays again to display the results.
    // ---------------------------------------------------------

    for (int i = 0; i < SIZE; i++)
    {
        cout << "Temperature " << (i + 1) << ": "
             << fahrenheit[i] << " F = "
             << celsius[i] << " C"
             << endl;
    }


    // ---------------------------------------------------------
    // STEP 7: END THE PROGRAM
    // ---------------------------------------------------------

    cout << endl;
    cout << "Conversion complete." << endl;

    return 0;
}
