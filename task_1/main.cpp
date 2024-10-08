#include <stdio.h>
#include <stdlib.h>
#include <coroutine>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <sstream>
#include <type_traits>
#include <future>

#include "holder.cpp"

void holders_night_checker(std::unordered_map<int, SharedPtr<Role>>& roles, int num);
void holders_day_checker(std::unordered_map<int, SharedPtr<Role>>& roles, int num);
void roles_creating(int num, bool extra, std::unordered_map<int, SharedPtr<Role>>& roles);
void night_is_coming(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, Player player);
void day_is_coming(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, Player player);
std::string are_we_finished(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, Player player);
void finishing_and_results(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, Player player);

int number_players = 0;   // global players number
std::string result = "no";   // global results
std::vector<std::pair<std::string, bool>> main_roles = {{"Psycho", false},
                                                        {"Mafia", false},
                                                        {"Sherif", false},
                                                        {"Doctor", false},
                                                        {"Villager", false}};
std::vector<std::pair<std::string, bool>> extra_roles = {{"Lier", false},
                                                         {"Bully", false},
                                                         {"Drunker", false}};

//---functions---
void
roles_creating(int num, bool extra, std::unordered_map<int, SharedPtr<Role>>& roles)
{
    roles[0] = SharedPtr<Role>(new Holder());
    roles[0]->set_role(0);
    int evil_quan = num / 4;
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
night_is_coming(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, Player player) 
{
    std::cout << "The night is coming... The city goes to sleep..." << std::endl;
    logger.night_logging("The " + std::to_string(logger.round_counter) + " night is coming... The city goes to sleep...");
    std::vector<std::future<void>> futures;
    for (int i = 1; i < num; ++i) {
        std::string alive = "not alive";
        if ((*roles[i]).is_alive()) alive = "alive";
        std::cout << i << " " << alive << std::endl;
        logger.night_logging("Player " + std::to_string(i) + " is " + alive);
        if (roles[i]->get_role() == "Lier" && roles[i]->is_alive() && ((i != 1 && player.exists) || !player.exists)) {
            futures.push_back(std::async(std::launch::async, [&]() {
                Action action = roles[lier]->act(roles, num, lier, liers_treatment);
                action.handle.resume();
            }));
        }
        if (roles[i]->get_role() == "Bully" && roles[i]->is_alive() && ((i != 1 && player.exists) || !player.exists)) {
            futures.push_back(std::async(std::launch::async, [&]() {
                Action action = roles[bully]->act(roles, num, bully, liers_treatment);
                action.handle.resume();
            }));
        }
        if (roles[i]->get_role() == "Drunker" && roles[i]->is_alive() && ((i != 1 && player.exists) || !player.exists)) {
            futures.push_back(std::async(std::launch::async, [&]() {
                Action action = roles[drunker]->act(roles, num, drunker, liers_treatment);
                action.handle.resume();
            }));
        }
    }
    for (auto& future : futures) {
        future.get();
    }
    futures.clear();

    //lier
    if (lier != -1) {
        liers_treatment = roles[lier]->get_target();
    } else {
        liers_treatment = -1;
    }

    //bully
    if (bully != -1) {
        bullies_treatment = roles[bully]->get_target();
    } else {
        bullies_treatment = -1;
    }

    //drunker
    if (drunker != -1) {
        drunkers_treatment = roles[drunker]->get_target();
    } else {
        drunkers_treatment = -1;
    }
    
    for (const auto& [i, bot] : roles) {
        if (player.exists && i == 1) {
            if (i == drunkers_treatment) {
                std::cout << "You're a drunker's target tonight, so just relax and have a nice night together!" << std::endl;
                logger.night_logging("You're a drunker's target today, so skipping your night activity...");
                if (roles[1]->get_role() == "Doctor") {
                    player.player_doctors_prev = -1;
                }
                continue;
            }
            player.player_night_activity(roles, num, extra);
        } else if (i != lier && i != bully && i != drunker) {
            if (i == drunkers_treatment) {
                logger.night_logging("Player " + std::to_string(i) + " is drunker's target, so he is skipping his activity.");
                continue;
            }
            futures.push_back(std::async(std::launch::async, [&]() {
                Action action = bot->act(roles, num, i, liers_treatment);
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
day_is_coming(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, Player player)
{
    std::cout << "The day is coming... Good morning, our citizens!" << std::endl;
    logger.day_logging("The " + std::to_string(logger.round_counter) + " day is coming... Good morning, our citizens!");
    for (int i = 1; i < num; ++i) {
        std::string alive = "not alive";
        if ((*roles[i]).is_alive()) alive = "alive";
        std::cout << i << " " << alive << std::endl;
        logger.day_logging("Player " + std::to_string(i) + " is " + alive);
    }
    std::cout << "The voting is starting..." << std::endl;
    std::vector<std::future<void>> futures;
    
    for (const auto& [i, bot] : roles) {
        if (player.exists && i == 1) {
            player.player_day_voting(roles, num, extra); 
        } else {
            futures.push_back(std::async(std::launch::async, [&]() {
                Action action = bot->vote(roles, num, i);
                action.handle.resume();
            }));
        }
    }
    for (auto& future : futures) {
        future.get();
    }
    futures.clear();

    // holders time
    holders_day_checker(roles, num);

    result = are_we_finished(roles, num, extra, player);
    logger.rise_counter();
    if (result == "no") night_is_coming(roles, num, extra, player);
    return;
}

std::string
are_we_finished(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, Player player)
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
        } else if (roles[i]->is_alive()) {
            ++good_alive;
        }
    }
    if (good_alive <= maniac_alive && mafias_alive == 0) return "Psycho";
    if (mafias_alive == 0 && maniac_alive == 0) return "Villager";
    return "no";
}

void
finishing_and_results(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, Player player)
{
    logger.finish_logging("-------Game ends here!-------");
    std::string alive = "alive";
    for (int i = 1; i < num; ++i) {
        alive = roles[i]->is_alive() ? "alive" : "not alive";
        logger.finish_logging("Player " + std::to_string(i) + " (" + roles[i]->get_role() + ") is " + alive);
    }
    switch (result[0]) {
        case 'M':
            logger.finish_logging("Mafia wins.");
            std::cout << "Everybody will remember the day, when Mafia gets its victory and rules the city!" << std::endl;
            if (player.exists && (*roles[1]).is_evil() && (*roles[1]).is_alive()) {
                std::cout << "Congratulations, our new family member, you did a great job for the family." << std::endl;
            } else if (player.exists && (*roles[1]).is_evil() && !(*roles[1]).is_alive()) {
                std::cout << "Family will not forget your contribution to the victory of our family... Never!" << std::endl;
            } else if (player.exists && !(*roles[1]).is_evil()) {
                std::cout << "You've tried to stop Mafia, but failed... Now this city belongs to them, but you did everything you could..." << std::endl;
            } else {
                std::cout << "You watched the fall of this city... Fall into the hands of mafia!" << std::endl;
            }
            break;
        case 'P':
            logger.finish_logging("Maniac wins.");
            if (player.exists && (*roles[1]).get_role() == "Maniac") {
                std::cout << "You've done a great job saving this city from mafia... and from its citizens. Now it's your city and no one will stop you!" << std::endl; 
            } else if (player.exists && !roles[1]->is_evil()) {
                std::cout << "You were trying to stop mafia family, but forgot about another deal - psycho with a knife. Now this city belongs to him, and no one knows, what will be with it tomorrow..." << std::endl;
            } else if (player.exists && roles[1]->is_evil()) {
                std::cout << "Your family did a great try to be a ruler of the city... but Maniac decide to be the only ruler of this city." << std::endl;
            } else {
                std::cout << "You watched the fall of mafia family... the fall of comissioner... villagers... and the rise of MANIAC!" << std::endl;
            }
            break;
        case 'V':
            logger.finish_logging("Villagers win.");
            if (player.exists && (*roles[1]).get_role() == "Sherif" && (*roles[1]).is_alive()) {
                std::cout << "You saved this city from gangs and muderers! Congratulations, boss, now you can safely demand a vacation!" << std::endl;
            } else if (player.exists && (*roles[1]).get_role() == "Sherif" && !(*roles[1]).is_alive()) {
                std::cout << "You died saving this city... But this city still belongs to its citizens! Despite your death, your mission lives on! Today this city showed, that it will withstand and gangs, and murderers." << std::endl;
            } else if (player.exists && roles[1]->is_evil()) {
                std::cout << "Your family fell this time... This city showed you, whose it is... this time..." << std::endl;
            } else if (player.exists) {
                std::cout << "You and your neighbours showed villains, whose this city is. Today you put all the villains behind bars and start a new, peaceful life." << std::endl;
            } else {
                std::cout << "You watched rallying city residents against dangers and their victory!" << std::endl;
            }
            break;
        default :
            logger.finish_logging("Smth is wrong...");
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
    Player player;
    std::cout << "Do you want to be a player, please type yes or no: " << std::endl;
    std::cin >> input_str;
    while (input_str != "Yes" && input_str != "yes" && input_str != "No" && input_str != "no") {
        std::cout << "Please type yes or no, not a " << input_str << ": " << std::endl;
        std::cin >> input_str;
    }
    if (input_str == "No" || input_str == "no") player.exists = false;

    std::random_device rd;
    std::mt19937 gen(rd());
    generator = gen;
    std::uniform_int_distribution<> distrib(1, number_players - 1);
    distribution = distrib;
    logger = Logger();
    logger.start_logging("-------Game starts right now!-------");

    // now we are ready to start our game

    // Roles creating
    std::unordered_map<int, SharedPtr<Role>> roles;
    roles_creating(number_players, extra, roles);
    // now all Roles have their roles

    for (int i = 0; i < number_players; ++i) {
        logger.start_logging("Player " + std::to_string(i) + " is " + roles[i]->get_role());
    }

    // start of the game
    if (player.exists) {
        logger.start_logging("You're also in the game as number 1 and your role is " + roles[1]->get_role());
        std::cout << "Your role is " << (*roles[1]).get_role() << std::endl;
        std::cout << "Your number is 1." << std::endl;
        if (roles[1]->is_evil()) {
            for (int i = 2; i < number_players; ++i) {
                if (roles[i]->is_evil()) {
                    std::cout << "Player " << i << " is " << roles[i]->get_role() << std::endl;
                }
            }
        }
        std::cout << "There are " << number_players /4 << " mafia family members among us!" << std::endl;
    } else {
        logger.start_logging("You're not at the game right now, so just watch it.");
        std::cout << "There are " << number_players /4 << " mafia family members at all." << std::endl;
    }
    logger.start_logging("There are " + std::to_string(number_players / 4) + " mafia family members at all.");
    night_is_coming(roles, number_players, extra, player);

    // finish
    finishing_and_results(roles, number_players, extra, player);

    return 0;
}