#include "player.cpp"

// constants

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

// functions

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
        } else if (role == "Mafia" || role == "Lier") {
            if (roles[i]->get_target() == -1) continue;
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
            if (drunker != -1 && drunkers_treatment == mafias_target && drunker != drunkers_treatment) {
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
            if (drunker != -1 && mafias_target == drunkers_treatment && drunker != drunkers_treatment) {
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
            if (drunker != -1 && drunkers_treatment == maniacs_target && drunker != drunkers_treatment) {
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
            if (drunker != -1 && drunkers_treatment == maniacs_target && drunker != drunkers_treatment) {
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
            if (drunker != -1 && sherifs_target == drunkers_treatment && drunker != drunkers_treatment) {
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
            if (drunker != -1 && drunkers_treatment == sherifs_target && drunker != drunkers_treatment) {
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
            if (drunker != -1 && drunkers_treatment == doctors_target && drunker != drunkers_treatment) {
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
        if (i == bullies_treatment && i != drunker) {   // everybody except drunker
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
        if (i == drunker && !drunkers_bullied && (drunkers_treatment == bullies_treatment || (drunkers_treatment == -1 && drunker == bullies_treatment))) {
            // if drunker was at bullies treatment house or he was a target and stayed at home
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
    if (max_index == bullies_treatment) {
        std::cout << "Player " << max_index << " can't be sent to the jail. He isn't alright now, so we'll deal with him later." << std::endl;
        logger.day_logging("Player " + std::to_string(max_index) + " is Bully's target, so he can't be sent to the jail.");
    } else {
        std::cout << "To the jail player " << max_index << " was sent. He was " << roles[max_index]->get_role() << " by the way." << std::endl;
        logger.day_logging("To the jail player " + std::to_string(max_index) + " was sent. He was a " + roles[max_index]->get_role() + " by the way.");
        roles[max_index]->dead();
    }
    return;
}