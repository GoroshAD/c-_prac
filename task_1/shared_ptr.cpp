#include <iostream>

template <typename T>
class SharedPtr {
private:
    T* ptr;
    int* ref_count;
public:
    //constructors
    SharedPtr() : ptr(nullptr), ref_count(nullptr) {}
    SharedPtr(T* ptr) : ptr(ptr), ref_count(new int(1)) {}
    SharedPtr(const SharedPtr& other) : ptr(other.ptr), ref_count(other.ref_count) {
        if (ref_count) {
            ++(*ref_count);
        }
    }
    SharedPtr(SharedPtr&& other) : ptr(other.ptr), ref_count(other.ref_count) {
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }

    ~SharedPtr() {
        if (ptr && --(*ref_count) == 0) {
            if constexpr (std::is_array_v<T>) {
                delete[] ptr;
            } else {
                delete ptr;
            }
            delete ref_count;
        }
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            if (ptr && --(*ref_count) == 0) {
                if constexpr (std::is_array_v<T>) {
                    delete[] ptr;
                } else {
                    delete ptr;
                }
                delete ref_count;
            }
            ptr = other.ptr;
            ref_count = other.ref_count;
            ++(*ref_count);
        }
        return *this;
    }

    T& operator*() const {
        return *ptr;
    }
    T* operator->() const {
        return ptr;
    }

    void reset(T* new_ptr = nullptr) {
        if (ptr && --(*ref_count) == 0) {
            if constexpr (std::is_array_v<T>) {
                delete[] ptr;
            } else {
                delete ptr;
            }
            delete ref_count;
        }
        ptr = new_ptr;
        if (new_ptr) {
            ref_count = new int(1);
        } else {
            ref_count = nullptr;
        }
    }

    void swap(SharedPtr& other) {
        std::swap(ptr, other.ptr);
        std::swap(ref_count, other.ref_count);
    }

    T* get() const {
        return ptr;
    }

    bool operator==(const SharedPtr& other) const {
        return ptr == other.ptr;
    }
    bool operator!=(const SharedPtr& other) const {
        return ptr != other.ptr;
    }
};