#include <stdio.h>
#include <stdlib.h>
#include <coroutine>
#include <string>
#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <thread>
#include <sstream>
#include <type_traits>
#include <future>

#include "roles.cpp"


void player_night_activity(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra);
void player_day_voting(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra);
void holders_night_checker(std::unordered_map<int, SharedPtr<Role>>& roles, int num);
void holders_day_checker(std::unordered_map<int, SharedPtr<Role>>& roles, int num);
void roles_creating(int num, bool extra, std::unordered_map<int, SharedPtr<Role>>& roles);
void night_is_coming(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, bool player);
void day_is_coming(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, bool player);
std::string are_we_finished(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, bool player);
void finishing_and_results(std::unordered_map<int, SharedPtr<Role>>& roles, bool extra, bool player);

int number_players = 0;   // global players number
std::string result = "no";   // global results
std::vector<std::pair<std::string, bool>> main_roles = {{"Mafia", false}, \
                                                   {"Sherif", false}, \
                                                   {"Doctor", false}, \
                                                   {"Psycho", false}, \
                                                   {"Villager", false}};
std::vector<std::pair<std::string, bool>> extra_roles = {{"Lier", false}, \
                                                         {"Bully", false}, \
                                                         {"Drunker", false}};
int player_doctors_prev = -1;

//---functions---
void
roles_creating(int num, bool extra, std::unordered_map<int, SharedPtr<Role>>& roles)
{
    roles[0] = SharedPtr<Role>(new Holder());
    roles[0]->set_role(0);
    int evil_quan = num / 3;
    int roles_quan = 5;
    int good_quan = num - 4 - evil_quan;
    if (extra) {
        roles_quan += 3;
        good_quan = num - 6 - evil_quan;
        evil_quan -= 1;   // for Lier
    }
    std::uniform_int_distribution<> distrib(0, roles_quan - 1);
    for (int i = 1; i < number_players; ++i) {
        while (true) {
            int role = distrib(generator);
            if (role >= 5 && !extra_roles[role - 5].second) {
                switch (extra_roles[role - 5].first[0]) {
                    case 'L' :
                        roles[i] = SharedPtr<Role>(new Lier());
                        break;
                    case 'B' :
                        roles[i] = SharedPtr<Role>(new Bully());
                        break;
                    default :
                        roles[i] = SharedPtr<Role>(new Drunker());
                }
                extra_roles[role - 5].second = true;
                roles[i]->set_role(i);
                break;
            } else if (role < 5 && !main_roles[role].second) {
                if (main_roles[role].first == "Mafia" && evil_quan == 0) continue;
                if (main_roles[role].first == "Villager" && good_quan == 0) continue;
                if (main_roles[role].first != "Mafia" && main_roles[role].first != "Villager") {
                    main_roles[role].second = true;
                }
                if (main_roles[role].first == "Mafia") --evil_quan;
                if (main_roles[role].first == "Villager") --good_quan;
                switch (main_roles[role].first[0]) {
                    case 'M' :
                        roles[i] = SharedPtr<Role>(new Mafia());
                        break;
                    case 'S' :
                        roles[i] = SharedPtr<Role>(new Sherif());
                        break;
                    case 'D' :
                        roles[i] = SharedPtr<Role>(new Doctor());
                        break;
                    case 'P' :
                        roles[i] = SharedPtr<Role>(new Maniac());
                        break;
                    default :
                        roles[i] = SharedPtr<Role>(new Villager());
                }
                roles[i]->set_role(i);
                break;
            }
        }
    }
    return;
}

