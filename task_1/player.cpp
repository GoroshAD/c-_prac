#include "roles.cpp"

struct Player {   // players number is always 1
    bool exists = true;   // if player exists

    int player_doctors_prev = -1;

    int drunkers_strike = 0;

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
};
