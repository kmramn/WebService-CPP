Development Environment

This solution was built using:

    Microsoft Visual C++ 2022

    C++17 standard

All code strictly adheres to idiomatic C++17 conventions, ensuring type safety and maintainability. Project settings and dependencies (e.g., cpprestsdk, vcpkg) are configured for reproducibility across compatible Windows environments.

This project implements the ReliaQuest Employee API functionality using C++. It includes two console applications:
Project Structure

    Console: A lightweight test project used to validate the core logic of EmployeeManager.

    RESTFul_CPP: The main RESTful API client, which implements endpoints against the mock server. Compiles into RESTFul_CPP.exe and should be executed from the command prompt.

After building RESTFul_CPP, RESTFul_CPP.exe, run it from the command line to interact with the mock Employee API. The project uses idiomatic, type-safe C++ and clean abstractions for RESTful calls.

Endpoints Handled by RESTFul_CPP

The client communicates with the mock Employee API hosted at:
http://localhost:8112/api/v1/employee

Supported operations include:

    getAllEmployees() → GET /employee → Returns the full list of employees.

    getEmployeesByNameSearch("John") → GET /employee/{name=John} → Returns employees whose names match or contain the given fragment.

    getEmployeeById("employee-id") → GET /employee/{id} → Returns employee details by unique ID.

    getHighestSalaryOfEmployees() → GET /employee/highest → Returns the highest salary among all employees.

    getTop10HighestEarningEmployeeNames() → GET /employee/top10highest → Returns names of the top 10 highest-paid employees.

    createEmployee(...) → POST /employee → Creates a new employee from a structured input object.

    deleteEmployeeById("employee-id") → DELETE /employee/{id} → Deletes the employee corresponding to the specified ID.

Sample API Usage

Creating a new employee:

POST /api/v1/employee
Content-Type: application/json

{
  "employee_name": "God",
  "employee_salary": 1,
  "employee_age": 777,
  "employee_title": "Universal Systems Architect",
  "employee_email": "pray@prayer.com"
}

Deleting an employee by ID:
DELETE /api/v1/employee/3

Data Persistence

All employee data retrieved or created by the C++ application is persistently stored in a local file:
Employee_Restful_CPP/employee_data.json f the application is run using Visual Studio C++ or if it run using cmd prompt, the json will be in the same place as in EXE.
