#include "nitworking.h"
#include <vector>

int main() {
    NW::Server server{};

    server.init("127.0.0.1", 8080);
    server.listen();

    return 0;
}