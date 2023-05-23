#define _USE_MATH_DEFINES

#include "../src/collision_detector.h"
#include <catch2/catch_test_macros.hpp>

// Напишите здесь тесты для функции collision_detector::FindGatherEvents
using namespace collision_detector;
class FindGatherEventsTests : public ItemGathererProvider {
public:
    FindGatherEventsTests(const std::vector<Item>& items, const std::vector<Gatherer>& gathrers ) :
                            items_(std::move(items)), gatherer_(std::move(gathrers)) {}
    size_t ItemsCount() const override { return items_.size(); };
    Item GetItem(size_t idx) const override { return items_.at(idx); }
    size_t GatherersCount() const override { return gatherer_.size(); }
    Gatherer GetGatherer(size_t idx) const override { return gatherer_.at(idx); }
private:
    std::vector<Item> items_;
    std::vector<Gatherer> gatherer_;
};


TEST_CASE("FindGather"){
    SECTION("null values") {
        std::vector<Item> items;
        std::vector<Gatherer> gatherer;
        FindGatherEventsTests findtest(items, gatherer);
        CHECK(FindGatherEvents(findtest).size() == 0);
    }
    SECTION("items not null") {
        std::vector<Item> items {
            {{0,0}, 3},
            {{5,5}, 2},
            {{5, 20}, 1},
            {{10,5}, 1},
            {{15,12}, 1}
        };
        SECTION(" gatherer null") {
            std::vector<Gatherer> gatherer;
            FindGatherEventsTests findtest(items, gatherer);
            CHECK(FindGatherEvents(findtest).size() == 0);
        }
        SECTION("NULL transfer") {
            std::vector<Gatherer> gatherer {
                    {{10, 19}, {10, 19}, 1}
            };
            FindGatherEventsTests findtest(items, gatherer);
            auto events = FindGatherEvents(findtest);
            CHECK(events.size() == 0);
        }
        SECTION(" 1 catch") {
            std::vector<Gatherer> gatherer {
                    {{0, 19}, {10, 19}, 1}
            };
            FindGatherEventsTests findtest(items, gatherer);
            auto events = FindGatherEvents(findtest);
            CHECK(events.size() == 1);
            CHECK(events.at(0).gatherer_id == 0);
            CHECK(events.at(0).item_id == 2);
//            CHECK(events.at(0).item_id == 2);
        }
        SECTION(" 2 catch difference gatherer") {
            std::vector<Gatherer> gatherer {
                    {{0, 19}, {10, 19}, 1},
                    {{0, 25}, {10, 15}, 1}
            };
            FindGatherEventsTests findtest(items, gatherer);
            auto events = FindGatherEvents(findtest);
            CHECK(events.size() == 2);
            CHECK(events.at(0).item_id == 2);
            CHECK(events.at(1).item_id == 2);
            CHECK(events.at(0).gatherer_id == 0);
            CHECK(events.at(1).gatherer_id == 1);
            CHECK(events.at(0).time < events.at(1).time);
        }


    }

}