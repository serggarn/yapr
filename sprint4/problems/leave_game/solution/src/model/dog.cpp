#include "dog.h"

namespace model {
    void Dog::SetMove(const SpeedLine& _speed, const std::string& _dir_str ) {

        dir = direction_from_str.at(_dir_str);
        switch (dir) {
            case Empty:
                speed = Vec2D { 0, 0 };
                break;
            case North:
                speed = Vec2D { 0, -_speed };
                break;
            case South:
                speed = Vec2D { 0, _speed };
                break;
            case West:
                speed = Vec2D { -_speed, 0 };
                break;
            case East:
                speed = Vec2D { _speed, 0 };
                break;
        }
    }
} // namespace player
