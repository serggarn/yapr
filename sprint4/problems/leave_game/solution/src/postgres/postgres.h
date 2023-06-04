#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include "../domain/player_db.h"
#include "../domain/record.h"

namespace postgres {

class PlayerRepositoryImpl : public domain::PlayerRepository {
public:
//    explicit PlayerRepositoryImpl(pqxx::connection& connection)
//        : connection_{connection} {
//    }
    explicit PlayerRepositoryImpl(pqxx::work& work)
        : work_{work} {
    }


    void Save(const domain::Player& author) override;
    domain::players_info Get() override;
    domain::players_info Get(const int stop_time) override;
    void Update(const domain::PlayerId& id, const std::optional<std::chrono::milliseconds>& stop_time) override;
    void Delete(const domain::PlayerId& id) override;

    //    authors_info Get() override;
private:
//    pqxx::connection& connection_;
    pqxx::work& work_;
};

class RecordRepositoryImpl : public domain::RecordRepository {
public:
//    explicit RecordRepositoryImpl(pqxx::connection& connection)
//            : connection_{connection} {
//    }
    explicit RecordRepositoryImpl(pqxx::work& work)
    : work_{work} {
    }
    void Save(const domain::Record& record) override;
//    domain::records_info Get() override;
    domain::records_info Get(const uint start, const uint maxItem) override;

private:
//    pqxx::connection& connection_;
    pqxx::work& work_;
};

class Database {
public:
    explicit Database(pqxx::connection connection);

    pqxx::work CreateTransaction() {
        return pqxx::work(connection_);
    }
//    PlayerRepositoryImpl& GetPlayers() & {
//        return player_;
//    }
//
//    RecordRepositoryImpl& GetRecords() & {
//        return records_;
//    }
    PlayerRepositoryImpl GetPlayers(pqxx::work& work) & {
        return PlayerRepositoryImpl{work};
    }

    RecordRepositoryImpl GetRecords(pqxx::work& work) & {
        return RecordRepositoryImpl{work};
    }

private:
    pqxx::connection connection_;
//    PlayerRepositoryImpl player_{connection_};
//    PlayerRepositoryImpl player_{work_};
//    RecordRepositoryImpl records_{connection_};
//    RecordRepositoryImpl records_{work_};
//    pqxx::work work_;
};

class DBConntrol {
public:

    void Commit() { work_.commit(); }
private:
    pqxx::work work_;

};
}  // namespace postgres