#include <random>
#include <ctime>
#include <vector>
#include <utility>
#include <unordered_map>
#include <string>
#include <coroutine>

#include "shared_ptr.cpp"
#include "log.cpp"

std::mt19937 generator;   // global generator
std::uniform_int_distribution<> distribution;   // global distribution
Logger logger;
int bully = -1;
int lier = -1;
int liers_treatment = -1;
int bullies_treatment = -1;
int drunkers_treatment = -1;
int drunker = -1;

bool drunkers_bullied = false;   // stupid constant

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
    virtual Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id, int liers_treat) const = 0;
    virtual Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const = 0;
    virtual void set_role(int num) const = 0;
    virtual std::string get_role() const = 0;
    virtual bool is_alive() const = 0;
    virtual bool is_evil() const = 0;
    virtual void set_target(int target) const = 0;
    virtual int get_target() const = 0;
    virtual int get_vote_target() const = 0;
    virtual void set_vote_target(int target) const = 0;
    virtual void dead() const = 0;

    virtual void bullys_treat() const = 0;
    virtual bool was_bully() const = 0;
    virtual int get_slander_target() const = 0;
    virtual void set_slander_target(int target) const = 0;
};

//---//

class Holder : public Role {
public:
    int number = -1;
    std::string role_name = "";
    bool alive = true;
    int target = -1;
    int vote_target = -1;

    bool bullys_treatment = false;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id, int liers_treat) const override {
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
    void set_target(int target) const override {
        const_cast<Holder*>(this)->target = target;
    }
    void set_vote_target(int target) const override {
        const_cast<Holder*>(this)->vote_target = target;
    }
    int get_target() const override {
        return const_cast<Holder*>(this)->target;
    }
    int get_vote_target() const override {
        return const_cast<Holder*>(this)->vote_target;
    }
    void dead() const override {
        const_cast<Holder*>(this)->alive = false;
    }

    void bullys_treat() const override {
        const_cast<Holder*>(this)->bullys_treatment = true;
    }
    bool was_bully() const override {
        return const_cast<Holder*>(this)->bullys_treatment;
    }
    int get_slander_target() const override {
        return -1;
    }
    void set_slander_target(int target) const override {}
};

class Doctor : public Role {
    bool evil = false;
    int target = -1;
    int prev = -1;
    int number = -1;
    std::string role_name = "";
    bool alive = true;
    int vote_target = -1;

