//
// Created by serg on 01.05.23.
//

#ifndef GAME_SERVER_SETTINGS_H
#define GAME_SERVER_SETTINGS_H

#include <iostream>
#include <atomic>
#include <memory>

namespace settings {

    class Settings {
    protected:
        Settings() {};
        static std::shared_ptr<Settings> settings_;
    public:
        Settings(Settings &) = delete;

        Settings &operator=(const Settings &) = delete;

        static std::shared_ptr<Settings> GetInstance();

        const bool IsRandomStart() const noexcept { return randomize_start; }

        const bool IsTestEnv() const noexcept { return test_environment; }

        void SetRandomStart(const bool _rnd) { randomize_start = _rnd; }

        void SetTestEnv(const bool _test) { test_environment = _test; }

    private:
        bool randomize_start{true};
        bool test_environment{false};

    };



} // namespace settings

#endif //GAME_SERVER_SETTINGS_H
