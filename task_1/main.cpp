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

#include "roles.cpp"

int counter_roles = 0;   // global Roles counter
int number_roles = 0;   // global Roles number
std::vector<std::pair<std::string, bool>> main_roles = {{"Mafia", false}, \
                                                   {"Sherif", false}, \
                                                   {"Doctor", false}, \
                                                   {"Psycho", false}, \
                                                   {"Villager", false}};
std::vector<std::pair<std::string, bool>> extra_roles = {{"Lier", false}, \
                                                         {"Bully", false}, \
                                                         {"Drunker", false}};


//---concepts---
template <typename T>
concept good_or_evil = requires(T t) {
    { t.evil } -> std::convertible_to<bool>;
};


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
    for (int i = 1; i < number_roles; ++i) {
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
playing() 
{

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

    std::cout << "Enter the number of Roles, that is over " << minimum_roles<< ", please: " << std::endl;
    std::cin >> number_roles;
    while (number_roles <= minimum_roles) {
        std::cout << "Roles should be over " << minimum_roles<< ", enter the number of Roles again, please: " << std::endl;
        std::cin >> number_roles;
    }
    bool role = true;
    std::cout << "Do you want to be a Role, please type yes or no: " << std::endl;
    std::cin >> input_str;
    while (input_str != "Yes" && input_str != "yes" && input_str != "No" && input_str != "no") {
        std::cout << "Please type yes or no, not a " << input_str << ": " << std::endl;
        std::cin >> input_str;
    }
    if (input_str == "No" || input_str == "no") role = false;

    std::random_device rd;
    std::mt19937 gen(rd());
    generator = gen;
    std::uniform_int_distribution<> distrib(1, number_roles - 1);
    distribution = distrib;

    // now we are ready to start our game

    // Roles creating
    std::unordered_map<int, SharedPtr<Role>> roles;
    roles_creating(number_roles, extra, roles);
    // now all Roles have their roles

    for (auto i: roles) {
        std::cout << i.first << " " << (*i.second).get_role() << std::endl;
    }
    

    // start of the game

    // finish

    return 0;
}