    bool bullys_treatment = false;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id, int liers_treat) const override {
        if (!(*this).is_alive()) co_return;
        logger.night_logging("The Doctor got up (player " + std::to_string(number) + ").");
        int tmp_target = distribution(generator);
        int tmp_prev = const_cast<Doctor*>(this)->prev;
        while (!(*roles[tmp_target]).is_alive() || tmp_prev == tmp_target) tmp_target = distribution(generator);
        const_cast<Doctor*>(this)->target = tmp_target;
        const_cast<Doctor*>(this)->prev = tmp_target;
        logger.night_logging("The Doctor chose player number " + std::to_string(target) + " and went to check his health.");
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        const_cast<Doctor*>(this)->vote_target = tmp_target;
        logger.day_logging("The Doctor (player " + std::to_string(number) + ") voted for player number " + std::to_string(vote_target) + ".");
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
    void set_vote_target(int target) const override {
        const_cast<Doctor*>(this)->vote_target = target;
    }
    void dead() const override {
        const_cast<Doctor*>(this)->alive = false;
    }

    void bullys_treat() const override {
        const_cast<Doctor*>(this)->bullys_treatment = true;
    }
    bool was_bully() const override {
        return const_cast<Doctor*>(this)->bullys_treatment;
    }
    int get_slander_target() const override {
        return -1;
    }
    void set_slander_target(int target) const override {}
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

    bool bullys_treatment = false;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id, int liers_treat) const override {
        if (!(*this).is_alive()) co_return;
        logger.night_logging("The Sherif got up (player " + std::to_string(number) + ").");
        if (const_cast<Sherif*>(this)->mafia_known != -1 && (*roles[const_cast<Sherif*>(this)->mafia_known]).is_alive()) {
            logger.night_logging("The Sherif knows, that player number " + std::to_string(mafia_known) + " is from mafia family. So he tries to assault him...");
            const_cast<Sherif*>(this)->target = const_cast<Sherif*>(this)->mafia_known;
            co_return;
        } else if (const_cast<Sherif*>(this)->mafia_known != -1 && !(*roles[const_cast<Sherif*>(this)->mafia_known]).is_alive()) {
            const_cast<Sherif*>(this)->mafia_known = -1;
        }
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        if ((*roles[tmp_target]).is_evil() || tmp_target == liers_treat) {
            const_cast<Sherif*>(this)->mafia_known = tmp_target;
            logger.night_logging("The Sherif checked player " + std::to_string(tmp_target) + " and knows that he is from mafia family.");
        } else {
            logger.night_logging("The Sherif checked player " + std::to_string(tmp_target) + " and knows that he is not from mafia family.");
        }
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        if (mafia_known != -1 && !roles[mafia_known]->is_alive()) {
            const_cast<Sherif*>(this)->mafia_known = -1;
        } else if (mafia_known != -1) {
            const_cast<Sherif*>(this)->vote_target = mafia_known;
            logger.day_logging("The Sherif (player " + std::to_string(number) + ") voted for player number " + std::to_string(vote_target) + " (because he knows, who mafia is).");
            co_return;
        }
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        const_cast<Sherif*>(this)->vote_target = tmp_target;
        logger.day_logging("The Sherif (player " + std::to_string(number) + ") voted for player number " + std::to_string(vote_target) + ".");
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
    void set_vote_target (int target) const override {
        const_cast<Sherif*>(this)->vote_target = target;
    }
    void dead() const override {
        const_cast<Sherif*>(this)->alive = false;
    }

    void bullys_treat() const override {
        const_cast<Sherif*>(this)->bullys_treatment = true;
    }
    bool was_bully() const override {
        return const_cast<Sherif*>(this)->bullys_treatment;
    }
    int get_slander_target() const override {
        return -1;
    }
    void set_slander_target(int target) const override {}
};

class Mafia : public Role {
public:
    bool evil = false;
    int target = -1;
    int number = -1;
    std::string role_name = "";
    bool alive = true;
    int vote_target = -1;

    bool bullys_treatment = false;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id, int liers_treat) const override {
        if (!(*this).is_alive()) co_return;
        logger.night_logging("The Mafia got up (player " + std::to_string(number) + ").");
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id || (*roles[tmp_target]).is_evil()) tmp_target = distribution(generator);
        const_cast<Mafia*>(this)->target = tmp_target;
        logger.night_logging("The Mafia chose player number " + std::to_string(target) + " and voted at mafia voting.");
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id || roles[tmp_target]->is_evil()) tmp_target = distribution(generator);
        const_cast<Mafia*>(this)->vote_target = tmp_target;
        logger.day_logging("The Mafia (player " + std::to_string(number) + ") voted for player number " + std::to_string(vote_target) + ".");
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
    void set_vote_target (int target) const override {
        const_cast<Mafia*>(this)->vote_target = target;
    }
    void dead() const override {
        const_cast<Mafia*>(this)->alive = false;
    }

    void bullys_treat() const override {
        const_cast<Mafia*>(this)->bullys_treatment = true;
    }
    bool was_bully() const override {
        return const_cast<Mafia*>(this)->bullys_treatment;
    }
    int get_slander_target() const override {
        return -1;
    }
    void set_slander_target(int target) const override {}
};

class Villager : public Role {
public:
    bool evil = false;
    int number = -1;
    std::string role_name = "";
    bool alive = true;
    int vote_target = -1;

    bool bullys_treatment = false;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id, int liers_treat) const override {
        if (!(*this).is_alive()) co_return;
        logger.night_logging("The Villager slept all night (player " + std::to_string(number) + ").");
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        const_cast<Villager*>(this)->vote_target = tmp_target;
        logger.day_logging("The Villager (player " + std::to_string(number) + ") voted for player number " + std::to_string(vote_target) + ".");
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
    void set_vote_target (int target) const override {
        const_cast<Villager*>(this)->vote_target = target;
    }
    void dead() const override {
        const_cast<Villager*>(this)->alive = false;
    }

    void bullys_treat() const override {
        const_cast<Villager*>(this)->bullys_treatment = true;
    }
    bool was_bully() const override {
        return const_cast<Villager*>(this)->bullys_treatment;
    }
    int get_slander_target() const override {
        return -1;
    }
    void set_slander_target(int target) const override {}
};

