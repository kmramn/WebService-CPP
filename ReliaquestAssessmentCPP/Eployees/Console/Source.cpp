
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <utility>
#include <algorithm>
using namespace std;

#include <tchar.h>
#include <strsafe.h>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

struct Employee {
    int id;
    string name;
    int salary;
    int age;
    string title;
    string email;
};

class EmployeeManager {
private:
    map<int, Employee> employees;
    string filename = "employee_data.json";

public:
    EmployeeManager() { loadFromFile(); }

    void loadFromFile() {
        ifstream infile(filename);
        if (!infile) return;

        json j;
        infile >> j;

        for (const auto& item : j) {
            Employee emp = {
                item["id"], item["employee_name"], item["employee_salary"],
                item["employee_age"], item["employee_title"], item["employee_email"]
            };
            employees[emp.id] = emp;
        }
    }

    void saveToFile() {
        json j = json::array();
        for (const auto& [id, emp] : employees) {
            j.push_back({
                {"id", emp.id},
                {"employee_name", emp.name},
                {"employee_salary", emp.salary},
                {"employee_age", emp.age},
                {"employee_title", emp.title},
                {"employee_email", emp.email}
                });
        }
        ofstream outfile(filename);
        outfile << j.dump(4);
    }

    vector<Employee> getAllEmployees() const {
        vector<Employee> result;
        for (const auto& [id, emp] : employees)
            result.push_back(emp);
        return result;
    }

    vector<Employee> getEmployeesByNameSearch(const string& nameFragment) {
        vector<Employee> result;
        for (const auto& [id, emp] : employees) {
            if (emp.name.find(nameFragment) != string::npos)
                result.push_back(emp);
        }
        return result;
    }

    Employee* getEmployeeById(const int id) {
        if (employees.count(id))
            return &employees[id];
        return nullptr;
    }

    int getHighestSalaryOfEmployees() {
        auto it = max_element(
            employees.begin(),
            employees.end(),
            [](const pair<int, Employee>& a, const pair<int, Employee>& b) {
                return a.second.salary < b.second.salary;
            });

        if (it != employees.end())
            return it->second.salary;
        else return 0;
    }

    vector<Employee> getTop10HighestEarningEmployeeNames() {
        vector<Employee> result;
        for (const auto& [id, emp] : employees)
            result.push_back(emp);
        sort(result.begin(), result.end(), [](auto& a, auto& b) {
            return a.salary > b.salary;
            });
        if (result.size() > 10)
            result.resize(10);
        return result;
    }

    bool createEmployee(const Employee& emp) {

        employees[emp.id] = emp;
        return true;
    }

    string deleteEmployeeById(const int id) {
        if (employees.count(id)) {
            string name = employees[id].name;
            employees.erase(id);
            return name;
        }
        return "";
    }
    

    // For user input and display

    int genNextId() const
    {
        if (employees.size() == 0) return 1;
        else return employees.rbegin()->first+1;
    }

    void getEmployeeFromUser()
    {
        Employee newEmp;
        newEmp.id = genNextId();
        cout << "Employee name:";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        getline(cin, newEmp.name);
        cout << "Employee salary:";
        cin >> newEmp.salary;
        cout << "Employee age:";
        cin >> newEmp.age;
        cout << "Employee title:";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        getline(cin, newEmp.title);
        cout << "Employee email:";
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cin>>newEmp.email;

        createEmployee(newEmp);
        cout << endl;
    }

    void displayEmployee(const Employee& emp)
    {
        cout << "Employee Id:" << emp.id<< endl;
        cout << "Employee name:" << emp.name << endl;
        cout << "Employee salary:" << emp.salary << endl;
        cout << "Employee age:" << emp.age << endl;
        cout << "Employee title:" << emp.title << endl;
        cout << "Employee email:" << emp.email << endl << endl;
    }

    void listAllEmployees()
    {
        cout << "*** Employees ***" << endl;
        if (employees.size() == 0) cout << "No employees." << endl;
        else {
            for (const auto& [id, emp] : employees)
            {
                displayEmployee(emp);
            }
        }
        cout << "*** Employees ***" << endl << endl;
    }

    void displayEmployees(const vector<Employee>& vecEmpList)
    {
        cout << "*** Employees ***" << endl;
        if (employees.size() == 0) cout << "No employees." << endl;
        else {
            for (const auto& emp : vecEmpList)
            {
                displayEmployee(emp);
            }
        }
        cout << "*** Employees ***" << endl << endl;
    }
};


int wmain()
{
    EmployeeManager manager;

    bool bContinue = true;
    int nId = 0;
    string szName;
    while (bContinue)
    {
        cout << "1. Create Employee" << endl;
        cout << "2. Get All Employees" << endl;
        cout << "3. Get Employees by Name" << endl;
        cout << "4. Get Employees by Id" << endl;
        cout << "5. Get Highest Salary of Employees" << endl;
        cout << "6. Get Top 10 Highest Earning Employee Names" << endl;
        cout << "7. Delete Employee by Id" << endl;
        cout << "8. Quit" << endl;
        int nOption = 0;
        cout << "Please enter the option:";
        cin >> nOption;
        cout << endl;
        switch (nOption)
        {
        case 1:
            manager.getEmployeeFromUser();
            break;
        case 2:
            manager.listAllEmployees();
            break;
        case 3:
            cout << "Please enter the Name:";
            cin >> szName;
            manager.displayEmployees(manager.getEmployeesByNameSearch(szName));
            break;
        case 4:
            cout << "Please enter the Id:";
            cin >> nId;
            cout << "*** Employees ***" << endl;
            manager.displayEmployee(*manager.getEmployeeById(nId));
            cout << "*** Employees ***" << endl << endl;
            break;
        case 5:
            cout << "Highest Salary:" << manager.getHighestSalaryOfEmployees() << endl << endl;
            break;
        case 6:
            manager.displayEmployees(manager.getTop10HighestEarningEmployeeNames());
            break;
        case 7:
            cout << "Please enter the Id:";
            cin >> nId;
            manager.deleteEmployeeById(nId);
            break;
        case 8:
            manager.saveToFile();
            bContinue = false;
            break;
        }
    }

    return 0;
}
