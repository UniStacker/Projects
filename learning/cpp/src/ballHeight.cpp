#include <iostream>

const double GRAVITY { 9.8 };
bool hit { false };

// Returns validated height from the user.
double get_height() {
    std::cout << "Enter the height of tower in meters: ";
    double height {};
    std::cin >> height;

    if (height <= 0 || std::cin.fail()) {
        std::cout << "Error: Invalid height: " << height;
        exit(EXIT_FAILURE);
    }
    return height;
}

// Clips the val between start and end.
double clip(double val, double start, double end) {
    if (val >= start && val <= end) return val;
    else if (val < start) return start;
    else return end;
}

// Returns the distance of ball from ground at x seconds
double calculate_distance(double height, int x_sec) {
    double fallen = GRAVITY * x_sec * x_sec / 2; // meters
    return clip(height - fallen, 0, height);
}

// Prints the height of ball from ground at x seconds
void print_at(int x_sec, double height) {
    double distance { calculate_distance(height, x_sec) };
    std::cout << "At " << x_sec << " seconds, the ball ";

    if (distance != 0)
        std::cout << "is at height: " << distance << " meters.\n";
    else if (!hit) {
        std::cout << "hit the ground.\n";
        hit = true;
    } else
        std::cout << "was already at the ground.\n";
}

int main() {
    double height { get_height() };

    print_at(0, height);
    print_at(1, height);
    print_at(2, height);
    print_at(3, height);
    print_at(4, height);
    print_at(5, height);

    return 0;
}
