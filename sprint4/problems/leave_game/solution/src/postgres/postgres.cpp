#include "postgres.h"

#include <pqxx/zview.hxx>
#include <pqxx/pqxx>
#include <iostream>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void PlayerRepositoryImpl::Save(const domain::Player& player) {
    // Пока каждое обращение к репозиторию выполняется внутри отдельной транзакции
    // В будущих уроках вы узнаете про паттерн Unit of Work, при помощи которого сможете несколько
    // запросов выполнить в рамках одной транзакции.
    // Вы также может самостоятельно почитать информацию про этот паттерн и применить его здесь.
//    pqxx::work work{connection_};
std::cout << "player: " <<       *player.GetId()<<"; " << player.GetName()<<"; " << player.GetJoinTime().count()<<"; " << player.GetStopTime().count()<<std::endl;
    work_.exec_params(
        R"(
            INSERT INTO players (id, name, jointime, stoptime) VALUES ($1, $2, $3, $4)
            ON CONFLICT (id) DO UPDATE SET name=$2, jointime=$3, stoptime=$4;
            )"_zv,
        *player.GetId(), player.GetName(), player.GetJoinTime().count(), player.GetStopTime().count());
//    work_.commit();
}
domain::players_info PlayerRepositoryImpl::Get() {
    domain::players_info players;
    return players;
}
domain::players_info PlayerRepositoryImpl::Get(const int stop_time) {
    domain::players_info players;
//    pqxx::read_transaction r{connection_};
    std::cout << stop_time<<std::endl;
    {
        auto query_text = "SELECT id, name, jointime FROM players WHERE stoptime <= "
                          + pqxx::to_string(stop_time);

        // Выполняем запрос и итерируемся по строкам ответа
        for (auto [id, name, jointime] : work_.query<std::string, std::string, double>(query_text)) {
            players.push_back({id, name, jointime});
        }
    }

    return players;
}
void PlayerRepositoryImpl::Update(const domain::PlayerId& id, const std::optional<std::chrono::milliseconds>& stop_time) {
//    pqxx::work work{connection_};

    work_.exec_params(
            R"(
            UPDATE players SET stoptime=$2 where id=$1;
            )"_zv,
            *id, stop_time->count());
//    work_.commit();

}
void PlayerRepositoryImpl::Delete(const domain::PlayerId& id) {
//    pqxx::work work{connection_};
    std::cout << "id: " << *id <<std::endl;
    work_.exec_params(
            R"(
            DELETE FROM players WHERE id=$1;
            )"_zv,
            *id);
//    work.commit();
}

//
//AuthorRepositoryImpl::authors_info AuthorRepositoryImpl::Get() {
//    AuthorRepositoryImpl::authors_info authors;
//
//    pqxx::read_transaction r{connection_};
//    {
//        auto query_text = "SELECT id, name FROM authors ORDER BY name"_zv;
//
//        // Выполняем запрос и итерируемся по строкам ответа
//        for (auto [id, name] : r.query<std::string, std::string>(query_text)) {
//            authors.push_back({id, name});
//        }
//    }
////        read.commit();
//
//    return authors;
//}

//BookRepositoryImpl::records_info BookRepositoryImpl::Get() {
//    records_info books;
//
//    pqxx::read_transaction r{connection_};
//    {
//        auto query_text = "SELECT title, publication_year FROM books ORDER BY title"_zv;
//
//        // Выполняем запрос и итерируемся по строкам ответа
//        for (auto [title, year] : r.query<std::string, int>(query_text)) {
//            books.push_back({title, year});
//        }
//    }
//    return books;
//}
//
//BookRepositoryImpl::records_info BookRepositoryImpl::Get(const std::string& autor_id) {
//    records_info books;
//
//    pqxx::read_transaction r{connection_};
//    {
//        auto query_text = "SELECT title, publication_year FROM books WHERE author_id = '"
//                + autor_id
//                + "' ORDER BY publication_year, title";
//
//        // Выполняем запрос и итерируемся по строкам ответа
//        for (auto [title, year] : r.query<std::string, int>(query_text)) {
//            books.push_back({title, year});
//        }
//    }
//    return books;
//}
//
//void BookRepositoryImpl::Save(const domain::Book& book) {
//    // Пока каждое обращение к репозиторию выполняется внутри отдельной транзакции
//    // В будущих уроках вы узнаете про паттерн Unit of Work, при помощи которого сможете несколько
//    // запросов выполнить в рамках одной транзакции.
//    // Вы также может самостоятельно почитать информацию про этот паттерн и применить его здесь.
//    pqxx::work work{connection_};
//    work.exec_params(
//            R"(
//INSERT INTO books (id, author_id, title, publication_year) VALUES ($1, $2, $3, $4)
//ON CONFLICT (id) DO UPDATE SET author_id = $2, title=$3, publication_year = $4;
//)"_zv,
//            book.GetId().ToString(), book.GetAuthorId().ToString(), book.GetTitle(), book.GetPublicationYear());
//    work.commit();
//}
void RecordRepositoryImpl::Save(const domain::Record &record) {
//    pqxx::work work{connection_};
    std::cout << "save: " <<record.GetId().ToString()<< "; " <<  record.GetName()<< "; " <<  record.GetScore()<< "; " <<  record.GetDiff() <<std::endl;
    work_.exec_params(
            R"(
INSERT INTO retired_players (id, name, score, playtime) VALUES ($1, $2, $3, $4);
)"_zv,
            record.GetId().ToString(), record.GetName(), record.GetScore(), record.GetDiff());
//    work.commit();
}

domain::records_info RecordRepositoryImpl::Get(const uint start, const uint maxItem) {
    domain::records_info records;
//    pqxx::read_transaction r{connection_};
    {
        auto query_text = "SELECT name, score, playTime FROM retired_players ORDER BY score DESC, playtime LIMIT "
                + pqxx::to_string(maxItem) + " OFFSET "
                + pqxx::to_string(start);

        // Выполняем запрос и итерируемся по строкам ответа
        for (auto [name, score, playTime] : work_.query<std::string, uint, double>(query_text)) {
            records.push_back({name, score, playTime});
        }
    }
    return records;
}

Database::Database(pqxx::connection connection)
    : connection_{std::move(connection)} {
    pqxx::work work{connection_};
    work.exec(R"(
        CREATE TABLE IF NOT EXISTS players (
            id char(32) CONSTRAINT player_id_constraint PRIMARY KEY,
            name varchar(30) NOT NULL,
            jointime bigint NOT NULL,
            stoptime bigint
        );
        )"_zv);
    // ... создать другие таблицы
    work.exec(R"(
        CREATE TABLE IF NOT EXISTS retired_players (
            id UUID CONSTRAINT book_id_constraint PRIMARY KEY,
            name varchar(100) NOT NULL,
            score int NOT NULL,
            playtime double precision NOT NULL
        );
        )"_zv);
    work.exec(R"(
        CREATE INDEX IF NOT EXISTS score_idx ON retired_players (score);
        )"_zv);

    // коммитим изменения
    work.commit();
}

}  // namespace postgres