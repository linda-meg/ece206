#include <iostream>   // Provides input/output functionality (cin, cout)
using namespace std;  // Allows us to use cin/cout without std:: prefix

int main() {

    int max_value;  // This will store the upper limit entered by the user

    // Ask the user for input
    cout << "Display primes up to what value? ";
    cin >> max_value;  // Read the maximum value

    int value = 2;  // Start checking from 2 (the smallest prime number)

    // Outer loop: checks every number from 2 up to max_value
    while (value <= max_value) {

        // ----------------------------------------
        // Step 1: Assume the current number is prime
        // ----------------------------------------
        bool is_prime = true;  // We assume it's prime until proven otherwise

        // ----------------------------------------
        // Step 2: Try to find a factor of 'value'
        // ----------------------------------------
        // A number is NOT prime if it has any divisor other than 1 and itself
        // We test all possible factors from 2 up to (value - 1)
        int trial_factor = 2;

        while (trial_factor < value) {

            // Check if trial_factor divides value evenly
            if (value % trial_factor == 0) {
                // If remainder is 0 → we found a factor → NOT prime
                is_prime = false;

                // No need to continue checking other factors
                break;
            }

            // Move to the next possible factor
            trial_factor++;
        }

        // ----------------------------------------
        // Step 3: If still prime, print it
        // ----------------------------------------
        if (is_prime) {
            cout << value << " ";  // Display the prime number
        }

        // ----------------------------------------
        // Step 4: Move to the next number
        // ----------------------------------------
        value++;
    }

    // Move to the next line after printing all primes
    cout << endl;

    return 0;  // Indicate successful program execution
}
