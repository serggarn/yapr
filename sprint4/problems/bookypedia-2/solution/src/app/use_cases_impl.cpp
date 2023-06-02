#include "use_cases_impl.h"

#include "../domain/author.h"
#include "../domain/book.h"

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string& name) {
    authors_.Save({AuthorId::New(), name});
}

void UseCasesImpl::AddBook(const std::string& author_id,
                           const std::string& title,
                           int year) {
    auto autor_uuid = AuthorId{util::detail::UUIDFromString(author_id)};
    books_.Save({BookId::New(), autor_uuid, title, year});
}

void UseCasesImpl::GetAuthors(std::vector<AuthorInfo>& info_authors) {

    for (const auto& author : authors_.Get()) {
        info_authors.push_back({author.first, author.second});
    }
}

void UseCasesImpl::GetBooks(std::vector<BookInfo>& info_books) {
    for (const auto& book : books_.Get()) {
        info_books.push_back({book.first, book.second});
    }
}

void UseCasesImpl::GetAuthorBooks(const std::string& author_id,
                    std::vector<BookInfo>& info_books) {
    for (const auto& book : books_.Get(author_id)) {
        info_books.push_back({book.first, book.second});
    }
}

}  // namespace app
