#pragma once
#include <iostream>
#include "tagged.h"
#include <map>
#include <sstream>
#include <iomanip>

namespace model {
using CoordReal = double;
using SpeedLine = double;

template <typename T>
const std::string real_to_string(const T& t) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << t;
    return ss.str().c_str();
}

enum Direction : uint8_t {
    North = 0x00,
    South = 0x01,
    West  = 0x02,
    East  = 0x03
};

static const std::map<Direction, std::string> direction_to_str {
        { North, "U" },
        { South, "D" },
        { West, "L" },
        { East, "R" },
};

static const std::map<std::string, Direction> direction_from_str {
        { "U", North },
        { "D", South },
        { "L", West },
        { "R", East },
};


struct Position {
    CoordReal x,y;
};

struct Speed {
    SpeedLine vx, vy;
    bool operator==(Speed& rhs) {
        return vx == rhs.vx && vy == rhs.vy;
    }
};

class Dog {
public:
	using Id = util::Tagged<std::string, Dog>;
	Dog(const std::string& _name, const Position& _pos) noexcept
		: id(Id{_name})
		, name (std::move(_name))
        , pos{std::move(_pos)}
        , speed{0, 0}
        , dir {North} {};

    const Id& GetId() const noexcept {
        return id;
    }
    const std::string& GetName() const noexcept {
        return name;
    }
    const Position& GetPos() const noexcept {
        return pos;
    }
    const std::string& GetDirStr() const noexcept {
        return direction_to_str.at(dir);
    }
    const Speed& GetSpeed() const noexcept {
        return speed;
    }
    void SetPos(Position& _pos) noexcept {
        pos = std::move(_pos);
    }

    void SetSpeed(Speed& _speed) noexcept {
        speed = std::move(_speed);
    }
    void Stop() noexcept {
        speed = Speed {0, 0};
    }
    void SetMove(const SpeedLine& _speed, const std::string& _dir_str );
private:
	Id id;
	std::string name;
    Position pos;
    Speed speed;
    Direction dir;
};

} // namespace player
