#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <catch2/catch_test_macros.hpp>
#include <sstream>

#include "../src/model/model.h"
#include "../src/model/model_serialization.h"
//#include "../src/system/backuph.h"

using namespace model;
using namespace std::literals;
namespace {

using InputArchive = boost::archive::text_iarchive;
using OutputArchive = boost::archive::text_oarchive;

struct Fixture {
    std::stringstream strm;
    OutputArchive output_archive{strm};
};

}  // namespace

SCENARIO_METHOD(Fixture, "Point serialization") {
    GIVEN("A point") {
        const geom::Point2D p{10, 20};
        WHEN("point is serialized") {
            output_archive << p;

            THEN("it is equal to point after serialization") {
                InputArchive input_archive{strm};
                geom::Point2D restored_point;
                input_archive >> restored_point;
                CHECK(p == restored_point);
            }
        }
    }
}

SCENARIO_METHOD(Fixture, "Loots Serialization") {
    GIVEN(" loots") {
        const auto loot = [] {
            Loot loot{Loot::Id{23}, Loot::Type{1}, {42.2, 12.5}, Loot::Value{27}};
            return loot;
        }();

        WHEN("loot is serialized") {
            {
                serialization::LootRepr repr{loot};
                output_archive << repr;
            }
            THEN("it can be deserialized") {
                InputArchive input_archive{strm};
                serialization::LootRepr repr;
                input_archive >> repr;
                const auto restored = repr.Restore();
                CHECK(loot.GetId() == restored.GetId());
                CHECK(loot.GetType() == restored.GetType());
                CHECK(loot.GetValue() == restored.GetValue());
                CHECK(loot.GetPosition() == restored.GetPosition());
            }
        }
    }
}
//SCENARIO_METHOD(Fixture, "Dog Serialization") {
//    GIVEN("a dog") {
//        const auto dog = [] {
//            Dog dog{Dog::Id{42}, "Pluto"s, {42.2, 12.5}, 3};
//            dog.AddScore(42);
//            CHECK(dog.PutToBag({FoundObject::Id{10}, 2u}));
//            dog.SetDirection(Direction::EAST);
//            dog.SetSpeed({2.3, -1.2});
//            return dog;
//        }();
//
//        WHEN("dog is serialized") {
//            {
//                serialization::DogRepr repr{dog};
//                output_archive << repr;
//            }
//
//            THEN("it can be deserialized") {
//                InputArchive input_archive{strm};
//                serialization::DogRepr repr;
//                input_archive >> repr;
//                const auto restored = repr.Restore();
//
//                CHECK(dog.GetId() == restored.GetId());
//                CHECK(dog.GetName() == restored.GetName());
//                CHECK(dog.GetPosition() == restored.GetPosition());
//                CHECK(dog.GetSpeed() == restored.GetSpeed());
//                CHECK(dog.GetBagCapacity() == restored.GetBagCapacity());
//                CHECK(dog.GetBagContent() == restored.GetBagContent());
//            }
//        }
//    }
//}
