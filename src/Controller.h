#pragma once
#include "HttpServer.h"

class Controller
{
public:
	Controller();
	~Controller();

	// ע��·��
	void route(HttpService& router);
private:

};
