#pragma once
#include <string>

#include "../util/tagged_uuid.h"
#include "player_db.h"

namespace domain {

namespace detail {
    struct RecordTag {};
}  // namespace detail

using RecordId = util::TaggedUUID<detail::RecordTag>;

class Record {
public:
    Record(RecordId id, std::string name, size_t score, size_t diff)
            : id_(std::move(id))
            , name_(std::move(name))
            , score_(score)
            , diff_(diff) {
    }

    const RecordId& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    const size_t & GetScore() const noexcept {
        return score_;
    }

    const size_t& GetDiff() const noexcept {
        return diff_;
    }

private:
    RecordId id_;
    std::string name_;
    size_t score_;
    size_t diff_;

};

class RecordRepository {
public:
    using books_info = std::vector<std::pair<std::string, int>>;
    virtual void Save(const Record& record) = 0;
    virtual books_info Get() = 0;
    virtual books_info Get(const std::string& autor_id) = 0;

protected:
    ~RecordRepository() = default;
};
    
}
