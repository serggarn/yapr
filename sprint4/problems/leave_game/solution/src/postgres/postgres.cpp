#include "postgres.h"

#include <pqxx/zview.hxx>
#include <pqxx/pqxx>
#include <iostream>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void PlayerRepositoryImpl::Save(const domain::Player& author) {
    // Пока каждое обращение к репозиторию выполняется внутри отдельной транзакции
    // В будущих уроках вы узнаете про паттерн Unit of Work, при помощи которого сможете несколько
    // запросов выполнить в рамках одной транзакции.
    // Вы также может самостоятельно почитать информацию про этот паттерн и применить его здесь.
    pqxx::work work{connection_};
    work.exec_params(
        R"(
INSERT INTO authors (id, name) VALUES ($1, $2)
ON CONFLICT (id) DO UPDATE SET name=$2;
)"_zv,
        author.GetId().ToString(), author.GetJoinTime());
    work.commit();
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
//
//BookRepositoryImpl::books_info BookRepositoryImpl::Get() {
//    books_info books;
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
//BookRepositoryImpl::books_info BookRepositoryImpl::Get(const std::string& autor_id) {
//    books_info books;
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

Database::Database(pqxx::connection connection)
    : connection_{std::move(connection)} {
    pqxx::work work{connection_};
    work.exec(R"(
CREATE TABLE IF NOT EXISTS player (
    id UUID CONSTRAINT player_id_constraint PRIMARY KEY,
    jointime time NOT NULL,
    stoptime time
);
)"_zv);
    // ... создать другие таблицы
    work.exec(R"(
CREATE TABLE IF NOT EXISTS retired_players (
    id UUID CONSTRAINT book_id_constraint PRIMARY KEY,
    name varchar(100) NOT NULL,
    score int NOT NULL,
    playtime int
);
)"_zv);

    // коммитим изменения
    work.commit();
}

}  // namespace postgres