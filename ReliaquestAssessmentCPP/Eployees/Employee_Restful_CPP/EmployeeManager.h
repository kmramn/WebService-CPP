#pragma once

struct Employee {
    int id;
    wstring name;
    int salary;
    int age;
    wstring title;
    wstring email;
};

class EmployeeManager {
private:
    map<int, Employee> employees;
    string filename = "employee_data.json";

public:
    EmployeeManager() { loadFromFile(); }

    void loadFromFile();
    void saveToFile();

    json::value getAllEmployees() const;
    json::value getEmployeeById(const int id, bool& bSuccess);
    json::value getEmployeesByNameSearch(const wstring& nameFragment);
    json::value getHighestSalaryOfEmployees();
    json::value getTop10HighestEarningEmployeeNames();
    json::value createEmployee(const json::value& body);
    json::value deleteEmployeeById(const int id);
    
    // For user input and display
    int genNextId() const;
};