void 
player_night_activity(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra) {
    int tmp_target = -1;
    if (!(*roles[1]).is_alive()) {
        std::cout << "You're died, so you can't do anything this night." << std::endl;
        return;
    }
    if ((*roles[1]).get_role() == "Sherif") {
        std::cout << "You woke up this night..." << std::endl;
        std::string action;
        std::cout << "Please choose your action, type assault or check: ";
        std::cin >> action;
        while (action != "assault" && action != "check") {
            std::cout << "Please type assault or check: ";
            std::cin >> action;
        }
        if (action == "check") {
            std::cout << "Please type a number of a player, which you want to check: ";
            std::cin >> tmp_target;
            while (tmp_target <= 0 || tmp_target >= num || !(*roles[tmp_target]).is_alive()) {
                std::cout << "This playerisn't alive, please try again: ";
                std::cin >> tmp_target;
            }
            if ((*roles[tmp_target]).is_evil()) {
                std::cout << "This player is evil." << std::endl; 
            } else {
                std::cout << "This player is good." << std::endl;
            }
        } else {
            std::cout << "Please type a number of a player, which you want to kill: ";
            std::cin >> tmp_target;
            while (tmp_target <= 0 || tmp_target >= num || !(*roles[tmp_target]).is_alive()) {
                std::cout << "This player isn't alive, please try again: ";
                std::cin >> tmp_target;
            }
            (*roles[1]).set_target(tmp_target);
        }
        std::cout << "Now it's time to have a sleep." << std::endl;
    } else if ((*roles[1]).get_role() == "Doctor") {
        std::cout << "You woke up this night..." << std::endl;
        std::cout << "Choose a target, please: ";
        std::cin >> tmp_target;
        while (tmp_target >= num || tmp_target < 1 || !(*roles[tmp_target]).is_alive()) {
            if (player_doctors_prev == tmp_target) {
                std::cout << "You went to this player yesterday, please select another one: ";
                std::cin >> tmp_target;
            } else{
                std::cout << "This player isn't alive, please try again: ";
                std::cin >> tmp_target;
            }
        }
    } else if ((*roles[1]).get_role() != "Villager") {
        std::cout << "You woke up this night..." << std::endl;
        std::cout << "Choose a target, please: ";
        std::cin >> tmp_target;
        while (tmp_target >= num || tmp_target < 1 || !(*roles[tmp_target]).is_alive()) {
            std::cout << "This player isn't alive, please try again: ";
            std::cin >> tmp_target;
        }
        (*roles[1]).set_target(tmp_target);
        std::cout << "Now it's time to have a sleep." << std::endl;
    } else {
        std::cout <<  "You slept all night." << std::endl;
    }
    return;
}
void
player_day_voting(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra) {

}

void
holders_night_checker(std::unordered_map<int, SharedPtr<Role>>& roles, int num)
{
    int sherifs_target = -1;
    std::unordered_map<int, int> mafias_targets;
    int doctors_target = -1;
    int maniacs_target = -1;

    for (int i = 1; i < num; ++i) {
        if (!(*roles[i]).is_alive()) continue;
        std::string role = (*roles[i]).get_role();
        if (role == "Villager") {
            continue;
        } else if (role == "Doctor") {
            doctors_target = (*roles[i]).get_target();
        } else if (role == "Sherif") {
            sherifs_target = (*roles[i]).get_target();
        } else if (role == "Maniac") {
            maniacs_target = (*roles[i]).get_target();
        } else if (role == "Mafia") {
            if (mafias_targets[(*roles[i]).get_target()] && mafias_targets[(*roles[i]).get_target()] >= 1) {
                mafias_targets[(*roles[i]).get_target()] += 1;
            } else {
                mafias_targets[(*roles[i]).get_target()] = 1;
            }
        }
    }

    int maxx = -1;
    int max_ind = -1;
    for (auto i : mafias_targets) {
        if (maxx < i.second) {
            maxx = i.second;
            max_ind = i.first;
        }
    }
    int mafias_target = max_ind;

    if (mafias_target != -1 && mafias_target != doctors_target) {
        std::cout << "Mafia killed player number " << mafias_target << " tonight. He was a " << (*roles[mafias_target]).get_role() << "." << std::endl;
        (*roles[mafias_target]).dead();
    } else if (mafias_target != -1) {
        std::cout << "Someone tried to kill player " << mafias_target << " tonight. Luckily, Doctor saved him from his death." << std::endl;
    }
    if (maniacs_target != -1 && maniacs_target != doctors_target && maniacs_target != mafias_target) {
        std::cout << "Maniac killed player number " << maniacs_target << " tonight. He was a " << (*roles[maniacs_target]).get_role() << "." << std::endl;
        (*roles[maniacs_target]).dead();
    } else if (maniacs_target != -1 && maniacs_target != mafias_target) {
        std::cout << "Someone tried to kill player " << mafias_target << " tonight. Luckily, Doctor saved him from his death." << std::endl;
    }
    if (sherifs_target != -1 && sherifs_target != doctors_target && maniacs_target != sherifs_target) {
        std::cout << "Sherif killed player number " << sherifs_target << " tonight. He was a " << (*roles[sherifs_target]).get_role() << "." << std::endl;
        (*roles[sherifs_target]).dead();
    } else if (sherifs_target != -1 && sherifs_target != maniacs_target) {
        std::cout << "Someone tried to kill player " << mafias_target << " tonight. Luckily, Doctor saved him from his death." << std::endl;
    }
    if (mafias_target != doctors_target && maniacs_target != doctors_target && sherifs_target != doctors_target && doctors_target != -1) {
        std::cout << "Doctor went to player " << doctors_target << " tonight." << std::endl;
    }
    return;
}

