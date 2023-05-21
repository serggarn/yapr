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
    int tick;
    std::filesystem::path config;
    std::string www_root;
    bool rand_points;
};

void SetRandomStart(const bool );
void SetTestEnv(const bool);

[[nodiscard]] std::optional<Args> ParseCommandLine(int argc, const char* argv[]);
//{
//    namespace po = boost::program_options;
//
//    po::options_description desc{"All options"s};
//    Args args;
//    desc.add_options()           //
//            ("help,h", "produce help message")  //
//            ("tick-period,t", po::value(&args.tick)->value_name("milliseconds"s), "set tick period") //
//            ("config-file,c", po::value(&args.config)->value_name("file"s), "set config file path")  //
//            ("www-root,w", po::value(&args.www_root)->value_name("dir"s), "set static files root") //
//            ("randomize-spawn-points", "spawn dogs at random positions");
//
//    po::variables_map vm;
//    po::store(po::parse_command_line(argc, argv, desc), vm);
//    po::notify(vm);
//
//    if (vm.contains("help"s)) {
//        std::cout << desc;
//        return std::nullopt;
//    }
//
//    if (!vm.contains("tick-period"s)) {
//        std::cout << "Work in test environment" <<std::endl;
//    }
////    SetTestEnv(! vm.contains("tick-period"s));
//
//    if (!vm.contains("randomize-spawn-points"s)) {
//        std::cout << "Work without randomize start" <<std::endl;
//    }
////    SetRandomStart(vm.contains("randomize-spawn-points"s));
////    std::cout << " init rand: " << sett->IsRandomStart() << "; " << vm.contains("randomize-spawn-points"s) <<std::endl;
//    if (!vm.contains("config-file"s)) {
//        throw std::runtime_error("No config-file arg"s);
//    }
//
//    if (!vm.contains("www-root"s)) {
//        throw std::runtime_error("No www-root arg"s);
//    }
//
//    return args;
//}

} // namespace sys_utils

#endif //GAME_SERVER_CMD_LINE_H
