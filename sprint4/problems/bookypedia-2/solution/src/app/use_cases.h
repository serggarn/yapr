#pragma once

#include <string>
#include "../ui/view.h"


namespace app {
using namespace ui::detail;
class UseCases {
public:
    virtual void AddAuthor(const std::string& name) = 0;
    virtual void AddBook(const std::string& author,
                         const std::string& title,
                         int year) = 0;

    virtual void GetAuthors(std::vector<AuthorInfo>& info_authors) = 0;
    virtual void GetBooks(std::vector<BookInfo>& info_books) = 0;
    virtual void GetAuthorBooks(const std::string& author_id,
                                std::vector<BookInfo>& info_books) = 0;

protected:
    ~UseCases() = default;
};

}  // namespace app
