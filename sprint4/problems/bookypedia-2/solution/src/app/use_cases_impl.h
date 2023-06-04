#pragma once
#include "../domain/author_fwd.h"
#include "../domain/book_fwd.h"
#include "use_cases.h"

namespace app {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::AuthorRepository& authors, domain::BookRepository& books)
        : authors_{authors}
        , books_{books} {
    }

    void AddAuthor(const std::string& name) override;
    void AddBook(const std::string& author,
                 const std::string& title,
                 int year) override;
    void GetAuthors(std::vector<AuthorInfo>& info_authors) override;
    void GetBooks(std::vector<BookInfo>& info_books) override;
    void GetAuthorBooks(const std::string& author_id,
                                std::vector<BookInfo>& info_books) override;
private:
    domain::AuthorRepository& authors_;
    domain::BookRepository& books_;
};

}  // namespace app
