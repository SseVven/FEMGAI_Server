#include "HttpServer.h"
#include "Controller.h"

int main() {
    HttpService router;
    Controller controller;

    controller.route(router);

    http_server_t server;
    server.port = 8080;
    server.service = &router;
    http_server_run(&server);

    return 0;
}
