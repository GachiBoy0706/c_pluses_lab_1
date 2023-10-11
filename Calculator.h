#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stack>
#include <Windows.h>
#include <map>
#include "Function.h"
#include <filesystem>
#include <algorithm>
#pragma once

using fun = double (*)(std::vector<double>);


class Calculator {
private:
	
	std::vector<std::string> Parse(std::string& str);
	std::vector<std::string> Postfix_Entry(std::vector<std::string>& par, std::map < std::string, Function>& map_of_func);
	double Postfix_Calculate(std::vector<std::string>& str, std::map < std::string, Function>& map_of_func);
public:
	Calculator() = default;
	~Calculator() = default;
	
	double Calculate(std::string& string);
};

