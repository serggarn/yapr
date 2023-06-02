#include <boost/serialization/vector.hpp>

#include "model.h"
#include "player.h"

namespace geom {

template <typename Archive>
void serialize(Archive& ar, Point2D& point, [[maybe_unused]] const unsigned version) {
    ar& point.x;
    ar& point.y;
}

template <typename Archive>
void serialize(Archive& ar, Vec2D& vec, [[maybe_unused]] const unsigned version) {
    ar& vec.x;
    ar& vec.y;
}

}  // namespace geom

//namespace model {
//
//template <typename Archive>
//void serialize(Archive& ar, Loot& obj, [[maybe_unused]] const unsigned version) {
//    ar&(*obj.GetId());
//    ar&(obj.GetType());
//    ar&(obj.GetPosition());
//    ar&(obj.GetValue());
//}

//}  // namespace model

namespace serialization {

class LootRepr {
public:
    LootRepr() = default;

    explicit LootRepr(const model::Loot& loot)
        : id_(loot.GetId())
        , position_(loot.GetPosition())
        , type_(loot.GetType())
        , value_(loot.GetValue()) {}

    [[nodiscard]] model::Loot Restore() const {
        model::Loot loot{id_, type_ , position_, value_};
        return loot;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar&* id_;
        ar& type_;
        ar& position_;
        ar& value_;
    }
private:
    model::Loot::Id id_ = model::Loot::Id{0u};
    model::Point2D position_;
    model::Loot::Type type_;
    model::Loot::Value value_;
};

// DogRepr (DogRepresentation) - сериализованное представление класса Dog
class DogRepr {
public:
    DogRepr() = default;

    explicit DogRepr(const model::Dog& dog)
        : id_(dog.GetId())
        , name_(dog.GetName())
        , pos_(dog.GetPos())
        , speed_(dog.GetSpeed())
        , direction_(dog.GetDir()) {
    }

    [[nodiscard]] model::Dog Restore() const {
        model::Dog dog{id_, name_, pos_};
        dog.SetSpeed(speed_);
        dog.SetDir(direction_);
//        dog.AddScore(score_);
//        for (const auto& item : bag_content_) {
//            if (!dog.PutToBag(item)) {
//                throw std::runtime_error("Failed to put bag content");
//            }
//        }
        return dog;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar&* id_;
        ar& name_;
        ar& pos_;
        ar& speed_;
        ar& direction_;
    }

private:
    model::Dog::Id id_ = model::Dog::Id{0u};
    std::string name_;
    geom::Point2D pos_;
    geom::Vec2D speed_;
    model::Direction direction_ = model::Direction::North;
};

/* Другие классы модели сериализуются и десериализуются похожим образом */
// DogRepr (DogRepresentation) - сериализованное представление класса Dog
class PlayerRepr {
public:
    PlayerRepr() = default;

    explicit PlayerRepr(const Player& player)
            : id_{player.GetId()}
            , dog_{DogRepr(*player.GetDog())}
//            , session_{*player.GetSession()}
            , loots_{player.GetLoots()}
            , score_{player.GetScore()}{
    }

    [[nodiscard]] Player Restore() const {
        Player player{id_,
                      std::make_shared<model::Dog>(dog_.Restore()),
                              nullptr};
//                      std::make_shared<model::GameSession>(session_)};
        player.AddScore(score_);
        for (const auto& item : loots_) {
            player.AddLoot(item);
//            if (!player.AddLoot(item)) {
//                throw std::runtime_error("Failed to put bag content");
//            }
        }
        return player;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar&* id_;
        ar& dog_;
//        ar& session_;
        ar& loots_;
        ar& score_;
    }

private:
    Player::Id id_ = Player::Id{0u};
    DogRepr dog_;
//    model::GameSession session_;
    Player::Loots loots_;
    size_t score_ = 0;
};

}  // namespace serialization
