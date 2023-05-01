//
// Created by serg on 01.05.23.
//
#include "cmd_line.h"
#include "settings.h"

namespace sys_utils {

    void SetTestEnv(const bool _test) {
        auto sett = settings::Settings::GetInstance();
        sett->SetTestEnv(_test);
    }

    void SetStartRandom(const bool _rand) {
        auto sett = settings::Settings::GetInstance();
        sett->SetRandomStart(_rand);
    }
    std::optional<sys_utils::Args> ParseCommandLine(int argc, const char* argv[]) {
        namespace po = boost::program_options;

        po::options_description desc{"All options"s};
        sys_utils::Args args;
        desc.add_options()           //
                ("help,h", "produce help message")  //
                ("tick-period,t", po::value(&args.tick)->value_name("milliseconds"s), "set tick period") //
                ("config-file,c", po::value(&args.config)->value_name("file"s), "set config file path")  //
                ("www-root,w", po::value(&args.www_root)->value_name("dir"s), "set static files root") //
                ("randomize-spawn-points", "spawn dogs at random positions");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.contains("help"s)) {
            std::cout << desc;
            return std::nullopt;
        }

        auto sett = settings::Settings::GetInstance();

        if (!vm.contains("tick-period"s)) {
            std::cout << "Work in test environment" <<std::endl;
        }
        sett->SetTestEnv(! vm.contains("tick-period"s));

        if (!vm.contains("randomize-spawn-points"s)) {
            std::cout << "Work without randomize start" <<std::endl;
        }
        sett->SetRandomStart(vm.contains("randomize-spawn-points"s));
//    std::cout << " init rand: " << sett->IsRandomStart() << "; " << vm.contains("randomize-spawn-points"s) <<std::endl;
        if (!vm.contains("config-file"s)) {
            throw std::runtime_error("No config-file arg"s);
        }

        if (!vm.contains("www-root"s)) {
            throw std::runtime_error("No www-root arg"s);
        }

        return args;
    }
} // namespace settings