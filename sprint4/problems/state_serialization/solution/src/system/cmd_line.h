//
// Created by serg on 01.05.23.
//

#ifndef GAME_SERVER_CMD_LINE_H
#define GAME_SERVER_CMD_LINE_H

#include <iostream>
#include <vector>
#include <optional>
#include <boost/program_options.hpp>
#include <filesystem>


namespace sys_utils {

using namespace std::literals;

struct Args {
    uint64_t tick;
    uint64_t save_period;
    std::filesystem::path config;
    std::string www_root;
    std::filesystem::path state_file;
    bool rand_points;
};

void SetRandomStart(const bool );
void SetTestEnv(const bool);

[[nodiscard]] std::optional<Args> ParseCommandLine(int argc, const char* argv[]);

} // namespace sys_utils

#endif //GAME_SERVER_CMD_LINE_H
