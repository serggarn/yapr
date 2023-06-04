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
    Record(RecordId id, std::string name, size_t score, double diff)
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

    const double& GetDiff() const noexcept {
        return diff_;
    }

private:
    RecordId id_;
    std::string name_;
    size_t score_;
    double diff_;

};

struct record_info {
    std::string name;
    uint score;
    double play_time;
};
using records_info = std::vector<record_info>;

class RecordRepository {
public:
    virtual void Save(const Record& record) = 0;
//    virtual records_info Get() = 0;
    virtual records_info Get(const uint start, const uint maxItem) = 0;

protected:
    ~RecordRepository() = default;
};
    
}