class Maniac : public Role {
public:
    bool evil = false;
    int target = -1;
    int number = -1;
    std::string role_name = "";
    bool alive = true;
    int vote_target = -1;

    bool bullys_treatment = false;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id, int liers_treat) const override {
        if (!(*this).is_alive()) co_return;
        logger.night_logging("The Maniac got up (player " + std::to_string(number) + ").");
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        const_cast<Maniac*>(this)->target = tmp_target;
        logger.night_logging("The Maniac chose player number " + std::to_string(target) + " and went to him.");
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        const_cast<Maniac*>(this)->vote_target = tmp_target;
        logger.day_logging("The Maniac (player " + std::to_string(number) + ") voted for player number " + std::to_string(vote_target) + ".");
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
    void set_vote_target (int target) const override {
        const_cast<Maniac*>(this)->vote_target = target;
    }
    void dead() const override {
        const_cast<Maniac*>(this)->alive = false;
    }

    void bullys_treat() const override {
        const_cast<Maniac*>(this)->bullys_treatment = true;
    }
    bool was_bully() const override {
        return const_cast<Maniac*>(this)->bullys_treatment;
    }
    int get_slander_target() const override {
        return -1;
    }
    void set_slander_target(int target) const override {}
};

// extra

class Lier : public Role {
public:
    bool alive = true;
    bool evil = true;
    int target = -1;
    int vote_target = -1;
    int number = -1;
    std::string role_name = "";

    int slander_target = -1;
    bool bullys_treatment = false;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id, int liers_treat) const override {
        const_cast<Lier*>(this)->slander_target = -1;
        if (!(*this).is_alive()) co_return;
        logger.night_logging("The Lier got up (player " + std::to_string(number) + ").");
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id || (*roles[tmp_target]).is_evil()) tmp_target = distribution(generator);
        const_cast<Lier*>(this)->slander_target = tmp_target;
        logger.night_logging("The Lier (player " + std::to_string(number) + ") chose player number " + std::to_string(slander_target) + " and lied about him.");
        
        tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id || (*roles[tmp_target]).is_evil()) tmp_target = distribution(generator);
        const_cast<Lier*>(this)->target = tmp_target;
        logger.night_logging("The Lier (player " + std::to_string(number) + ") voted for player number " + std::to_string(target) + " at mafia voting.");
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id || roles[tmp_target]->is_evil()) tmp_target = distribution(generator);
        const_cast<Lier*>(this)->vote_target = tmp_target;
        logger.day_logging("The Lier (player " + std::to_string(number) + ") voted for player number " + std::to_string(vote_target) + ".");
        co_return;
    }
    void set_role(int num) const override {
        const_cast<Lier*>(this)->number = num;
        const_cast<Lier*>(this)->role_name = "Lier";
        lier = num;
    }
    std::string get_role() const override {
        return const_cast<Lier*>(this)->role_name;
    }
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
    void set_vote_target (int target) const override {
        const_cast<Lier*>(this)->vote_target = target;
    }
    void dead() const override {
        lier = -1;
        const_cast<Lier*>(this)->alive = false;
    }

    void bullys_treat() const override {
        const_cast<Lier*>(this)->bullys_treatment = true;
    }
    bool was_bully() const override {
        return const_cast<Lier*>(this)->bullys_treatment;
    }
    int get_slander_target() const override {
        return const_cast<Lier*>(this)->slander_target;
    }
    void set_slander_target(int target) const override {
        const_cast<Lier*>(this)->slander_target = target;
    }
};

class Drunker : public Role {
public:
    bool alive = true;
    bool evil = false;
    int target = -1;
    int vote_target = -1;
    int number = -1;
    std::string role_name = "";

