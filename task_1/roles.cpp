#include <random>
#include <ctime>
#include <vector>
#include <utility>
#include <unordered_map>
#include <string>

#include "shared_ptr.cpp"

std::mt19937 generator;   // global generator
std::uniform_int_distribution<> distribution;   // global distribution


class Action {
public:
    struct promise_type {
        Action get_return_object() {
            return Action{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        void return_void() {}
        void unhandled_exception() {}
    };
    std::coroutine_handle<promise_type> handle;
    ~Action() { handle.destroy(); }
};

class Role {
public:
    virtual ~Role() = default;
    virtual Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const = 0;
    virtual Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const = 0;
    virtual void set_role(int num) const = 0;
    virtual std::string get_role() const = 0;
    virtual bool is_alive() const = 0;
    virtual bool is_evil() const = 0;
    virtual void set_target(int target) const = 0;
    virtual int get_target() const = 0;
    virtual int get_vote_target() const = 0;
    virtual void dead() const = 0;
};

//---//

class Holder : public Role {
public:
    int number = -1;
    std::string role_name = "";
    bool alive = true;
    int target = -1;
    int vote_target = -1;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        co_return;
    }
    void set_role(int num) const override {
        const_cast<Holder*>(this)->number = num;
        const_cast<Holder*>(this)->role_name = "Holder";
    }
    std::string get_role() const override {
        return const_cast<Holder*>(this)->role_name;
    }
    bool is_alive() const override{
        return const_cast<Holder*>(this)->alive;
    }
    bool is_evil() const override{
        return false;
    }
    void set_target(int target) const override {}
    int get_target() const override {
        return const_cast<Holder*>(this)->target;
    }
    int get_vote_target() const override {
        return const_cast<Holder*>(this)->vote_target;
    }
    void dead() const override {}
};

class Doctor : public Role {
    bool evil = false;
    int target = -1;
    int prev = -1;
    int number = -1;
    std::string role_name = "";
    bool alive = true;
    int vote_target = -1;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        int tmp_prev = const_cast<Doctor*>(this)->prev;
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id || tmp_prev == tmp_target) tmp_target = distribution(generator);
        const_cast<Doctor*>(this)->target = tmp_target;
        const_cast<Doctor*>(this)->prev = tmp_target;
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        const_cast<Doctor*>(this)->vote_target = tmp_target;
        co_return;
    }
    void set_role(int num) const override {
        const_cast<Doctor*>(this)->evil = false;
        const_cast<Doctor*>(this)->number = num;
        const_cast<Doctor*>(this)->role_name = "Doctor";
    }
    std::string get_role() const override {
        return const_cast<Doctor*>(this)->role_name;
    }
    bool is_alive() const override {
        return const_cast<Doctor*>(this)->alive;
    }
    bool is_evil() const override {
        return const_cast<Doctor*>(this)->evil;
    }
    void set_target(int target) const override {
        const_cast<Doctor*>(this)->target = target;
    }
    int get_target() const override {
        return const_cast<Doctor*>(this)->target;
    }
    int get_vote_target() const override {
        return const_cast<Doctor*>(this)->vote_target;
    }
    void dead() const override {
        const_cast<Doctor*>(this)->alive = false;
    }
};

class Sherif : public Role {
public:
    bool evil = false;
    int target = -1;
    int mafia_known = -1;
    int number = -1;
    std::string role_name = "";
    bool alive = true;
    int vote_target = -1;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        if (const_cast<Sherif*>(this)->mafia_known != -1 && (*roles[const_cast<Sherif*>(this)->mafia_known]).is_alive()) {
            const_cast<Sherif*>(this)->target = const_cast<Sherif*>(this)->mafia_known;
            co_return;
        } else if (const_cast<Sherif*>(this)->mafia_known != -1 && !(*roles[const_cast<Sherif*>(this)->mafia_known]).is_alive()) {
            const_cast<Sherif*>(this)->mafia_known = -1;
        }
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        if ((*roles[tmp_target]).is_evil()) const_cast<Sherif*>(this)->mafia_known = tmp_target;
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        if (mafia_known != -1) {
            const_cast<Sherif*>(this)->vote_target = mafia_known;
            co_return;
        }
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        const_cast<Sherif*>(this)->vote_target = tmp_target;
        co_return;
    }
    void set_role(int num) const override {
        const_cast<Sherif*>(this)->evil = false;
        const_cast<Sherif*>(this)->number = num;
        const_cast<Sherif*>(this)->role_name = "Sherif";
    }
    std::string get_role() const override {
        return const_cast<Sherif*>(this)->role_name;
    }
    bool is_alive() const override{
        return const_cast<Sherif*>(this)->alive;
    }
    bool is_evil() const override{
        return const_cast<Sherif*>(this)->evil;
    }
    void set_target(int target) const override {
        const_cast<Sherif*>(this)->target = target;
    }
    int get_target() const override {
        return const_cast<Sherif*>(this)->target;
    }
    int get_vote_target() const override {
        return const_cast<Sherif*>(this)->vote_target;
    }
    void dead() const override {
        const_cast<Sherif*>(this)->alive = false;
    }
};

