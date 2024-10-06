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

#include "roles.cpp"


void player_night_activity(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra);
void player_day_voting(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra);
void holders_night_checker(std::unordered_map<int, SharedPtr<Role>>& roles, int num);
void holders_day_checker(std::unordered_map<int, SharedPtr<Role>>& roles, int num);
void roles_creating(int num, bool extra, std::unordered_map<int, SharedPtr<Role>>& roles);
void night_is_coming(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, bool player);
void day_is_coming(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, bool player);
std::string are_we_finished(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, bool player);
void finishing_and_results(std::unordered_map<int, SharedPtr<Role>>& roles, bool extra, bool player, int num);

int number_players = 0;   // global players number
std::string result = "no";   // global results
std::vector<std::pair<std::string, bool>> main_roles = {{"Psycho", false},
                                                        {"Mafia", false},
                                                        {"Sherif", false},
                                                        {"Doctor", false},
                                                        {"Villager", false}};
std::vector<std::pair<std::string, bool>> extra_roles = {{"Lier", false},
                                                         {"Drunker", false},
                                                         {"Bully", false}};
int player_doctors_prev = -1;
std::unordered_map<int, std::string> phrases = {
                                            {0, "Hmm... I really think it is "},
                                            {1, "Well well well... "},
                                            {2, "DEFENETLY "},
                                            {3, "You know... I think i've heard something tonight from "},
                                            {4, "I know, that this is "},
                                            {5, "Maaaaaybe "},
                                            {6, "I don't want to vote... But okay, "},
                                            {7, "It. Was. "},
                                            {8, "Yeah, yeah, "},
                                            {9, "We all know, that this is "}
};
int liers_treatment = -1;
int bullies_treatment = -1;
int drunkers_treatment = -1;
int drunkers_strike = 0;
int drunker = -1;
bool drunkers_bullied = false;   // stupid constant
std::unordered_map<int, std::string> stickers = {
                                            {0, "ᕕ(╭ರ╭ ͟ʖ╮•́)⊃¤=(————-"},
                                            {1, "(-(-_-(-_(-_(-_-)_-)-_-)_-)_-)-)"},
                                            {2, "(ﾉ◕ヮ◕)ﾉ*:・ﾟ✧"},
                                            {3, "┻━┻ ︵ヽ(`Д´)ﾉ︵ ┻━┻"},
                                            {4, "⊂(◉‿◉)つ"},
                                            {5, "☜(⌒▽⌒)☞"},
                                            {6, "⤜(ⱺ ʖ̯ⱺ)⤏"},
                                            {7, "ʕノ•ᴥ•ʔノ ︵ ┻━┻"},
                                            {8, "┌( ͝° ͜ʖ͡°)=ε/̵͇̿̿/’̿’̿ ̿"},
                                            {9, "ヽ༼ ຈل͜ຈ༼ ▀̿̿Ĺ̯̿̿▀̿ ̿༽Ɵ͆ل͜Ɵ͆ ༽ﾉ"}
};

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
player_night_activity(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra) {
    int tmp_target = -1;
    if (!(*roles[1]).is_alive()) {
        std::cout << "You're died, so you can't do anything this night." << std::endl;
        logger.night_logging("You're died.");
        return;
    }
    if (roles[1]->get_role() == "Drunker") {
        std::cout << "You woke up this night..." << std::endl;
        logger.night_logging("You woke up this night...");
        drunkers_treatment = -1;
        if (drunkers_strike == 2) {
            std::cout << "And went to bed, because it's the third night of drunkenness and it's not allowed." << std::endl;
            logger.night_logging("And went to bed, because it's the third night of drunkenness and it's not allowed.");
            drunkers_strike = 0;
            return;
        }
        std::string input = "";
        std::cout << "Please select your next move, go to bed tonight (type sleep) or go to your neighbour (type drink): ";
        std::cin >> input;
        while (input != "sleep" && input != "drink") {
            std::cout << "Please, type sleep or drink, not a " << input << ": ";
            std::cin >> input;
        }
        if (input == "sleep") {
            std::cout << "You went to bed and slept all night..." << std::endl;
            logger.night_logging("You went to bed and slept all night...");
            drunkers_strike = 0;
            return;
        }
        std::cout << "Please choose a drinking buddy for the night (type number): ";
        std::cin >> tmp_target;
        while (tmp_target <= 0 || tmp_target >= num || !roles[tmp_target]->is_alive()) {
            std::cout << "This player isn't alive, please try again: ";
            std::cin >> tmp_target;
        }
        drunkers_treatment = tmp_target;
        drunkers_strike++;
        std::cout << "You drank all night with a buddy and went to bed in the morning." << std::endl;
        logger.night_logging("You drank all night with a buddy (player " + std::to_string(tmp_target) + ") and went to bed in the morning.");
    } else if (roles[1]->get_role() == "Bully") {
        std::cout << "You woke up this night..." << std::endl;
        int still_alive_not_bullied = false;
        for (int i = 1; i < num; ++i) {
            if (roles[i]->is_alive() && !roles[i]->was_bully()) {
                still_alive_not_bullied = true;
                break;
            }
        }
        if (!still_alive_not_bullied) {
            std::cout << "You've already bullied everyone in this city, great job!" << std::endl;
            std::cout << "You remembered some of your jokes, laughed a little and went to bed again." << std::endl;
            logger.night_logging("You've already bullied everyone in this city, great job!");
            roles[1]->set_target(-1);
            return;
        }

        std::cout << "Please choose who laugh at this night: ";
        std::cin >> tmp_target;
        while (tmp_target <= 0 || tmp_target >= num || !roles[tmp_target]->is_alive() || roles[tmp_target]->was_bully()) {
            std::cout << "This player isn't alive or was bullied already, please try again: ";
            std::cin >> tmp_target;
        }
        roles[1]->set_target(tmp_target);
        roles[tmp_target]->bullys_treat();
        bullies_treatment = tmp_target;
        std::cout << "Now it's time to have a sleep." << std::endl;
        logger.night_logging("You laughed at player " + std::to_string(tmp_target) + ".");
    } else if (roles[1]->get_role() == "Lier") {
        std::cout << "You woke up this night..." << std::endl;
        logger.night_logging("You woke up this night...");
        std::cout << "Please choose who to slander this night: ";
        std::cin >> tmp_target;
        while (tmp_target <= 0 || tmp_target >= num || !roles[tmp_target]->is_alive()) {
            std::cout << "This player isn't alive, please try again: ";
            std::cin >> tmp_target;
        }
        roles[1]->set_target(tmp_target);
        liers_treatment = tmp_target;
        std::cout << "Now it's time to have a sleep." << std::endl;
        logger.night_logging("You slandered player " + std::to_string(tmp_target) + ".");
    } else if ((*roles[1]).get_role() == "Sherif") {
        std::cout << "You woke up this night..." << std::endl;
        logger.night_logging("You woke up this night...");
        std::string action;
        std::cout << "Please choose your action, type assault or check: ";
        std::cin >> action;
        while (action != "assault" && action != "check") {
            std::cout << "Please type assault or check: ";
            std::cin >> action;
        }
        logger.night_logging("You decided to " + action + ".");
        if (action == "check") {
            std::cout << "Please type a number of a player, which you want to check: ";
            std::cin >> tmp_target;
            while (tmp_target <= 0 || tmp_target >= num || !(*roles[tmp_target]).is_alive()) {
                std::cout << "This player isn't alive, please try again: ";
                std::cin >> tmp_target;
            }
            if (tmp_target == drunker && drunkers_treatment != -1 && drunkers_treatment != drunker) {
                std::cout << "This player isn't tonight at home." << std::endl;
                logger.night_logging("Player " + std::to_string(tmp_target) + "isn't tonight at home.");
            } else {
                if ((*roles[tmp_target]).is_evil() || tmp_target == liers_treatment) {
                    std::cout << "This player is evil." << std::endl;
                    logger.night_logging("Player " + std::to_string(tmp_target) + " is from mafia family");
                } else {
                    std::cout << "This player is good." << std::endl;
                    logger.night_logging("Player " + std::to_string(tmp_target) + " isn't from mafia family");
                }
                if (drunkers_treatment == tmp_target && tmp_target == liers_treatment) {
                    std::cout << "Also player " << drunker << " was here tonight. He is evil." << std::endl;
                    logger.night_logging("Also player " + std::to_string(drunker) + " is from mafia family");
                } else if (drunkers_treatment == tmp_target) {
                    std::cout << "Also player " << drunker << " was here tonight. He is good." << std::endl;
                    logger.night_logging("Also player " + std::to_string(drunker) + " isn't from mafia family");
                }
            }
        } else {
            std::cout << "Please type a number of a player, which you want to kill: ";
            std::cin >> tmp_target;
            while (tmp_target <= 0 || tmp_target >= num || !(*roles[tmp_target]).is_alive()) {
                std::cout << "This player isn't alive, please try again: ";
                std::cin >> tmp_target;
            }
            (*roles[1]).set_target(tmp_target);
            logger.night_logging("You selected player " + std::to_string(tmp_target));
        }
        std::cout << "Now it's time to have a sleep." << std::endl;
    } else if ((*roles[1]).get_role() == "Doctor") {
        std::cout << "You woke up this night..." << std::endl;
        logger.night_logging("You woke up this night...");
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
        roles[1]->set_target(tmp_target);
        player_doctors_prev = tmp_target;
        std::cout << "Now it's time to have a sleep." << std::endl;
        logger.night_logging("You selected player " + std::to_string(tmp_target));
    } else if ((*roles[1]).get_role() != "Villager") {
        std::cout << "You woke up this night..." << std::endl;
        logger.night_logging("You woke up this night...");
        std::cout << "Choose a target, please: ";
        std::cin >> tmp_target;
        while (tmp_target >= num || tmp_target < 1 || !(*roles[tmp_target]).is_alive()) {
            std::cout << "This player isn't alive, please try again: ";
            std::cin >> tmp_target;
        }
        (*roles[1]).set_target(tmp_target);
        std::cout << "Now it's time to have a sleep." << std::endl;
        logger.night_logging("You selected player " + std::to_string(tmp_target));
    } else {
        std::cout <<  "You slept all night." << std::endl;
        logger.night_logging("You slept all night.");
    }
    return;
}
void
player_day_voting(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra) {
    if (!roles[1]->is_alive()) {
        std::cout << "Well, you're dead now, just watch judge's decision..." << std::endl;
        logger.day_logging("You're dead.");
        return;
    }
    int tmp_vote = -1;
    std::cout << "Please, choose a player, which you suspect: ";
    std::cin >> tmp_vote;
    while (tmp_vote <= 0 || tmp_vote >= num || !roles[tmp_vote]->is_alive()) {
        std::cout << "This player isn't alive, please select another one: ";
        std::cin >> tmp_vote;
    }
    roles[1]->set_vote_target(tmp_vote);
    logger.day_logging("You selected player " + std::to_string(tmp_vote));
    return;
}

