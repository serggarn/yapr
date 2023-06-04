//
// Created by serg on 01.05.23.
//
#include "settings.h"
#include <cstdlib>

namespace settings {
    const std::string Settings::table_retired = "retired_players";
    const size_t records_count = 100;

    std::shared_ptr<Settings> Settings::settings_{};
    std::shared_ptr<Settings> Settings::GetInstance() {
        if (settings_ != nullptr)
            return settings_;
        settings_ = std::shared_ptr<Settings>(new Settings());
        return settings_;
    }

    void Settings::ReadDbConnectionString() {
        if (const auto* url = std::getenv(db_env.c_str())) {
            connection_string = url;
        } else {
            throw std::runtime_error(db_env + " environment variable not found");
        }
    }
}
