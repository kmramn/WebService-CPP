#pragma once

class EmployeeHandler
{
	EmployeeManager& m_employeeHandler;
public:
	EmployeeHandler(EmployeeManager& employeeHandler) : m_employeeHandler(employeeHandler)
	{ }

	void httpGETProcess(vector<utility::string_t>& path, map<utility::string_t, utility::string_t>& query, http_request request);
	void httpPOSTProcess(http_request request);
	void httpDELProcess(vector<utility::string_t>& path, http_request request);
};