void
holders_day_checker(std::unordered_map<int, SharedPtr<Role>>& roles, int num)
{
    return;
}

void
night_is_coming(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, bool player) 
{
    std::cout << "The night is coming... The city goes to sleep..." << std::endl;
    for (int i = 0; i < num; ++i) {
        std::string alive = "not alive";
        if ((*roles[i]).is_alive()) alive = "alive";
        std::cout << i << " " << alive << std::endl;
    }
    std::vector<std::future<void>> futures;
    
    for (const auto& [i, bot] : roles) {
        if (player && i == 1) {
            player_night_activity(roles, num, extra); 
        } else {
            futures.push_back(std::async(std::launch::async, [&]() {
                Action action = bot->act(roles, num, i);
                action.handle.resume();
            }));
        }
    }
    for (auto& future : futures) {
        future.get();
    }
    futures.clear();

    // holders time
    holders_night_checker(roles, num);

    result = are_we_finished(roles, num, extra, player);
    if (result == "no") day_is_coming(roles, num, extra, player);
    return;
}

void
day_is_coming(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, bool player)
{
    std::cout << "The day is coming... Good morning, our citizens!" << std::endl;
    for (int i = 0; i < num; ++i) {
        std::string alive = "not alive";
        if ((*roles[i]).is_alive()) alive = "alive";
        std::cout << i << " " << alive << std::endl;
    }

    // holders time
    holders_day_checker(roles, num);

    result = are_we_finished(roles, num, extra, player);
    if (result == "no") night_is_coming(roles, num, extra, player);
    return;
}

std::string
are_we_finished(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, bool player)
{
    int mafias_alive = 0;
    int good_alive = 0;
    for (int i = 1; i < num; ++i) {
        if ((*roles[i]).is_alive() && (*roles[i]).is_evil()) {
            ++mafias_alive;
        } else if (roles[i]->is_alive()){
            ++good_alive;
        }
    }
    if (good_alive <= mafias_alive) return "Mafia";
    int maniac_alive = 0;
    mafias_alive = 0;
    good_alive = 0;
    for (int i = 1; i < num; ++i) {
        if ((*roles[i]).is_alive() && (*roles[i]).get_role() == "Maniac") {
            ++maniac_alive;
        } else if ((*roles[i]).is_alive() && (*roles[i]).is_evil()) {
            ++mafias_alive;
        } else {
            ++good_alive;
        }
    }
    if (good_alive <= maniac_alive && mafias_alive == 0) return "Psycho";
    if (mafias_alive == 0 && maniac_alive == 0) return "Villager";
    return "no";
}

