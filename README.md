# Simple C++ Calculator

## Overview

This project is a simple calculator written in C++.

The program asks the user to enter two numbers and select a mathematical operation. It then performs the calculation and displays the result.

This project is designed for students who are learning the basic concepts of C++ programming.

## Supported Operations

The calculator supports the following operations:

- `+` Addition
- `-` Subtraction
- `*` Multiplication
- `/` Division

## C++ Concepts Covered

This program demonstrates several important C++ programming concepts:

- Variables
- `double` data type
- `char` data type
- User input with `cin`
- Console output with `cout`
- Arithmetic operators
- `switch` statements
- `case` statements
- `if/else` statements
- The `break` statement
- Basic error checking
- Division-by-zero protection
- Code comments

## How the Program Works

The program follows these steps:

1. Display the calculator title.
2. Ask the user to enter the first number.
3. Ask the user to select an operation.
4. Ask the user to enter the second number.
5. Use a `switch` statement to determine which operation was selected.
6. Perform the calculation.
7. Display the result.
8. End the program.

## Example Run

```text
==============================
       SIMPLE CALCULATOR
==============================

Enter the first number: 25

Choose an operation:
+  Addition
-  Subtraction
*  Multiplication
/  Division

Enter operation: *
Enter the second number: 4

25 * 4 = 100

Calculator program finished.
```

## Understanding the Variables

The program uses the following variables:

```cpp
double number1;
double number2;
double result;
char operation;
```

### `number1`

Stores the first number entered by the user.

### `number2`

Stores the second number entered by the user.

### `result`

Stores the result of the calculation.

### `operation`

Stores the mathematical operation selected by the user.

For example:

```text
+
-
*
/
```

A `char` variable is used because each operator is a single character.

## Why Do We Use `double
