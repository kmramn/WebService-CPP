#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <utility>
#include <algorithm>
#include <string>
using namespace std;

#include <tchar.h>
#include <strsafe.h>

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

#include "nlohmann/json.hpp"
//using njson = nlohmann::json;

struct Employee {
    int id;
    string name;
    int salary;
    int age;
    string title;
    string email;
};


json::value getAllEmployees() {
    json::value response = json::value::array();
    // Mock data for demo
    for (int i = 0; i < 3; ++i) {
        json::value emp;
        emp[U("id")] = json::value::number(i);
        emp[U("name")] = json::value::string(U("Employee ") + std::to_wstring(i));
        emp[U("salary")] = json::value::number(50000 + (i * 5000));
        response[i] = emp;
    }
    return response;
}

json::value getEmployeeById(int id) {
    // TODO: Implement logic
    json::value emp;
    emp[U("id")] = json::value::number(id);
    emp[U("name")] = json::value::string(U("Placeholder"));
    return emp;
}

json::value createEmployee(const json::value& body) {
    // Basic validation
    if (!body.has_field(U("name")) || !body.has_field(U("salary"))) {
        return json::value::string(U("Invalid payload"));
    }
    return body; // Echo back for now
}

json::value deleteEmployeeById(int id) {
    // TODO: Add actual deletion logic here
    return json::value::string(U("Deleted employee with ID: ") + std::to_wstring(id));
}

const wchar_t* tok = L"{}";

// trim from end of string (right)
inline std::wstring& rtrim(std::wstring& s, const wchar_t* t = tok)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::wstring& ltrim(std::wstring& s, const wchar_t* t = tok)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

void setup_routes(http_listener& listener) {
    listener.support(methods::GET, [](http_request request) {
        auto path = uri::split_path(uri::decode(request.relative_uri().path()));
        auto query = uri::split_query(request.request_uri().query());

        //if (path.size() == 3 && path[2] == U("employee")) {
        if (path.size() == 0) {
            // GET /api/v1/employee
            auto employees = getAllEmployees();
            request.reply(status_codes::OK, employees);
        }
        //else if (path.size() == 4 && path[2] == U("employee")) {
        else if (path.size() == 1) {
            // GET /api/v1/employee/{id}
            try {
                ltrim(path[0]); rtrim(path[0]);
                int id = std::stoi(path[0]);
                auto employee = getEmployeeById(id);
                request.reply(status_codes::OK, employee);
            }
            catch (const std::exception&) {
                request.reply(status_codes::BadRequest, U("Invalid employee ID"));
            }
        }
        else {
            request.reply(status_codes::NotFound, U("Invalid GET endpoint"));
        }
        });

    listener.support(methods::POST, [](http_request request) {
        request.extract_json().then([&request](json::value body) {
            auto employee = createEmployee(body);  // Stubbed
            request.reply(status_codes::Created, employee);
            });
        });

    listener.support(methods::DEL, [](http_request request) {
        auto path = uri::split_path(uri::decode(request.relative_uri().path()));

        if (path.size() == 4 && path[2] == U("employee")) {
            try {
                int id = std::stoi(path[3]);
                auto result = deleteEmployeeById(id);
                request.reply(status_codes::OK, result);
            }
            catch (const std::exception&) {
                request.reply(status_codes::BadRequest, U("Invalid employee ID"));
            }
        }
        else {
            request.reply(status_codes::NotFound, U("Invalid DELETE endpoint"));
        }
        });
}

int main() {
    web::http::experimental::listener::http_listener listener(U("http://localhost:8080/api/v1/employee"));
    //web::http::experimental::listener::http_listener listener(U("http://localhost:8080"));
    setup_routes(listener);

    try {
        listener.open().wait();
        std::cout << "Employee REST API C++ server is running..." << std::endl;
        std::cin.get();  // Keep alive
        listener.close().wait();
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }

    return 0;
}
