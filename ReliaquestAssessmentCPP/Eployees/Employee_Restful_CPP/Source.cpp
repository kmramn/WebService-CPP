
#include "stdafx.h"

EmployeeManager g_manager;

void setup_routes(http_listener& listener, EmployeeHandler& employeeHandler) {
    listener.support(methods::GET, [&](http_request request) {
        auto path = uri::split_path(uri::decode(request.relative_uri().path()));
        auto query = uri::split_query(request.request_uri().query());

        employeeHandler.httpGETProcess(path, query, request);

        });

    listener.support(methods::POST, [&](http_request request) {

        employeeHandler.httpPOSTProcess(request);

        });

    listener.support(methods::DEL, [&](http_request request) {
        auto path = uri::split_path(uri::decode(request.relative_uri().path()));

        employeeHandler.httpDELProcess(path, request);

        });
}

int main() {
    web::http::experimental::listener::http_listener listener(U("http://localhost:8080/api/v1/employee"));

    EmployeeHandler employeeHandler(g_manager);
    setup_routes(listener, employeeHandler);

    try {
        listener.open().wait();
        std::cout << "Employee REST API C++ server is running..." << std::endl;
        std::cout << "Press Enter to stop..." << std::endl;
        std::cin.get();  // Keep alive
        listener.close().wait();
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }

    return 0;
}
