#include <stdio.h>
#include <stdlib.h>
#include <coroutine>
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <thread>
#include <sstream>
#include <type_traits>

#include "roles.cpp"

int counter_players = 0;   // global players counter
int number_players = 0;   // global players number
std::vector<std::pair<std::string, bool>> main_roles = {{"Mafia", false}, \
                                                   {"Sherif", false}, \
                                                   {"Doctor", false}, \
                                                   {"Maniac", false}, \
                                                   {"Villager", false}};
std::vector<std::pair<std::string, bool>> extra_roles = {{"Lier", false}, \
                                                         {"Bully", false}, \
                                                         {"Drunker", false}};

template <typename T>
concept good_or_evil = requires(T t) {
    { t.evil } -> std::convertible_to<bool>;
};

void
players_creating(std::vector<std::string>& roles, int num, bool extra, std::vector<std::thread>& players)
{
    roles[0] = "Holder";
    /*for (int i = 1; i < num; ++i) {
        players[i] = std::thread(playing);
    }*/
    int evil_quan = num / 3;
    int roles_quan = 5;
    int good_quan = num - 3 - evil_quan;
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
                roles[i] = extra_roles[role - 5].first;
                extra_roles[role - 5].second = true;
                break;
            } else if (role < 5 && !main_roles[role].second) {
                if (main_roles[role].first == "Mafia" && evil_quan == 0) continue;
                if (main_roles[role].first == "Villager" && good_quan == 0) continue;
                roles[i] = main_roles[role].first;
                if (roles[i] != "Mafia" && roles[i] != "Villager") {
                    main_roles[role].second = true;
                }
                if (roles[i] == "Mafia") --evil_quan;
                if (roles[i] == "Villager") --good_quan;
                break;
            }
        }
    }
    /*for (int i = 1; i < number_players; ++i) {
        // send to threads their roles;
    }*/
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
    int minimum_players = 4;
    if (extra) minimum_players = 10;

    std::cout << "Enter the number of players, that is over " << minimum_players<< ", please: " << std::endl;
    std::cin >> number_players;
    while (number_players <= minimum_players) {
        std::cout << "Players should be over " << minimum_players<< ", enter the number of players again, please: " << std::endl;
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

    // players creating
    std::vector<std::string> roles(number_players);
    std::vector<std::thread> players(number_players);
    players_creating(roles, number_players, extra, players);
    // now all players have their roles

    for (auto i: roles) {
        std::cout << i << std::endl;
    }

    //test shared_ptr
    SharedPtr<std::vector<int>> p1(new std::vector<int>(10));
    SharedPtr<std::vector<int>> p2(p1);
    (*p1)[0] = 1;

    std::cout << *(*p1).begin() << std::endl;
    std::cout << *(*p2).begin() << std::endl;

    p1.reset(new std::vector<int>(20));
    (*p1)[0] = 3;
    std::cout << *(*p1).begin() << std::endl;
    std::cout << *(*p2).begin() << std::endl;

    SharedPtr<std::vector<int>> p3(std::move(p1));
    std::cout << *(*p3).begin() << std::endl;
    auto tmp1 = SharedPtr<std::vector<int>>(new std::vector<int>(10));
    auto tmp2 = SharedPtr<std::vector<int>>(tmp1);
    std::vector<SharedPtr<std::vector<int>>> arr(2);
    arr.push_back(tmp1);

    std::cout << ":heart:" << std::endl;

    // start of the game

    // finish

    return 0;
}