#pragma once
#include <string>

#include "../util/tagged_uuid.h"
#include "author.h"

namespace domain {

namespace detail {
    struct BookTag {};
}  // namespace detail

using BookId = util::TaggedUUID<detail::BookTag>;

class Book {
public:
    Book(BookId id, AuthorId author_id, std::string title, int publication_year)
            : id_(std::move(id))
            , author_id_(author_id)
            , title_(std::move(title))
            , publication_year_(publication_year) {
    }

    const BookId& GetId() const noexcept {
        return id_;
    }

    const std::string& GetTitle() const noexcept {
        return title_;
    }

    const AuthorId& GetAuthorId() const noexcept {
        return author_id_;
    }

    const int& GetPublicationYear() const noexcept {
        return publication_year_;
    }

private:
    BookId id_;
    AuthorId author_id_;
    std::string title_;
    int publication_year_;

};

class BookRepository {
public:
    using books_info = std::vector<std::pair<std::string, int>>;
    virtual void Save(const Book& Book) = 0;
    virtual books_info Get() = 0;
    virtual books_info Get(const std::string& autor_id) = 0;

protected:
    ~BookRepository() = default;
};
    
}
