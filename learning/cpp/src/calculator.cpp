#include <iostream>

// ==== Globals ====
const int SUCCESS = 0;
const int ZERO_DIVISION_ERROR = 1;
const int INVALID_INPUT_ERROR = 2;


// ==== Helpers ====
double get_double(char var) {
    std::cout << "Enter a double for " << var << ": ";
    double d {};
    std::cin >> d;

    if (std::cin.fail()) {
        std::cout << "\nError: Invalid value: '" << d << "'\n";
        exit(INVALID_INPUT_ERROR);
    }
    return d;
}

char get_operator() {
    std::cout << "Enter operator (+, -, *, or /): ";
    char op {};
    std::cin >> op;
    return op;
}

void print(double val) {
    std::cout << "\nResult:\t" << val << '\n';
}

int main() {
    double num1 { get_double('x') };
    double num2 { get_double('y') };
    char op { get_operator() };

    if (op == '+') print(num1 + num2);
    else if (op == '-') print(num1 - num2);
    else if (op == '*') print(num1 * num2);
    else if (op == '/') {
        if (num2 != 0) print(num1 / num2);
        else {
            std::cout << "\nError: Cannot divide by zero.\n";
            exit(ZERO_DIVISION_ERROR);
        }
    }
    else {
        std::cout << "\nError: Invalid operator: '"
                  << op << "'\n";
        exit(INVALID_INPUT_ERROR);
    }

    exit(SUCCESS);
}
