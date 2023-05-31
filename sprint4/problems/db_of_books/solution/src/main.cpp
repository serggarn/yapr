#include <iostream>
#include <pqxx/pqxx>
#include <boost/json.hpp>
#include "json_tags.h"

using namespace std::literals;
namespace json = boost::json;
// libpqxx использует zero-terminated символьные литералы вроде "abc"_zv;
using pqxx::operator"" _zv;

int main(int argc, const char* argv[]) {
    try {
        if (argc == 1) {
            std::cout << "Usage: book_manager <conn-string>\n"sv;
            return EXIT_SUCCESS;
        } else if (argc != 2) {
            std::cerr << "Invalid command line\n"sv;
            return EXIT_FAILURE;
        }

        // Подключаемся к БД, указывая её параметры в качестве аргумента
        pqxx::connection conn{argv[1]};

        // Создаём транзакцию. Это понятие будет разобрано в следующих уроках.
        // Транзакция нужна, чтобы выполнять запросы.
        pqxx::work w(conn);

        // Используя транзакцию создадим таблицу в выбранной базе данных:
        w.exec(
                "CREATE TABLE IF NOT EXISTS books (id SERIAL PRIMARY KEY, "
                                                    "title varchar(100) NOT NULL, "
                                                    "author varchar(100) NOT NULL, "
                                                    "year integer NOT NULL, "
                                                    "ISBN char(13) UNIQUE );"_zv);

        // Применяем все изменения
        w.commit();
        bool exit = false;
        do {
            std::string request;
            std::cin >> request;
            auto json_obj = json::parse(request).as_object();
            if ( json_obj[tags::action].as_string() == "add_book" ) {
                pqxx::work w(conn);
                auto payload_obj = json_obj[tags::payload].as_object();
                if ( payload_obj.contains(tags::ISBN) ) {
                    w.exec("INSERT INTO books (title, author, year, ISBN) VALUES ('" +
                           w.esc(payload_obj[tags::title].as_string()) + "', " +
                           w.esc(payload_obj[tags::author].as_string()) + "', " +
                           std::to_string(payload_obj[tags::year].as_uint64()) + "', " +
                           w.esc(payload_obj[tags::ISBN].as_string()) + ")");
                }
                else {
                    w.exec("INSERT INTO books (title, author, year, ISBN) VALUES ('" +
                           w.esc(payload_obj[tags::title].as_string()) + "', " +
                           w.esc(payload_obj[tags::author].as_string()) + "', " +
                           std::to_string(payload_obj[tags::year].as_uint64()) + ")");
                }
                w.commit();
                json::object result;
                result[tags::result] = tags::result_map.at(true);
                std::cout << json::serialize(result);
            }
            else if (json_obj[tags::action].as_string() == "all_books") {
                pqxx::read_transaction r(conn);
                {
                    auto query_text = "SELECT id, title, author, year, ISBN FROM books ORDER BY year DESC, title, author, ISBN"_zv;

                    json::array result;
                // Выполняем запрос и итерируемся по строкам ответа
                for (auto [id, title, author, year, ISBN]: r.query<int, std::string, std::string, int, std::string>(query_text)) {
                    json::object row;
                    row[tags::id] = id;
                    row[tags::title] = title;
                    row[tags::author] = author;
                    row[tags::year] = year;
                    row[tags::ISBN] = ISBN;
                    result.push_back(row);
                }
                std::cout << json::serialize(result);
            }
            }
            else if (json_obj[tags::action].as_string() == "exit") {
                exit = true;
            }
        } while( ! exit );
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
