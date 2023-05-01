//
// Created by serg on 01.05.23.
//
#include "settings.h"

namespace settings {
    std::shared_ptr<Settings> Settings::settings_{};
    std::shared_ptr<Settings> Settings::GetInstance() {
        if (settings_ != nullptr)
            return settings_;
        settings_ = std::shared_ptr<Settings>(new Settings());
        return settings_;
    }
}
