
#include "stdafx.h"

std::string ws2s(const std::wstring& wstr) {
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(wstr);
}

std::wstring s2ws(const std::string& str) {
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.from_bytes(str);
}

void EmployeeManager::loadFromFile() {
    std::ifstream infile(filename);
    if (!infile) return;

    njson j;
    infile >> j;

    for (const auto& item : j) {
        Employee emp = {
            item["id"].get<int>(),
            s2ws(item["employee_name"].get<std::string>()),
            item["employee_salary"].get<int>(),
            item["employee_age"].get<int>(),
            s2ws(item["employee_title"].get<std::string>()),
            s2ws(item["employee_email"].get<std::string>())
        };
        employees[emp.id] = emp;
    }
}

void EmployeeManager::saveToFile() {
    njson j = njson::array();
    for (const auto& [id, emp] : employees) {
        j.push_back({
            {"id", emp.id},
            {"employee_name", ws2s(emp.name)},
            {"employee_salary", emp.salary},
            {"employee_age", emp.age},
            {"employee_title", ws2s(emp.title)},
            {"employee_email", ws2s(emp.email)}
            });
    }
    std::ofstream outfile(filename);
    outfile << j.dump(4);
}

json::value EmployeeManager::getAllEmployees() const {
    json::value response = json::value::array();
    // Mock data for demo
    int i = 0;
    for (const auto& [id, emp] : employees) {
        json::value remp;
        remp[U("id")] = json::value::number(id);
        remp[U("employee_name")] = json::value::string(emp.name);
        remp[U("employee_salary")] = json::value::number(emp.salary);
        remp[U("employee_age")] = json::value::number(emp.age);
        remp[U("employee_title")] = json::value::string(emp.title);
        remp[U("employee_email")] = json::value::string(emp.email);
        response[i++] = remp;
    }
    return response;
}

json::value EmployeeManager::getEmployeeById(const int id, bool& bSuccess) {
    json::value response = json::value::array();
    // Mock data for demo
    int i = 0;
    if (employees.count(id))
    {
        json::value remp;
        remp[U("id")] = json::value::number(employees[id].id);
        remp[U("employee_name")] = json::value::string(employees[id].name);
        remp[U("employee_salary")] = json::value::number(employees[id].salary);
        remp[U("employee_age")] = json::value::number(employees[id].age);
        remp[U("employee_title")] = json::value::string(employees[id].title);
        remp[U("employee_email")] = json::value::string(employees[id].email);
        response[i] = remp;
        bSuccess = true;
        return response;
    }
    else
    {
        bSuccess = false;
        return json::value::string(U("Id not found!"));
    }
}

json::value EmployeeManager::getEmployeesByNameSearch(const wstring& nameFragment) {
    json::value response = json::value::array();
    // Mock data for demo
    int i = 0;
    for (const auto& [id, emp] : employees)
    {
        json::value remp;
        if (emp.name.find(nameFragment) != string::npos)
        {
            remp[U("id")] = json::value::number(emp.id);
            remp[U("employee_name")] = json::value::string(emp.name);
            remp[U("employee_salary")] = json::value::number(emp.salary);
            remp[U("employee_age")] = json::value::number(emp.age);
            remp[U("employee_title")] = json::value::string(emp.title);
            remp[U("employee_email")] = json::value::string(emp.email);
            response[i++] = remp;
        }
    }
    return response;
}

json::value EmployeeManager::getHighestSalaryOfEmployees() {
    auto it = max_element(
        employees.begin(),
        employees.end(),
        [](const pair<int, Employee>& a, const pair<int, Employee>& b) {
            return a.second.salary < b.second.salary;
        });

    json::value response = json::value::array();
    json::value remp;
    remp[U("Highest Salary")] = json::value::number(0);
    if (it != employees.end())
        remp[U("Highest Salary")] = json::value::number(it->second.salary);
    response[0] = remp;
    return response;
}

json::value EmployeeManager::getTop10HighestEarningEmployeeNames() {
    vector<Employee> result;
    for (const auto& [id, emp] : employees)
        result.push_back(emp);
    sort(result.begin(), result.end(), [](auto& a, auto& b) {
        return a.salary > b.salary;
        });
    if (result.size() > 10)
        result.resize(10);

    json::value response = json::value::array();
    // Mock data for demo
    int i = 0;
    for (const auto& emp : result)
    {
        json::value remp;
        remp[U("name")] = json::value::string(emp.name);
        response[i++] = remp;
    }
    return response;
}

json::value EmployeeManager::createEmployee(const json::value& body) {
    // Validate presence of required fields
    if (!body.has_field(U("employee_name")) ||
        !body.has_field(U("employee_salary")) ||
        !body.has_field(U("employee_age")) ||
        !body.has_field(U("employee_title")) ||
        !body.has_field(U("employee_email")))
    {
        return json::value::string(U("Invalid payload"));
    }

    // Extract and type-check each value
    Employee newEmp;
    newEmp.id = genNextId();
    newEmp.name = body.at(U("employee_name")).as_string();
    newEmp.salary = body.at(U("employee_salary")).as_integer();
    newEmp.age = body.at(U("employee_age")).as_integer();
    newEmp.title = body.at(U("employee_title")).as_string();
    newEmp.email = body.at(U("employee_email")).as_string();
    employees[newEmp.id] = newEmp;
    saveToFile();

    // Echo back as structured JSON response (or construct custom object)
    json::value response = json::value::object();
    response[U("employee_name")] = json::value::string(newEmp.name);
    response[U("employee_salary")] = json::value::number(newEmp.salary);
    response[U("employee_age")] = json::value::number(newEmp.age);
    response[U("employee_title")] = json::value::string(newEmp.title);
    response[U("employee_email")] = json::value::string(newEmp.email);

    return response;
}

json::value EmployeeManager::deleteEmployeeById(const int id) {
    if (employees.count(id)) {
        wstring name = employees[id].name;
        employees.erase(id);
        saveToFile();
        return json::value::string(U("Deleted employee with ID: ") + std::to_wstring(id));
    }
    return json::value::string(U("Id not found:") + std::to_wstring(id));
}
    
// For user input and display
int EmployeeManager::genNextId() const
{
    if (employees.size() == 0) return 1;
    else return employees.rbegin()->first+1;
}