void
holders_night_checker(std::unordered_map<int, SharedPtr<Role>>& roles, int num)
{
    logger.night_logging("The Holder made final results:");
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
        if (mafias_target == drunker && drunkers_treatment != -1 && drunkers_treatment != drunker) {
            std::cout << "Mafia came to drunker's place tonight, but he wasn't home." << std::endl;
            logger.night_logging("Mafia came to drunker's place tonight (player " + std::to_string(drunker) + "), but he wasn't home.");
        } else {
            std::cout << "Mafia killed player number " << mafias_target << " tonight. He was a " << (*roles[mafias_target]).get_role() << "." << std::endl;
            logger.night_logging("Mafia killed player number " + std::to_string(mafias_target) + " tonight. He was a " + (*roles[mafias_target]).get_role() + ".");
            (*roles[mafias_target]).dead();
            if (drunkers_treatment == mafias_target && drunker != drunkers_treatment) {
                std::cout << "Mafia also killed player number " << drunker << " tonight. He was a " << (*roles[drunker]).get_role() << "." << std::endl;
                logger.night_logging("Mafia also killed player number " + std::to_string(mafias_target) + " tonight. He was a " + (*roles[mafias_target]).get_role() + ".");
                (*roles[drunker]).dead();
            }
        }
    } else if (mafias_target != -1) {
        if (mafias_target == drunker && drunkers_treatment != -1 && drunkers_treatment != drunker) {
            std::cout << "Mafia came to drunker's place tonight, but he wasn't home." << std::endl;
            logger.night_logging("Mafia came to drunker's place tonight (player " + std::to_string(drunker) + "), but he wasn't home.");
        } else {
            std::cout << "Someone tried to kill player " << mafias_target << " tonight. Luckily, Doctor saved him from his death." << std::endl;
            logger.night_logging("Mafia tried to kill player number " + std::to_string(mafias_target) + " tonight, but Doctor saved him.");
            if (mafias_target == drunkers_treatment && drunker != drunkers_treatment) {
                std::cout << "Someone also tried to kill player " << drunker << " tonight. Luckily, Doctor saved him from his death." << std::endl;
                logger.night_logging("Mafia also tried to kill player number " + std::to_string(mafias_target) + " tonight, but Doctor saved him.");
            }
        }
    }

    if (maniacs_target != -1 && maniacs_target != doctors_target && roles[maniacs_target]->is_alive()) {
        if (maniacs_target == drunker && drunkers_treatment != -1 && drunkers_treatment != drunker) {
            std::cout << "Maniac came to drunker's place tonight, but he wasn't home." << std::endl;
            logger.night_logging("Maniac came to drunker's place tonight (player " + std::to_string(drunker) + "), but he wasn't home.");
        } else {
            std::cout << "Maniac killed player number " << maniacs_target << " tonight. He was a " << (*roles[maniacs_target]).get_role() << "." << std::endl;
            logger.night_logging("Maniac killed player number " + std::to_string(maniacs_target) + " tonight. He was a " + (*roles[maniacs_target]).get_role() + ".");
            (*roles[maniacs_target]).dead();
            if (drunkers_treatment == maniacs_target && drunker != drunkers_treatment) {
                std::cout << "Maniac also killed player number " << drunker << " tonight. He was a " << (*roles[drunker]).get_role() << "." << std::endl;
                logger.night_logging("Maniac also killed player number " + std::to_string(drunker) + " tonight. He was a " + (*roles[drunker]).get_role() + ".");
                (*roles[drunker]).dead();
            }
        }
    } else if (maniacs_target != -1 && roles[maniacs_target]->is_alive() && maniacs_target == doctors_target) {
        if (maniacs_target == drunker && drunkers_treatment != -1 && drunkers_treatment != drunker) {
            std::cout << "Maniac came to drunker's place tonight, but he wasn't home." << std::endl;
            logger.night_logging("Maniac came to drunker's place tonight (player " + std::to_string(drunker) + "), but he wasn't home.");
        } else {
            std::cout << "Someone tried to kill player " << maniacs_target << " tonight. Luckily, Doctor saved him from his death." << std::endl;
            logger.night_logging("Maniac tried to kill player number " + std::to_string(mafias_target) + " tonight, but Doctor saved him.");
            if (drunkers_treatment == maniacs_target && drunker != drunkers_treatment) {
                std::cout << "Someone also tried to kill player " << drunker << " tonight. Luckily, Doctor saved him from his death." << std::endl;
                logger.night_logging("Maniac also tried to kill player number " + std::to_string(mafias_target) + " tonight, but Doctor saved him.");
            }
        }
    }
    
    if (sherifs_target != -1 && sherifs_target != doctors_target && roles[sherifs_target]->is_alive()) {
        if (sherifs_target == drunker && drunkers_treatment != -1 && drunkers_treatment != drunker) {
            std::cout << "Sherif came to drunker's home tonight, but nobody was at home." << std::endl;
            logger.night_logging("Sherif came to drunker's place tonight (player " + std::to_string(drunker) + "), but he wasn't home.");
        } else {
            std::cout << "Sherif killed player number " << sherifs_target << " tonight. He was a " << (*roles[sherifs_target]).get_role() << "." << std::endl;
            logger.night_logging("Sherif killed player number " + std::to_string(sherifs_target) + " tonight. He was a " + (*roles[sherifs_target]).get_role() + ".");
            (*roles[sherifs_target]).dead();
            if (sherifs_target == drunkers_treatment && drunker != drunkers_treatment) {
                std::cout << "Sherif also killed player number " << drunker << " tonight. He was a " << (*roles[drunker]).get_role() << "." << std::endl;
                logger.night_logging("Sherif also killed player number " + std::to_string(drunker) + " tonight. He was a " + (*roles[drunker]).get_role() + ".");
                (*roles[drunker]).dead();
            }
        }
    } else if (sherifs_target != -1 && roles[sherifs_target]->is_alive() && doctors_target == sherifs_target) {
        if (sherifs_target == drunker && drunkers_treatment != -1 && drunkers_treatment != drunker) {
            std::cout << "Sherif came to drunker's home tonight, but nobody was at home." << std::endl;
            logger.night_logging("Sherif came to drunker's place tonight (player " + std::to_string(drunker) + "), but he wasn't home.");
        } else {
            std::cout << "Someone tried to kill player " << sherifs_target << " tonight. Luckily, Doctor saved him from his death." << std::endl;
            logger.night_logging("Sherif tried to kill player number " + std::to_string(sherifs_target) + " tonight, but Doctor saved him.");
            if (drunkers_treatment == sherifs_target && drunker != drunkers_treatment) {
                std::cout << "Someone also tried to kill player " << drunker << " tonight. Luckily, Doctor saved him from his death." << std::endl;
                logger.night_logging("Sherif also tried to kill player number " + std::to_string(drunker) + " tonight, but Doctor saved him.");
            }
        }
    }

    if (mafias_target != doctors_target && maniacs_target != doctors_target && sherifs_target != doctors_target && doctors_target != -1) {
        if (doctors_target == drunker && drunker != drunkers_treatment && drunkers_treatment != -1) {
            std::cout << "Doctor went to drunker's house this night, but he wasn't at home this night." << std::endl;
            logger.night_logging("Doctor came to drunker's place tonight (player " + std::to_string(drunker) + "), but he wasn't home.");
        } else {
            std::cout << "Doctor went to player " << doctors_target << " tonight." << std::endl;
            logger.night_logging("Doctor went to player " + std::to_string(doctors_target) + " tonight.");
            if (drunkers_treatment == doctors_target && drunker != drunkers_treatment) {
                std::cout << "Doctor also saw there player " << drunker << "." << std::endl;
                logger.night_logging("Doctor also saw player " + std::to_string(drunker) + " tonight.");
            }
        }
    }
    return;
}

