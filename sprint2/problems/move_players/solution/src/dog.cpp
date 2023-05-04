#include "dog.h"

namespace model {
    void Dog::SetMove(const SpeedLine& _speed, const std::string& _dir_str ) {

        switch (direction_from_str.at(_dir_str)) {
            case North:
                speed = Speed {0, -_speed};
                break;
            case South:
                speed = Speed {0, _speed};
                break;
            case West:
                speed = Speed {-_speed, 0};
                break;
            case East:
                speed = Speed {_speed, 0};
                break;
        }
    }
} // namespace player
