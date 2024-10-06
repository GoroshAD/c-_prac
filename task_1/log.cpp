#include <filesystem>
#include <fstream>
#include <string>

class
Logger {
private:
    std::filesystem::path directory;

public:
    int round_counter = 1;

    Logger () {
        directory = std::filesystem::current_path() / "logs";
        std::filesystem::create_directory(directory);
    }

    void
    start_logging(const std::string &log) {
        std::ofstream log_file(directory / "starting_day.log", std::ios::app);
        log_file << log << std::endl;
        log_file.close();
    }

    void
    night_logging(const std::string &log) {
        std::ofstream log_file(directory / (std::to_string(round_counter) + "_night_phase.log"), std::ios::app);
        log_file << log << std::endl;
        log_file.close();
    }

    void
    day_logging(const std::string &log) {
        std::ofstream log_file(directory / (std::to_string(round_counter) + "_day_phase.log"), std::ios::app);
        log_file << log << std::endl;
        log_file.close();
    }

    void
    rise_counter() {
        round_counter++;
    }

    void
    finish_logging(const std::string &log) {
        std::ofstream log_file(directory / "final.log", std::ios::app);
        log_file << log << std::endl;
        log_file.close();
    }
};