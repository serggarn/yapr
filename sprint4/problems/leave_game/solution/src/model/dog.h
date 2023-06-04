#pragma once
#include <iostream>
#include "../util/tagged.h"
#include <map>
#include <sstream>
#include <iomanip>
#include "../geom/geom.h"
#include <memory>

namespace model {
using CoordReal = double;
using SpeedLine = double;

template <typename T>
const std::string real_to_string(const T& t) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << t;
    return ss.str().c_str();
}

using namespace geom;

enum Direction : uint8_t {
    Empty = 0,
    North = 1,
    South = 2,
    West  = 3,
    East  = 4
};

static const std::map<Direction, std::string> direction_to_str {
        { North, "U" },
        { South, "D" },
        { West, "L" },
        { East, "R" },
        { Empty, "" },
};

static const std::map<std::string, Direction> direction_from_str {
        { "U", North },
        { "D", South },
        { "L", West },
        { "R", East },
        { "", Empty },
};

static bool is_direction_empty(const std::string& dir) {
    return direction_from_str.at(dir) == Empty;
}
class Dog {
public:
    using Id = util::Tagged<uint32_t, Dog>;
    using DogPtr = std::shared_ptr<Dog>;

    Dog(Id _id, std::string _name, Point2D _pos) noexcept
		: id(std::move(_id))
		, name (std::move(_name))
        , pos(_pos)
        , speed{0, 0}
        , dir {Empty} {};

    [[ nodiscard ]] const Id& GetId() const noexcept {
        return id;
    }
    [[ nodiscard ]] const std::string& GetName() const noexcept {
        return name;
    }
    [[ nodiscard ]] const Point2D& GetPos() const noexcept {
        return pos;
    }
    [[ nodiscard ]] const Direction& GetDir() const noexcept {
        return dir;
    }
    [[ nodiscard ]] const std::string& GetDirStr() const noexcept {
        return direction_to_str.at(dir);
    }
    [[ nodiscard ]] const Vec2D& GetSpeed() const noexcept {
        return speed;
    }
    bool IsStop() {
        return speed == Vec2D{0,0};
    }
    void SetPos(const Point2D& _pos) noexcept {
        pos = _pos;
    }

    void SetSpeed(const Vec2D& _speed) noexcept {
        speed = _speed;
    }
    void SetDir(const Direction& _dir) noexcept {
        dir = _dir;
    }
    void Stop() noexcept {
        speed = Vec2D {0, 0};
    }

    void SetMove(const SpeedLine& _speed, const std::string& _dir_str );
private:
	Id id;
	std::string name;
    Point2D pos;
    Vec2D speed;
    Direction dir;
};

} // namespace player
