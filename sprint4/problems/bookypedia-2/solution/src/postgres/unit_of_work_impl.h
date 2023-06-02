#pragma once
#include "postgres.h"
#include "../app/unit_of_work.h"

using namespace postgres;

class UnitOfWorkImpl : public UnitOfWork {
public:
    explicit UnitOfWorkImpl(pqxx::connection& connection)
    : connection_{connection} {
    }
private:
    AuthorRepositoryImpl author_repo_;
    BookRepositoryImpl book_repo_;
    pqxx::connection& connection_

};



