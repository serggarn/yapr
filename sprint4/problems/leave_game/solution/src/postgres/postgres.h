#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include "../domain/player_db.h"
#include "../domain/record.h"/

namespace postgres {

class PlayerRepositoryImpl : public domain::PlayerRepository {
public:
    explicit PlayerRepositoryImpl(pqxx::connection& connection)
        : connection_{connection} {
    }

    void Save(const domain::Player& author) override;
//    authors_info Get() override;
private:
    pqxx::connection& connection_;
};

class RecordRepositoryImpl : public domain::RecordRepository {
public:
    explicit RecordRepositoryImpl(pqxx::connection& connection)
            : connection_{connection} {
    }

    void Save(const domain::Record& book) override;
    books_info Get() override;
    books_info Get(const std::string& autor_id) override;

private:
    pqxx::connection& connection_;
};

class Database {
public:
    explicit Database(pqxx::connection connection);

//    AuthorRepositoryImpl& GetAuthors() & {
//        return authors_;
//    }
//
//    BookRepositoryImpl& GetBooks() & {
//        return books_;
//    }

private:
    pqxx::connection connection_;
//    AuthorRepositoryImpl authors_{connection_};
//    BookRepositoryImpl books_{connection_};
};

}  // namespace postgres