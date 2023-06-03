#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include "../domain/player_db.h"
#include "../domain/record.h"

namespace postgres {

class PlayerRepositoryImpl : public domain::PlayerRepository {
public:
    explicit PlayerRepositoryImpl(pqxx::connection& connection)
        : connection_{connection} {
    }

    void Save(const domain::Player& author) override;
    domain::players_info Get() override;
    domain::players_info Get(const int stop_time) override;
    void Update(const domain::PlayerId& id, const std::optional<long>& stop_time) override;
    void Delete(const domain::PlayerId& id) override;
    //    authors_info Get() override;
private:
    pqxx::connection& connection_;
};

class RecordRepositoryImpl : public domain::RecordRepository {
public:
    explicit RecordRepositoryImpl(pqxx::connection& connection)
            : connection_{connection} {
    }

    void Save(const domain::Record& record) override;
//    domain::records_info Get() override;
    domain::records_info Get(const uint start, const uint maxItem) override;

private:
    pqxx::connection& connection_;
};

class Database {
public:
    explicit Database(pqxx::connection connection);

    PlayerRepositoryImpl& GetPlayers() & {
        return player_;
    }

    RecordRepositoryImpl& GetRecords() & {
        return records_;
    }

private:
    pqxx::connection connection_;
    PlayerRepositoryImpl player_{connection_};
    RecordRepositoryImpl records_{connection_};
};

}  // namespace postgres