class Mafia : public Role {
public:
    bool evil = false;
    int target = -1;
    int number = -1;
    std::string role_name = "";
    bool alive = true;
    int vote_target = -1;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id || (*roles[tmp_target]).is_evil()) tmp_target = distribution(generator);
        const_cast<Mafia*>(this)->target = tmp_target;
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id || roles[tmp_target]->is_evil()) tmp_target = distribution(generator);
        const_cast<Mafia*>(this)->vote_target = tmp_target;
        co_return;
    }
    void set_role(int num) const override {
        const_cast<Mafia*>(this)->evil = true;
        const_cast<Mafia*>(this)->role_name = "Mafia";
        const_cast<Mafia*>(this)->number = num;
    }
    std::string get_role() const override {
        return const_cast<Mafia*>(this)->role_name;
    }
    bool is_alive() const override{
        return const_cast<Mafia*>(this)->alive;
    }
    bool is_evil() const override{
        return const_cast<Mafia*>(this)->evil;
    }
    void set_target(int target) const override {
        const_cast<Mafia*>(this)->target = target;
    }
    int get_target() const override {
        return const_cast<Mafia*>(this)->target;
    }
    int get_vote_target() const override {
        return const_cast<Mafia*>(this)->vote_target;
    }
    void dead() const override {
        const_cast<Mafia*>(this)->alive = false;
    }
};

class Villager : public Role {
public:
    bool evil = false;
    int number = -1;
    std::string role_name = "";
    bool alive = true;
    int vote_target = -1;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        const_cast<Villager*>(this)->vote_target = tmp_target;
        co_return;
    }
    void set_role(int num) const override {
        const_cast<Villager*>(this)->evil = false;
        const_cast<Villager*>(this)->number = num;
        const_cast<Villager*>(this)->role_name = "Villager";
    }
    std::string get_role() const override {
        return const_cast<Villager*>(this)->role_name;
    }
    bool is_alive() const override{
        return const_cast<Villager*>(this)->alive;
    }
    bool is_evil() const override{
        return const_cast<Villager*>(this)->evil;
    }
    void set_target(int target) const override {
        const_cast<Villager*>(this)->vote_target = target;
    }
    int get_target() const override {
        return const_cast<Villager*>(this)->vote_target;
    }
    int get_vote_target() const override {
        return const_cast<Villager*>(this)->vote_target;
    }
    void dead() const override {
        const_cast<Villager*>(this)->alive = false;
    }
};

class Maniac : public Role {
public:
    bool evil = false;
    int target = -1;
    int number = -1;
    std::string role_name = "";
    bool alive = true;
    int vote_target = -1;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        const_cast<Maniac*>(this)->target = tmp_target;
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        const_cast<Maniac*>(this)->vote_target = tmp_target;
        co_return;
    }
    void set_role(int num) const override {
        const_cast<Maniac*>(this)->evil = false;
        const_cast<Maniac*>(this)->number = num;
        const_cast<Maniac*>(this)->role_name = "Maniac";
    }
    std::string get_role() const override {
        return const_cast<Maniac*>(this)->role_name;
    }
    bool is_alive() const override{
        return const_cast<Maniac*>(this)->alive;
    }
    bool is_evil() const override{
        return const_cast<Maniac*>(this)->evil;
    }
    void set_target(int target) const override {
        const_cast<Maniac*>(this)->target = target;
    }
    int get_target() const override {
        return const_cast<Maniac*>(this)->target;
    }
    int get_vote_target() const override {
        return const_cast<Maniac*>(this)->vote_target;
    }
    void dead() const override {
        const_cast<Maniac*>(this)->alive = false;
    }
};

// extra

class Lier : public Role {
public:
    bool alive = true;
    bool evil = true;
    int target = -1;
    int vote_target = -1;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {co_return;}
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {co_return;}
    void set_role(int num) const override {}
    std::string get_role() const override {return "";}
    bool is_alive() const override{
        return const_cast<Lier*>(this)->alive;
    }
    bool is_evil() const override{
        return const_cast<Lier*>(this)->evil;
    }
    void set_target(int target) const override {
        const_cast<Lier*>(this)->target = target;
    }
    int get_target() const override {
        return const_cast<Lier*>(this)->target;
    }
    int get_vote_target() const override {
        return const_cast<Lier*>(this)->vote_target;
    }
    void dead() const override {
        const_cast<Lier*>(this)->alive = false;
    }
};

class Drunker : public Role {
public:
    bool alive = true;
    bool evil = false;
    int target = -1;
    int vote_target = -1;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {co_return;}
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {co_return;}
    void set_role(int num) const override {}
    std::string get_role() const override {return "";}
    bool is_alive() const override{
        return const_cast<Drunker*>(this)->alive;
    }
    bool is_evil() const override{
        return const_cast<Drunker*>(this)->evil;
    }
    void set_target(int target) const override {
        const_cast<Drunker*>(this)->target = target;
    }
    int get_target() const override {
        return const_cast<Drunker*>(this)->target;
    }
    int get_vote_target() const override {
        return const_cast<Drunker*>(this)->vote_target;
    }
    void dead() const override {
        const_cast<Drunker*>(this)->alive = false;
    }
};

class Bully : public Role {
public:
    bool alive = true;
    bool evil = false;
    int target = -1;
    int vote_target = -1;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {co_return;}
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {co_return;}
    void set_role(int num) const override {}
    std::string get_role() const override {return "";}
    bool is_alive() const override{
        return const_cast<Bully*>(this)->alive;
    }
    bool is_evil() const override{
        return const_cast<Bully*>(this)->evil;
    }
    void set_target(int target) const override {
        const_cast<Bully*>(this)->target = target;
    }
    int get_target() const override {
        return const_cast<Bully*>(this)->target;
    }
    int get_vote_target() const override {
        return const_cast<Bully*>(this)->vote_target;
    }
    void dead() const override {
        const_cast<Bully*>(this)->alive = false;
    }
};