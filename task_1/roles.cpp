#include <random>
#include <ctime>
#include <vector>
#include <utility>
#include <unordered_map>
#include <string>

#include "shared_ptr.cpp"

std::mt19937 generator;   // global generator
std::uniform_int_distribution<> distribution;   // global distribution

class Role {
public:
    virtual ~Role() = default;
    virtual void act(std::unordered_map<int, SharedPtr<Role>> &roles) const = 0;
    virtual void vote(std::unordered_map<int, SharedPtr<Role>> &roles) const = 0;
    virtual void set_role(int num) const = 0;
    virtual std::string get_role() const = 0;
};

//---//

class Holder : public Role {
public:
    int number = -1;
    std::string role_name = "";
    bool alive = true;
    void act(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void vote(std::unordered_map<int, SharedPtr<Role>> &roles) const override {
        // here'll be walking around the SharedPtr<Role>s
    }
    void set_role(int num) const override {
        const_cast<Holder*>(this)->number = num;
        const_cast<Holder*>(this)->role_name = "Holder";
    }
    std::string get_role() const override {
        return const_cast<Holder*>(this)->role_name;
    }
};

class Doctor : public Role {
    bool sleep = false;
    bool evil = false;
    int target = -1;
    int prev = -1;
    int number = -1;
    std::string role_name = "";
    bool alive = true;

    void act(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void vote(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void set_role(int num) const override {
        const_cast<Doctor*>(this)->sleep = false;
        const_cast<Doctor*>(this)->evil = false;
        const_cast<Doctor*>(this)->number = num;
        const_cast<Doctor*>(this)->role_name = "Doctor";
    }
    std::string get_role() const override {
        return const_cast<Doctor*>(this)->role_name;
    }
};

class Sherif : public Role {
public:
    bool sleep = false;
    bool evil = false;
    int target = -1;
    int number = -1;
    std::string role_name = "";
    bool alive = true;

    void act(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void vote(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void set_role(int num) const override {
        const_cast<Sherif*>(this)->sleep = false;
        const_cast<Sherif*>(this)->evil = false;
        const_cast<Sherif*>(this)->number = num;
        const_cast<Sherif*>(this)->role_name = "Sherif";
    }
    std::string get_role() const override {
        return const_cast<Sherif*>(this)->role_name;
    }
};

class Mafia : public Role {
public:
    bool sleep = false;
    bool evil = false;
    int target = -1;
    int number = -1;
    std::string role_name = "";
    bool alive = true;

    void act(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void vote(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void set_role(int num) const override {
        const_cast<Mafia*>(this)->sleep = false;
        const_cast<Mafia*>(this)->evil = true;
        const_cast<Mafia*>(this)->role_name = "Mafia";
        const_cast<Mafia*>(this)->number = num;
    }
    std::string get_role() const override {
        return const_cast<Mafia*>(this)->role_name;
    }
};

class Villager : public Role {
public:
    bool sleep = false;
    bool evil = false;
    int number = -1;
    std::string role_name = "";
    bool alive = true;

    void act(std::unordered_map<int, SharedPtr<Role>> &roles) const override {return;}
    void vote(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void set_role(int num) const override {
        const_cast<Villager*>(this)->sleep = true;
        const_cast<Villager*>(this)->evil = false;
        const_cast<Villager*>(this)->number = num;
        const_cast<Villager*>(this)->role_name = "Villager";
    }
    std::string get_role() const override {
        return const_cast<Villager*>(this)->role_name;
    }
};

class Maniac : public Role {
public:
    bool sleep = false;
    bool evil = false;
    int target = -1;
    int number = -1;
    std::string role_name = "";
    bool alive = true;

    void act(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void vote(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void set_role(int num) const override {
        const_cast<Maniac*>(this)->sleep = false;
        const_cast<Maniac*>(this)->evil = false;
        const_cast<Maniac*>(this)->number = num;
        const_cast<Maniac*>(this)->role_name = "Maniac";
    }
    std::string get_role() const override {
        return const_cast<Maniac*>(this)->role_name;
    }
};

// extra

class Lier : public Role {
public:
    void act(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void vote(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void set_role(int num) const override {}
    std::string get_role() const override {return "";}
};

class Drunker : public Role {
public:
    void act(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void vote(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void set_role(int num) const override {}
    std::string get_role() const override {return "";}
};

class Bully : public Role {
public:
    void act(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void vote(std::unordered_map<int, SharedPtr<Role>> &roles) const override {}
    void set_role(int num) const override {}
    std::string get_role() const override {return "";}
};