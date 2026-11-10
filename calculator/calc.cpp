#include <iostream>   // Gives us access to cout and cin
using namespace std;

/*
    ============================================================
    SIMPLE C++ CALCULATOR
    ============================================================

    This program allows the user to:

        1. Enter two numbers
        2. Select an arithmetic operation
        3. See the calculated result

    Operations supported:

        +   Addition
        -   Subtraction
        *   Multiplication
        /   Division

    Concepts demonstrated:

        - Variables
        - User input using cin
        - Output using cout
        - Arithmetic operators
        - switch statements
        - if statements
        - Error checking
    ============================================================
*/

int main()
{
    // ---------------------------------------------------------
    // STEP 1: DECLARE VARIABLES
    // ---------------------------------------------------------

    // We use double instead of int so the calculator can work
    // with both whole numbers and decimal numbers.
    //
    // Examples:
    //     10
    //     25
    //     3.14
    //     7.5

    double number1;
    double number2;
    double result;

    // This variable stores the operator entered by the user.
    //
    // char means "character."
    //
    // Examples of characters:
    // '+'
    // '-'
    // '*'
    // '/'

    char operation;


    // ---------------------------------------------------------
    // STEP 2: DISPLAY A TITLE
    // ---------------------------------------------------------

    cout << "==============================" << endl;
    cout << "       SIMPLE CALCULATOR      " << endl;
    cout << "==============================" << endl;


    // ---------------------------------------------------------
    // STEP 3: ASK FOR THE FIRST NUMBER
    // ---------------------------------------------------------

    cout << "Enter the first number: ";

    // cin reads information typed by the user.
    // The value entered will be stored inside number1.

    cin >> number1;


    // ---------------------------------------------------------
    // STEP 4: ASK THE USER TO SELECT AN OPERATION
    // ---------------------------------------------------------

    cout << endl;
    cout << "Choose an operation:" << endl;
    cout << "+  Addition" << endl;
    cout << "-  Subtraction" << endl;
    cout << "*  Multiplication" << endl;
    cout << "/  Division" << endl;

    cout << endl;
    cout << "Enter operation: ";

    // Store the character entered by the user in operation.

    cin >> operation;


    // ---------------------------------------------------------
    // STEP 5: ASK FOR THE SECOND NUMBER
    // ---------------------------------------------------------

    cout << "Enter the second number: ";
    cin >> number2;


    // ---------------------------------------------------------
    // STEP 6: DETERMINE WHICH OPERATION TO PERFORM
    // ---------------------------------------------------------
    //
    // A switch statement is useful when one variable can have
    // several possible values.
    //
    // Here, we check the value stored in "operation."
    //
    // For example:
    //
    //     operation = '+'
    //
    // causes:
    //
    //     case '+':
    //
    // to execute.
    // ---------------------------------------------------------

    switch (operation)
    {

        // -----------------------------------------------------
        // ADDITION
        // -----------------------------------------------------

        case '+':

            // Add the two numbers and store the answer
            // inside the result variable.

            result = number1 + number2;

            cout << endl;
            cout << number1 << " + " << number2
                 << " = " << result << endl;

            // break tells C++ that we are finished with
            // this case and should leave the switch statement.

            break;


        // -----------------------------------------------------
        // SUBTRACTION
        // -----------------------------------------------------

        case '-':

            result = number1 - number2;

            cout << endl;
            cout << number1 << " - " << number2
                 << " = " << result << endl;

            break;


        // -----------------------------------------------------
        // MULTIPLICATION
        // -----------------------------------------------------

        case '*':

            result = number1 * number2;

            cout << endl;
            cout << number1 << " * " << number2
                 << " = " << result << endl;

            break;


        // -----------------------------------------------------
        // DIVISION
        // -----------------------------------------------------

        case '/':

            /*
                Before performing division, we MUST check whether
                the second number is zero.

                Division by zero is not mathematically defined.

                For example:

                    10 / 0

                should not be allowed.
            */

            if (number2 != 0)
            {
                // The denominator is not zero, so division
                // can safely be performed.

                result = number1 / number2;

                cout << endl;
                cout << number1 << " / " << number2
                     << " = " << result << endl;
            }
            else
            {
                // The user entered zero as the second number.

                cout << endl;
                cout << "ERROR: Cannot divide by zero." << endl;
            }

            break;


        // -----------------------------------------------------
        // INVALID OPERATION
        // -----------------------------------------------------

        default:

            /*
                The default section executes if none of the
                cases above match the user's input.

                For example, if the user enters:

                    %

                there is no case for %, so C++ executes default.
            */

            cout << endl;
            cout << "ERROR: Invalid operation." << endl;
            cout << "Please use +, -, *, or /." << endl;

            break;
    }


    // ---------------------------------------------------------
    // STEP 7: END THE PROGRAM
    // ---------------------------------------------------------

    cout << endl;
    cout << "Calculator program finished." << endl;

    /*
        Returning 0 tells the operating system that the program
        finished successfully.
    */

    return 0;
}
