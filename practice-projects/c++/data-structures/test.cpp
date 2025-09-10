#include "./01-array/array.hpp"
#include <iostream>

int main() {
    Array<int> arr = {1, 2, 3, 4, 5};
    Array<int> arr2 = arr;
    arr2[-1] = 6;
    arr2.reverse();

    Array<int> sum = arr + arr2;

    std::cout << "Original: " << arr << '\n';
    std::cout << "Reversed: " << arr2 << '\n';
    std::cout << "Sum: " << sum << '\n';
    std::cout << "Slice[3:7]: " << sum.slice(3, 7) << '\n';

    return 0;
}
