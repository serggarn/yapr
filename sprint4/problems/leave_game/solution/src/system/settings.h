//
// Created by serg on 01.05.23.
//

#pragma once

#include <iostream>
#include <atomic>
#include <memory>
#include <chrono>

namespace settings {

    using ms = std::chrono::milliseconds;
    class Settings {
    protected:
        Settings() {};
        static std::shared_ptr<Settings> settings_;
    public:
        Settings(Settings &) = delete;

        Settings &operator=(const Settings &) = delete;

        static std::shared_ptr<Settings> GetInstance();

        bool IsRandomStart() const noexcept { return randomize_start; }

        bool IsTestEnv() const noexcept { return test_environment; }

        void SetRandomStart(const bool _rnd) { randomize_start = _rnd; }

        void SetSaveStateFlag(const bool _save_state) { save_state = _save_state; }

        bool IsSaveState() const noexcept { return save_state; }

        void SetSavePeriodFlag(const bool _save_period) { save_period_flag = _save_period; }

        bool IsSavePeriod() const noexcept { return save_period_flag; }

        void SetSavePeriod(const size_t period_ms) { save_period = ms {period_ms};}

        ms GetSavePeriod(const size_t ms) { return save_period; }

        void SetTestEnv(const bool _test) { test_environment = _test; }

        void ReadDbConnectionString();

        [[nodiscard]] const std::string& GetDbConnectionString() const { return connection_string; }

        static const std::string table_retired;
        static const size_t records_count;

    private:
        bool randomize_start{true};
        bool test_environment{false};
        bool save_state{false};
        bool save_period_flag{false};
        std::chrono::milliseconds save_period{0};
        const std::string db_env = "GAME_DB_URL";
        std::string connection_string;

    };



} // namespace settings
