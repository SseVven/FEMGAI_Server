#include "Controller.h"
#include "VoxelModel.h"

Controller::Controller()
{
}

Controller::~Controller()
{
}

void Controller::route(HttpService& router) {
    // curl -v http://ip:port/
    router.Static("/", "./dist");

    // curl -v http://ip:port/html/index.html
    router.GET("/index", [](HttpRequest* req, HttpResponse* resp) {
        return resp->File("./dist/index.html");
        });

    // curl -v http://ip:port/proxy/get
    router.Proxy("/proxy/", "http://httpbin.org/");

    // curl -v http://ip:port/ping
    router.GET("/ping", [](HttpRequest* req, HttpResponse* resp) {
        return resp->String("pong");
        });

    // curl -v http://ip:port/data
    router.GET("/data", [](HttpRequest* req, HttpResponse* resp) {
        static char data[] = "0123456789";
        return resp->Data(data, 10 /*, false */);
        });

    // curl -v http://ip:port/paths
    router.GET("/paths", [&router](HttpRequest* req, HttpResponse* resp) {
        return resp->Json(router.Paths());
        });

    // curl -v http://ip:port/get?env=1
    router.GET("/get", [](HttpRequest* req, HttpResponse* resp) {
        resp->json["origin"] = req->client_addr.ip;
        resp->json["url"] = req->url;
        resp->json["args"] = req->query_params;
        resp->json["headers"] = req->headers;
        return 200;
        });

    // curl -v http://ip:port/echo -d "hello,world!"
    router.POST("/api/echo", [](const HttpContextPtr& ctx) {
        //std::cout << ctx->body() << "\n" << ctx->type();

        // 根据信息返回体素体 外壳点面
        VoxelModel* vm = new VoxelModel(ctx->body());
        const std::string res = vm->to_string();
        delete vm;
        return ctx->send(res, ctx->type());
        });

    // curl -v http://ip:port/user/123
    router.GET("/user/{id}", [](const HttpContextPtr& ctx) {
        hv::Json resp;
        resp["id"] = ctx->param("id");
        return ctx->send(resp.dump(2));
        });

    // curl -v http://ip:port/user/123
    router.GET("/user/{id}", [](const HttpContextPtr& ctx) {
        hv::Json resp;
        resp["id"] = ctx->param("id");
        return ctx->send(resp.dump(2));
        });
}