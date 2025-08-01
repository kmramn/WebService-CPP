
#include "stdafx.h"

void EmployeeHandler::httpGETProcess(vector<utility::string_t>& path, map<utility::string_t, utility::string_t>& query, http_request request)
{
    if (path.size() == 0) {
        // GET /api/v1/employee
        auto employees = m_employeeHandler.getAllEmployees();
        request.reply(status_codes::OK, employees);
    }
    else if (path.size() == 1) {
        try {
            ltrim(path[0]); rtrim(path[0]);
            wstring val;
            if (path[0] == L"highest")
            {
                // GET /api/v1/employee/highest
                auto employee = m_employeeHandler.getHighestSalaryOfEmployees();
                request.reply(status_codes::OK, employee);
            }
            else if (path[0] == L"top10highest")
            {
                // GET /api/v1/employee/top10highest
                auto employee = m_employeeHandler.getTop10HighestEarningEmployeeNames();
                request.reply(status_codes::OK, employee);
            }
            else  if ((val = parseKeyValue(path[0], L"name=")) != L"")
            {
                // GET /api/v1/employee/{name=John}
                auto employee = m_employeeHandler.getEmployeesByNameSearch(val);
                request.reply(status_codes::OK, employee);
            }
            else
            {
                // GET /api/v1/employee/{id}
                int id = std::stoi(path[0]);
                bool bSuccess = true;
                auto employee = m_employeeHandler.getEmployeeById(id, bSuccess);
                if (bSuccess)
                    request.reply(status_codes::OK, employee);
                else
                    request.reply(status_codes::BadRequest, U("Id not found!"));
            }
        }
        catch (const std::exception&) {
            request.reply(status_codes::BadRequest, U("Invalid employee arguments"));
        }
    }
    else {
        request.reply(status_codes::NotFound, U("Invalid GET endpoint"));
    }
}

void EmployeeHandler::httpPOSTProcess(http_request request)
{
    // curl -X POST http://localhost:8080/api/v1/employee -H "Content-Type: application/json" -d "{\"employee_name\": \"God\", \"employee_salary\": 1, \"employee_age\": 777, \"employee_title\": \"Universal Systems Architect\", \"employee_email\": \"pray@prayer.com\"}"
    try {
        ucout << "Received POST request\n";
        request.extract_json().then([&](json::value body) {
            ucout << "Parsed JSON\n";
            auto employee = m_employeeHandler.createEmployee(body);  // Stubbed
            request.reply(status_codes::Created, employee);
            }).wait();
    }
    catch (const std::exception& e) {
        ucout << "JSON parsing failed: " << e.what() << std::endl;
        request.reply(status_codes::BadRequest, U("Malformed JSON"));
    }
}

void EmployeeHandler::httpDELProcess(vector<utility::string_t>& path, http_request request)
{
    // curl -X DELETE http://localhost:8080/api/v1/employee/3
    if (path.size() == 1) {
        try {
            ltrim(path[0]); rtrim(path[0]);
            int id = std::stoi(path[0]);
            auto result = m_employeeHandler.deleteEmployeeById(id);
            request.reply(status_codes::OK, result);
        }
        catch (const std::exception&) {
            request.reply(status_codes::BadRequest, U("Invalid employee ID"));
        }
    }
    else {
        request.reply(status_codes::NotFound, U("Invalid DELETE endpoint"));
    }
}