void
holders_day_checker(std::unordered_map<int, SharedPtr<Role>>& roles, int num)
{
    std::unordered_map<int, int> votes;
    std::uniform_int_distribution<> distrib(0, 9);
    for (int i = 1; i < num; ++i) {
        if (!roles[i]->is_alive()) continue;
        if (i == bullies_treatment && (i != drunker || (i == drunker && (drunkers_treatment == drunker || drunkers_treatment == -1) && !drunkers_bullied))) {
            if ((i == drunker && (drunkers_treatment == drunker || drunkers_treatment == -1) && !drunkers_bullied)) {
                roles[drunker]->bullys_treat();
                drunkers_bullied = true;
            }
            logger.day_logging("Player " + std::to_string(i) + " is under bully's joke, so: ");
            int sticker_num = distrib(generator);
            std::cout << "Player " << i << " says: " << stickers[sticker_num] << std::endl;
            logger.day_logging("Player " + std::to_string(i) + " says: " + stickers[sticker_num]);
            std::cout << "The judge says: what?.." << std::endl;
            sticker_num = distrib(generator);
            std::cout << "Player " << i << " says: " << stickers[sticker_num] << std::endl;
            logger.day_logging("Player " + std::to_string(i) + " says: " + stickers[sticker_num]);
            std::cout << "The judge says: please, be serious..." << std::endl;
            sticker_num = distrib(generator);
            std::cout << "Player " << i << " says: " << stickers[sticker_num] << std::endl;
            logger.day_logging("Player " + std::to_string(i) + " says: " + stickers[sticker_num]);
            std::cout << "The judge says: nevermind." << std::endl;
            continue;
        }
        if (!drunkers_bullied && i == drunker && (drunkers_treatment == bullies_treatment || drunkers_treatment == -1)) {
            int sticker_num = distrib(generator);
            logger.day_logging("Player " + std::to_string(i) + " is under bully's joke, so: ");
            std::cout << "Player " << i << " says: " << stickers[sticker_num] << std::endl;
            logger.day_logging("Player " + std::to_string(i) + " says: " + stickers[sticker_num]);
            std::cout << "The judge says: what?.." << std::endl;
            sticker_num = distrib(generator);
            std::cout << "Player " << i << " says: " << stickers[sticker_num] << std::endl;
            logger.day_logging("Player " + std::to_string(i) + " says: " + stickers[sticker_num]);
            std::cout << "The judge says: please, be serious..." << std::endl;
            sticker_num = distrib(generator);
            std::cout << "Player " << i << " says: " << stickers[sticker_num] << std::endl;
            logger.day_logging("Player " + std::to_string(i) + " says: " + stickers[sticker_num]);
            std::cout << "The judge says: nevermind." << std::endl;
            roles[drunker]->bullys_treat();
            drunkers_bullied = true;
            continue;
        }

        int phrase_num = distrib(generator);
        std::cout << "Player " << i << " says: " << phrases[phrase_num] << roles[i]->get_vote_target() << std::endl;
        logger.day_logging("Player " + std::to_string(i) + " says: " + phrases[phrase_num] + std::to_string(roles[i]->get_vote_target()));
        if (votes[roles[i]->get_vote_target()] && votes[roles[i]->get_vote_target()] >= 1) {
            votes[roles[i]->get_vote_target()] += 1;
        } else {
            votes[roles[i]->get_vote_target()] = 1;
        }
    }
    std::cout << "The judge stood up and announced the verdict..." << std::endl;

    int maxx = -1;
    int max_index = -1;
    for (auto i : votes) {
        if (i.second > maxx) {
            maxx = i.second;
            max_index = i.first;
        }
    }

    std::cout << "To the jail player " << max_index << " was sent. He was " << roles[max_index]->get_role() << " by the way." << std::endl;
    logger.day_logging("To the jail player " + std::to_string(max_index) + " was sent. He was " + roles[max_index]->get_role() + " by the way.");
    roles[max_index]->dead();
    return;
}

