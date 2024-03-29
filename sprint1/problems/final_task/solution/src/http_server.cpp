#include "http_server.h"

namespace http_server {

// Разместите здесь реализацию http-сервера, взяв её из задания по разработке асинхронного сервера
void SessionBase::Run() {
    // Вызываем метод Read, используя executor объекта stream_.
    // Таким образом вся работа со stream_ будет выполняться, используя его executor
    net::dispatch(stream_.get_executor(),
                  beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));
}
}  // namespace http_server
