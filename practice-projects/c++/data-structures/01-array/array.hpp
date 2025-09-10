#pragma once
#include <iostream>
#include <cstdlib>

#define EXIT_ALLOCFAIL 1
#define EXIT_INVALIDINDEX 2

void error_allocFail(std::string reason) {
    std::cout << "Error: memory allocation failed for " << reason << '\n';
    exit(EXIT_ALLOCFAIL);
}

void error_invalidIndex(int idx, std::string reason) {
    std::cout << "Error: array index out of bounds (" << idx << ") while " << reason << '\n';
    exit(EXIT_INVALIDINDEX);
}

template<typename T>
class Array {
private:
    T* data;
    size_t len;

    int parse_index(int& idx) const {
        if (idx < 0 && idx + len >= 0) return idx + len;
        return idx;
    }

    void allocate(size_t elements) {
        try {
            data = new T[elements]();
        } catch (const std::bad_alloc&) {
            error_allocFail("array initialization");
        }
    }

public:
    /* Constructors */
    Array(size_t len): len(len) {
        allocate(len);
    }

    Array(const Array<T>& other): len(other.len) {
        allocate(len);
        for (size_t i = 0; i < len; i++) data[i] = other.data[i];
    }

    Array(std::initializer_list<T> list): len(list.size()) {
        allocate(len);
        size_t i = 0;
        for (auto& val : list) data[i++] = val;
    }

    /* Destructor */
    ~Array() {
        delete[] data;
    }

    /* Methods */
    void put(int idx, T val) {
        idx = parse_index(idx);
        if (idx < 0 || static_cast<size_t>(idx) >= len) error_invalidIndex(idx, "putting");
        data[idx] = val;
    }

    T get(int idx) const {
        idx = parse_index(idx);
        if (idx < 0 || static_cast<size_t>(idx) >= len) error_invalidIndex(idx, "getting");
        return data[idx];
    }

    void fill(T val) {
        for (size_t i = 0; i < len; i++) data[i] = val;
    }

    void reverse() {
        for (size_t i = 0; i < len / 2; i++) {
            T temp = data[i];
            data[i] = data[len - 1 - i];
            data[len - 1 - i] = temp;
        }
    }

    Array<T> slice(int start, int end = 0) {
        start = parse_index(start);
        if (start < 0 || static_cast<size_t>(start) >= len) error_invalidIndex(start, "slicing from start");

        if (end == 0) end = len;
        end = parse_index(end);
        if (end < 0 || static_cast<size_t>(end) > len) error_invalidIndex(end, "slicing to end");
        
        Array<T> arr_slice(end - start);
        for (int i=start; i<end; i++) arr_slice[i - start] = get(i);

        return arr_slice;
    }

    size_t length() const { return len; }

    T* begin() { return data; }
    T* end() { return data + len; }
    const T* begin() const { return data; }
    const T* end() const { return data + len; }

    /* Operator overloads */
    T& operator[](int idx) {
        idx = parse_index(idx);
        if (idx < 0 || static_cast<size_t>(idx) >= len) error_invalidIndex(idx, "subscript access");
        return data[idx];
    }

    const T& operator[](int idx) const {
        idx = parse_index(idx);
        if (idx < 0 || static_cast<size_t>(idx) >= len) error_invalidIndex(idx, "const subscript access");
        return data[idx];
    }

    Array<T> operator+(const Array<T>& other) const {
        Array<T> result(len + other.len);
        for (size_t i = 0; i < len; i++) result[i] = data[i];
        for (size_t i = 0; i < other.len; i++) result[i + len] = other[i];
        return result;
    }

    Array<T>& operator=(const Array<T>& other) {
        if (this == &other) return *this;
        delete[] data;
        len = other.len;
        allocate(len);
        for (size_t i = 0; i < len; i++) data[i] = other.data[i];
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Array<T>& arr) {
        os << '[';
        for (size_t i = 0; i < arr.len; i++) {
            os << arr[i];
            if (i != arr.len - 1) os << ", ";
        }
        os << ']';
        return os;
    }
};
