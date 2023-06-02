#pragma once
#include "../domain/author.h"
#include "../domain/book.h"

class UnitOfWork {
public:
    virtual void Commit() = 0;
    virtual domain::AuthorRepository& Authors() = 0;
    virtual domain::BookRepository& Books() = 0;
protected:
    ~UnitOfWork() = default;
};

class UnitOfWorkFactory {
public:
    virtual UnitOfWork CreateUnitOfWork() = 0;
};
