
#include <iostream>
#include <string>

using namespace std;

/*
    ============================================================
                GRADE CONVERTER
    ============================================================

    This program converts a numerical grade from 0 to 100
    into a letter grade.

    Example:

        95  -> A
        91  -> A-
        88  -> B+
        84  -> B
        72  -> C-
        55  -> F

    Concepts demonstrated:

        - Variables
        - User input using cin
        - Output using cout
        - if statements
        - else if statements
        - else statements
        - Comparison operators
        - Logical operators
        - Input validation
        - Strings
    ============================================================
*/

int main()
{
    // ---------------------------------------------------------
    // STEP 1: DECLARE VARIABLES
    // ---------------------------------------------------------

    // "grade" stores the numerical grade entered by the user.
    //
    // We use double instead of int because a grade might
    // contain a decimal value.
    //
    // Examples:
    //      95
    //      89.5
    //      72.75

    double grade;

    // "letterGrade" will store the final converted grade.
    //
    // We use string because some grades contain two characters,
    // such as A-, B+, or C+.

    string letterGrade;


    // ---------------------------------------------------------
    // STEP 2: DISPLAY THE PROGRAM TITLE
    // ---------------------------------------------------------

    cout << "=================================" << endl;
    cout << "        GRADE CONVERTER          " << endl;
    cout << "=================================" << endl;

    cout << endl;


    // ---------------------------------------------------------
    // STEP 3: ASK THE USER FOR A NUMERICAL GRADE
    // ---------------------------------------------------------

    cout << "Enter a numerical grade (0-100): ";
    cin >> grade;


    // ---------------------------------------------------------
    // STEP 4: CHECK FOR INVALID INPUT
    // ---------------------------------------------------------
    //
    // A valid grade must be between 0 and 100.
    //
    // The || operator means OR.
    //
    // Therefore:
    //
    //     grade < 0 || grade > 100
    //
    // means:
    //
    //     "The grade is below 0 OR above 100."
    //
    // If either condition is true, the grade is invalid.
    // ---------------------------------------------------------

    if (grade < 0 || grade > 100)
    {
        cout << endl;
        cout << "ERROR: Grade must be between 0 and 100."
             << endl;
    }


    // ---------------------------------------------------------
    // STEP 5: CONVERT THE NUMBER TO A LETTER GRADE
    // ---------------------------------------------------------
    //
    // The program checks the grade from HIGHEST to LOWEST.
    //
    // This order is VERY important.
    //
    // As soon as one condition is true, C++ executes that
    // section and skips the remaining else-if statements.
    // ---------------------------------------------------------

    else if (grade >= 93)
    {
        letterGrade = "A";
    }

    else if (grade >= 90)
    {
        letterGrade = "A-";
    }

    else if (grade >= 87)
    {
        letterGrade = "B+";
    }

    else if (grade >= 83)
    {
        letterGrade = "B";
    }

    else if (grade >= 80)
    {
        letterGrade = "B-";
    }

    else if (grade >= 77)
    {
        letterGrade = "C+";
    }

    else if (grade >= 73)
    {
        letterGrade = "C";
    }

    else if (grade >= 70)
    {
        letterGrade = "C-";
    }

    else if (grade >= 67)
    {
        letterGrade = "D+";
    }

    else if (grade >= 63)
    {
        letterGrade = "D";
    }

    else if (grade >= 60)
    {
        letterGrade = "D-";
    }

    else
    {
        // If none of the conditions above are true,
        // then the grade must be below 60.

        letterGrade = "F";
    }


    // ---------------------------------------------------------
    // STEP 6: DISPLAY THE RESULT
    // ---------------------------------------------------------
    //
    // We only display a letter grade if the numerical grade
    // entered by the user was valid.
    // ---------------------------------------------------------

    if (grade >= 0 && grade <= 100)
    {
        cout << endl;
        cout << "Numerical Grade: " << grade << endl;
        cout << "Letter Grade:    " << letterGrade << endl;
    }


    // ---------------------------------------------------------
    // STEP 7: END THE PROGRAM
    // ---------------------------------------------------------

    cout << endl;
    cout << "Program finished." << endl;

    return 0;
}
