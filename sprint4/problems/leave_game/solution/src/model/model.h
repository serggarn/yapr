#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>

#include "../util/tagged.h"
#include "dog.h"
#include "../json/lootdefinition.h"
#include <chrono>

namespace model {

using Dimension = int;
using Coord = Dimension;

struct Point {
    Coord x, y;
};

struct Size {
    Dimension width, height;
};

struct Rectangle {
    Point position;
    Size size;
};

struct Offset {
    Dimension dx, dy;
};

class Road {
    struct HorizontalTag {
        explicit HorizontalTag() = default;
    };

    struct VerticalTag {
        explicit VerticalTag() = default;
    };

public:
    constexpr static HorizontalTag HORIZONTAL{};
    constexpr static VerticalTag VERTICAL{};
    constexpr static double HALF_WIDTH = 0.4;

    Road(HorizontalTag, Point start, Coord end_x) noexcept
        : start_{start}
        , end_{end_x, start.y} {
    }

    Road(VerticalTag, Point start, Coord end_y) noexcept
        : start_{start}
        , end_{start.x, end_y} {
    }

    bool IsHorizontal() const noexcept {
        return start_.y == end_.y;
    }

    bool IsVertical() const noexcept {
        return start_.x == end_.x;
    }

    Point GetStart() const noexcept {
        return start_;
    }

    Point GetEnd() const noexcept {
        return end_;
    }

    bool CheckPoint(const Point& point) const noexcept;

    bool CheckPoint(const Point2D& point) const noexcept;

private:
    Point start_;
    Point end_;
};

class Building {
public:
    explicit Building(Rectangle bounds) noexcept
        : bounds_{bounds} {
    }

    const Rectangle& GetBounds() const noexcept {
        return bounds_;
    }

private:
    Rectangle bounds_;
};

class Office {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point position, Offset offset) noexcept
            : id_{std::move(id)}
            , position_{position}
            , offset_{offset} {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    Point GetPosition() const noexcept {
        return position_;
    }

    Offset GetOffset() const noexcept {
        return offset_;
    }

private:
    Id id_;
    Point position_;
    Offset offset_;
};

class Loot {
public:
    using Id = util::Tagged<std::uint64_t, Loot>;
    using Type = uint64_t;
    using Value = size_t;

    Loot(Type type, Point2D position, Value value) noexcept
        : id_{next_id++}
        , type_{type}
        , position_{position}
        , value_{value} { }

    Loot(Id id, Type type, Point2D position, Value value) noexcept
            : id_{id}
            , type_{type}
            , position_{position}
            , value_{value} { next_id = std::max(next_id, *id_); next_id++; }

    [[nodiscard]] const Id& GetId() const noexcept {
        return id_;
    }

    [[nodiscard]] Point2D GetPosition() const noexcept {
        return position_;
    }

    [[nodiscard]] Type GetType() const noexcept {
        return type_;
    }

    [[nodiscard]] Value GetValue() const noexcept {
        return value_;
    }

    [[nodiscard]] auto operator<=>(const Loot&) const = default;
private:
    Id id_;
    Point2D position_;
    Type type_;
    static uint64_t next_id;
    Value value_;
};

class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<std::shared_ptr<Road>>;
    using MapRoad1D = std::map<Coord, std::shared_ptr<Road>>;
    using MapRoads = std::map<Coord, MapRoad1D>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;
    using Loots = std::vector<Loot>;
    using LootsDefinitions = std::vector<LootDefinition>;

    Map(Id id, std::string name, double dogSpeed, size_t bagCapacity) noexcept
        : id_(std::move(id))
        , name_(std::move(name))
        , dogSpeed_(std::move(dogSpeed))
        , bagCapacity_(std::move(bagCapacity)) {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    const Buildings& GetBuildings() const noexcept {
        return buildings_;
    }

    const Roads& GetRoads() const noexcept {
        return roads_;
    }

    const Offices& GetOffices() const noexcept {
        return offices_;
    }

    const Loots& GetLoots() const noexcept {
        return loots_;
    }

    void RemoveLoot(const Loot::Id id);


    const LootsDefinitions & GetLootsDefinitions() const noexcept {
        return loots_definitions_;
    }

    void AddRoad(const Road& road);

    void AddBuilding(const Building& building) {
        buildings_.emplace_back(building);
    }

    void AddOffice(const Office& office);

    const double& GetDogSpeed() const noexcept { return dogSpeed_; }

    const std::pair <std::shared_ptr<Road>, std::shared_ptr<Road>> GetRoadsByCoord(const Point& pos) const;

    void PrintVHRoads() const;

    void AddLootDefinition(const LootDefinition& definition);

    void AddLoot(const Loot& loot);

    size_t GetBagCapacity() const noexcept { return bagCapacity_; }

    std::shared_ptr<Loot> GetLoot(const size_t index) const noexcept;

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    MapRoads vert_roads_;
    MapRoads hor_roads_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;
    double dogSpeed_;
    Loots loots_;
    LootsDefinitions loots_definitions_;
    size_t bagCapacity_;
};



}  // namespace model