void
night_is_coming(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, bool player) 
{
    std::cout << "The night is coming... The city goes to sleep..." << std::endl;
    logger.night_logging("The " + std::to_string(logger.round_counter) + " night is coming... The city goes to sleep...");
    int lier = -1;
    int bully = -1;
    drunker = -1;
    std::vector<std::future<void>> futures;
    for (int i = 1; i < num; ++i) {
        std::string alive = "not alive";
        if ((*roles[i]).is_alive()) alive = "alive";
        std::cout << i << " " << alive << std::endl;
        logger.night_logging("Player " + std::to_string(i) + " is " + alive);
        if (roles[i]->get_role() == "Lier" && roles[i]->is_alive() && ((i != 1 && player) || !player)) {
            lier = i;
            futures.push_back(std::async(std::launch::async, [&]() {
                Action action = roles[lier]->act(roles, num, lier, liers_treatment);
                action.handle.resume();
            }));
        }
        if (roles[i]->get_role() == "Bully" && roles[i]->is_alive() && ((i != 1 && player) || !player)) {
            bully = i;
            futures.push_back(std::async(std::launch::async, [&]() {
                Action action = roles[bully]->act(roles, num, bully, liers_treatment);
                action.handle.resume();
            }));
        }
        if (roles[i]->get_role() == "Drunker" && roles[i]->is_alive() && ((i != 1 && player) || !player)) {
            drunker = i;
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
        if (player && i == 1) {
            player_night_activity(roles, num, extra);
        } else if (i != lier && i != bully && i != drunker) {
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
day_is_coming(std::unordered_map<int, SharedPtr<Role>>& roles, int num, bool extra, bool player)
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
        if (player && i == 1) {
            player_day_voting(roles, num, extra); 
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
        } else if (roles[i]->is_alive()) {
            ++good_alive;
        }
    }
    if (good_alive <= maniac_alive && mafias_alive == 0) return "Psycho";
    if (mafias_alive == 0 && maniac_alive == 0) return "Villager";
    return "no";
}

void
finishing_and_results(std::unordered_map<int, SharedPtr<Role>>& roles, bool extra, bool player, int num)
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
            if (player && (*roles[1]).is_evil() && (*roles[1]).is_alive()) {
                std::cout << "Congratulations, our new family member, you did a great job for the family." << std::endl;
            } else if (player && (*roles[1]).is_evil() && !(*roles[1]).is_alive()) {
                std::cout << "Family will not forget your contribution to the victory of our family... Never!" << std::endl;
            } else if (player && !(*roles[1]).is_evil()) {
                std::cout << "You've tried to stop Mafia, but failed... Now this city belongs to them, but you did everything you could..." << std::endl;
            } else {
                std::cout << "You watched the fall of this city... Fall into the hands of mafia!" << std::endl;
            }
            break;
        case 'P':
            logger.finish_logging("Maniac wins.");
            if (player && (*roles[1]).get_role() == "Maniac") {
                std::cout << "You've done a great job saving this city from mafia... and from its citizens. Now it's your city and no one will stop you!" << std::endl; 
            } else if (player) {
                std::cout << "You were trying to stop mafia family, but forgot about another deal - psycho with a knife. Now this city belongs to him, and no one knows, what will be with it tomorrow..." << std::endl;
            } else {
                std::cout << "You watched the fall of mafia family... the fall of comissioner... villagers... and the rise of MANIAC!" << std::endl;
            }
            break;
        case 'V':
            logger.finish_logging("Villagers win.");
            if (player && (*roles[1]).get_role() == "Sherif" && (*roles[1]).is_alive()) {
                std::cout << "You saved this city from gangs and muderers! Congratulations, boss, now you can safely demand a vacation!" << std::endl;
            } else if (player && (*roles[1]).get_role() == "Sherif" && !(*roles[1]).is_alive()) {
                std::cout << "You died saving this city... But this city still belongs to its citizens! Despite your death, your mission lives on! Today this city showed, that it will withstand and gangs, and murderers." << std::endl;
            } else if (player && roles[1]->is_evil()) {
                std::cout << "Your family fell this time... This city showed you, whos it is... this time..." << std::endl;
            } else if (player) {
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
    if (player) {
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
    } else {
        logger.start_logging("You're not at the game right now, so just watch it.");
    }
    night_is_coming(roles, number_players, extra, player);

    // finish
    finishing_and_results(roles, extra, player, number_players);

    return 0;
}