    bool bullys_treatment = false;
    int drunkers_strike = 0;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id, int liers_treat) const override {
        const_cast<Drunker*>(this)->target = -1;
        if (!(*this).is_alive()) co_return;
        logger.night_logging("The Drunker got up (player " + std::to_string(number) + ").");
        if (const_cast<Drunker*>(this)->drunkers_strike == 2) {
            const_cast<Drunker*>(this)->drunkers_strike = 0;
            logger.night_logging("The Drunker decided go to bed this night.");
            co_return;
        }
        std::uniform_int_distribution<> distrib(0, 1);
        if (distrib(generator) == 0) {
            const_cast<Drunker*>(this)->drunkers_strike = 0;
            logger.night_logging("The Drunker decided go to bed this night.");
            co_return;
        }
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive()) tmp_target = distribution(generator);
        const_cast<Drunker*>(this)->target = tmp_target;
        const_cast<Drunker*>(this)->drunkers_strike += 1;
        logger.night_logging("The Drunker chose player number " + std::to_string(target) + " and went to him for all night.");
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        const_cast<Drunker*>(this)->vote_target = tmp_target;
        logger.day_logging("The Drunker (player " + std::to_string(number) + ") voted for player number " + std::to_string(vote_target) + ".");
        co_return;
    }
    void set_role(int num) const override {
        const_cast<Drunker*>(this)->number = num;
        const_cast<Drunker*>(this)->role_name = "Drunker";
        drunker = num;
    }
    std::string get_role() const override {
        return const_cast<Drunker*>(this)->role_name;
    }
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
    void set_vote_target (int target) const override {
        const_cast<Drunker*>(this)->vote_target = target;
    }
    void dead() const override {
        const_cast<Drunker*>(this)->alive = false;
        drunker = -1;
    }

    void bullys_treat() const override {
        const_cast<Drunker*>(this)->bullys_treatment = true;
    }
    bool was_bully() const override {
        return const_cast<Drunker*>(this)->bullys_treatment;
    }
    int get_slander_target() const override {
        return -1;
    }
    void set_slander_target(int target) const override {}
};

class Bully : public Role {
public:
    bool alive = true;
    bool evil = false;
    int target = -1;
    int vote_target = -1;
    int number = -1;
    std::string role_name = "";

    bool bullys_treatment = false;

    Action act(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id, int liers_treat) const override {
        const_cast<Bully*>(this)->target = -1;
        if (!(*this).is_alive()) co_return;
        logger.night_logging("The Bully got up (player " + std::to_string(number) + ").");
        bool still_alive_not_bullied = false;
        for (int i = 1; i < num; ++i) {
            if (roles[i]->is_alive() && !roles[i]->was_bully()) {
                still_alive_not_bullied = true;
                break;
            }
        }
        if (!still_alive_not_bullied) {
            const_cast<Bully*>(this)->target = -1;
            logger.night_logging("The Bully already joked at everybody in this city, so he went to bed.");
            co_return;
        }
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || roles[tmp_target]->was_bully()) {
            tmp_target = distribution(generator);
        }
        const_cast<Bully*>(this)->target = tmp_target;
        logger.night_logging("The Bully chose player number " + std::to_string(target) + " and joked at him.");
        co_return;
    }
    Action vote(std::unordered_map<int, SharedPtr<Role>> &roles, int num, int id) const override {
        if (!(*this).is_alive()) co_return;
        int tmp_target = distribution(generator);
        while (!(*roles[tmp_target]).is_alive() || tmp_target == id) tmp_target = distribution(generator);
        const_cast<Bully*>(this)->vote_target = tmp_target;
        logger.day_logging("The Bully (player " + std::to_string(number) + ") voted for player number " + std::to_string(vote_target) + ".");
        co_return;
    }
    void set_role(int num) const override {
        const_cast<Bully*>(this)->number = num;
        const_cast<Bully*>(this)->role_name = "Bully";
        bully = num;
    }
    std::string get_role() const override {
        return const_cast<Bully*>(this)->role_name;
    }
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
    void set_vote_target (int target) const override {
        const_cast<Bully*>(this)->vote_target = target;
    }
    void dead() const override {
        const_cast<Bully*>(this)->alive = false;
        bully = -1;
    }

    void bullys_treat() const override {
        const_cast<Bully*>(this)->bullys_treatment = true;
    }
    bool was_bully() const override {
        return const_cast<Bully*>(this)->bullys_treatment;
    }
    void set_slander_target(int target) const override {}
    int get_slander_target() const override {
        return -1;
    }
};