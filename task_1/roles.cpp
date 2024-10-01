#include <random>
#include <ctime>
#include <vector>
#include <utility>
#include <unordered_map>

#include "shared_ptr.cpp"

std::mt19937 generator;   // global generator
std::uniform_int_distribution<> distribution;   // global distribution

class Role {
public:
    Role() : sleep(true), evil(false) {}
    Role(bool sleep, bool evil) : sleep(sleep), evil(evil) {}

    ~Role() {
        sleep = true;
        evil = false;
    }

    bool sleep;
    bool evil;
};

//---//

template <typename T>
class Holder : Role {
public:
    Holder() : Role(false, false), living(nullptr), roles(nullptr) {}
    Holder(T *ptr, T *ptr2) : Role(false, false), living(ptr), roles(ptr2) {}

    ~Holder() {
        if (std::is_array_v<T>) {
            delete[] living;
        } else {
            delete living;
        }
    }

    T *living;
    T *roles;
};

class Doctor : Role {
public:
    Doctor() : Role(false, false), prev(0), target(0) {}

    ~Doctor() {}

    int
    act() {   // simple realization
        target = distribution(generator);
        while (target == prev or false) {   // here is also Holders answer that target is alive or not
            target = distribution(generator);
        }
        prev = target;
        return target;
    }

    int
    vote() {
        int target = distribution(generator);
        while (false) {   // is it alive
            target = distribution(generator);
        }
        return target;
    }

    int prev;
    int target;
};

template <typename T>
class Sherif : Role {
public:
    Sherif() : Role(false, false), target(0), memory(nullptr) {}
    Sherif(T *ptr) : Role(false, false), target(0), memory(ptr) {}

    ~Sherif() {
        if (std::is_array_v<T>) {
            delete[] memory;
        } else {
            delete memory;
        }
    }

    int
    act() {
        target = distribution(generator);

        // smth adding to memory

        return target;
    }

    int
    vote() {
        int target = distribution(generator);
        while (false) {   // is it alive   +   memory
            target = distribution(generator);
        }
        return target;
    }

    int target;
    T *memory;
};

template <typename T>
class Mafia : Role {
public:
    Mafia() : Role(false, true), target(0), allies(nullptr) {}
    Mafia(T *ptr) : Role(false, true), target(0), allies(ptr) {}

    ~Mafia() {
        if (std::is_array_v<T>) {
            delete[] allies;
        } else {
            delete allies;
        }
    }

    int
    act() {
        target = distribution(generator);
        
        // smth

        return target;
    }

    int
    vote() {
        int target = distribution(generator);
        while (false) {   // is it alive   +   not evil 
            target = distribution(generator);
        }
        return target;
    }

    int target;
    T *allies;
};

class Villager : Role {
public:
    Villager() : Role(true, false) {}
    
    ~Villager() {}

    int
    vote() {
        int target = distribution(generator);
        while (false) {   // is it alive
            target = distribution(generator);
        }
        return target;
    }
};

class Maniac : Role {
public:
    Maniac() : Role(false, false), target(0) {}

    ~Maniac() {}

    int
    act() {
        target = distribution(generator);
        while (false) {   // holder's answer is target alive now
            target = distribution(generator);
        }
        return target;
    }

    int
    vote() {
        int target = distribution(generator);
        while (false) {   // is it alive
            target = distribution(generator);
        }
        return target;
    }

    int target;
};

// extra

class Lier : Role {
public:
    Lier() : Role(false, true), target(0) {}

    ~Lier() {}

    int
    act() {
        target = distribution(generator);
        while (false) {   // is it alive
            target = distribution(generator);
        }
        return target;
    }

    int
    vote() {
        int target = distribution(generator);
        while (false) {   // is it alive
            target = distribution(generator);
        }
        return target;
    }

    int target;
};

class Drunker : Role {
public:
    Drunker() : Role(false, false), target(0), strike(0) {}

    ~Drunker() {}

    int
    act() {
        std::uniform_int_distribution<> distrib(0, 1);
        if (strike != 2 && distrib(generator)) {
            target = distribution(generator);
            while (false) {   // is it alive
                target = distribution(generator);
            }
            strike++;
            return target;
        } else {
            strike = 0;
        }
        return -1;
    }

    int
    vote() {
        int target = distribution(generator);
        while (false) {   // is it alive
            target = distribution(generator);
        }
        return target;
    }

    int target;
    int strike;
};

template <typename T>
class Bully : Role {
public:
    Bully() : Role(false, false), target(0), memory(nullptr) {}
    Bully(T *ptr) : Role(false, false), target(0), memory(ptr) {}

    ~Bully() {
        if (std::is_array_v<T>) {
            delete[] memory;
        } else {
            delete memory;
        }
    }

    int
    act() {
        int target = distribution(generator);
        while (false) {   // is it alive   +   not in memory
            target = distribution(generator);
        }
        return target;
    }

    int
    vote() {
        int target = distribution(generator);
        while (false) {   // is it alive
            target = distribution(generator);
        }
        return target;
    }

    int target;
    T *memory;
};



template<typename T>
class Player {
public:
    Player() : number(0), thoughts(nullptr), role(NULL) {}
    Player(int num, SharedPtr<std::unordered_map<int, std::string>> tho, T rol) {
        number = num;
        thoughts = tho;
        role = rol;
    }

    ~Player() {
        delete thoughts;
    }

    int number;
    SharedPtr<std::unordered_map<int, std::string>> thoughts;
    T role;
};