void
finishing_and_results(std::unordered_map<int, SharedPtr<Role>>& roles, bool extra, bool player)
{
    std::cout << "finishing" << std::endl;
    switch (result[0]) {
        case 'M':
            std::cout << "Everybody will remember the day, when Mafia gets its victory and rules the city!" << std::endl;
            if (player && (*roles[1]).is_evil() && (*roles[1]).is_alive()) {
                std::cout << "Congratulations, our new family member, you did a great job for the family." << std::endl;
            } else if (player && (*roles[1]).is_evil() && !(*roles[1]).is_alive()) {
                std::cout << "Family will not remember your contribution to the victory of our family... Never!" << std::endl;
            } else if (player && !(*roles[1]).is_evil()) {
                std::cout << "You've tried to stop Mafia, but failed... Now this city belongs to them, but you did everything you could..." << std::endl;
            } else {
                std::cout << "You watched the fall of this city... Fall into the hands of mafia!" << std::endl;
            }
            break;
        case 'P':
            if (player && (*roles[1]).get_role() == "Maniac") {
                std::cout << "You've done a great job saving this city from mafia... and from its citizens. Now it's your city and no one will stop you!" << std::endl; 
            } else if (player) {
                std::cout << "You were trying to stop mafia family, but forgot about another deal - psycho with a knife. Now this city belongs to him, and no one knows, what will be with it tomorrow..." << std::endl;
            } else {
                std::cout << "You watched the fall of mafia family... the fall of comissioner... villagers... and the rise of MANIAC!" << std::endl;
            }
            break;
        case 'V':
            if (player && (*roles[1]).get_role() == "Sherif" && (*roles[1]).is_alive()) {
                std::cout << "You saved this city from gangs and muderers! Congratulations, boss, now you can safely demand a vacation!" << std::endl;
            } else if (player && (*roles[1]).get_role() == "Sherif" && !(*roles[1]).is_alive()) {
                std::cout << "You died saving this city... But this city still belongs to its citizens! Despite your death, your mission lives on! Today this city showed, that it will withstand and gangs, and murderers." << std::endl;
            } else if (player) {
                std::cout << "You and your neighbours showed villains, whose this city is. Today you put all the villains behind bars and start a new, peaceful life." << std::endl;
            } else {
                std::cout << "You watched rallying city residents against dangers and their victory!" << std::endl;
            }
            break;
        default :
            std::cout << "Smth is wrong..." << std::endl;
    }
}

//--------------------------------MAin-layer--------------------------------//

int
main(int argc, char* argv[]) {
    // preparations
    bool extra = true;
    std::string input_str;
    std::cout << "Do you want to use extra roles for the game? Please type yes or no: " << std::endl;
    std::cin >> input_str;
    while (input_str != "Yes" && input_str != "yes" && input_str != "No" && input_str != "no") {
        std::cout << "Please type yes or no, not a " << input_str << ": " << std::endl;
        std::cin >> input_str;
    }
    if (input_str == "No" || input_str == "no") extra = false;
    int minimum_roles = 4;
    if (extra) minimum_roles = 10;

    std::cout << "Enter the number of players, that is over " << minimum_roles<< ", please: " << std::endl;
    std::cin >> number_players;
    while (number_players <= minimum_roles) {
        std::cout << "Players should be over " << minimum_roles<< ", enter the number of players again, please: " << std::endl;
        std::cin >> number_players;
    }
    bool player = true;
    std::cout << "Do you want to be a player, please type yes or no: " << std::endl;
    std::cin >> input_str;
    while (input_str != "Yes" && input_str != "yes" && input_str != "No" && input_str != "no") {
        std::cout << "Please type yes or no, not a " << input_str << ": " << std::endl;
        std::cin >> input_str;
    }
    if (input_str == "No" || input_str == "no") player = false;

    std::random_device rd;
    std::mt19937 gen(rd());
    generator = gen;
    std::uniform_int_distribution<> distrib(1, number_players - 1);
    distribution = distrib;

    // now we are ready to start our game

    // Roles creating
    std::unordered_map<int, SharedPtr<Role>> roles;
    roles_creating(number_players, extra, roles);
    // now all Roles have their roles

    for (auto i: roles) {
        std::cout << i.first << " " << (*i.second).get_role() << std::endl;
    }

    // start of the game
    if (player) {
        std::cout << "Your role is " << (*roles[1]).get_role() << std::endl;
    }
    night_is_coming(roles, number_players, extra, player);

    // finish
    finishing_and_results(roles, extra, player);

    return 0;
}