#include "Calculator.h"
#include <stdio.h>
#include  <ctype.h>

double Add(std::vector<double> vec) {
	return vec[0]+vec[1];
}
double Sub(std::vector<double> vec) {
	return vec[0] - vec[1];
}
double Mul(std::vector<double> vec) {
	return vec[0] * vec[1];
}
double Div(std::vector<double> vec) {
	return vec[0] / vec[1];
}




std::vector<std::string> get_plugins_filenames() {
	const std::filesystem::path plugins{ "Debug/plugins" };
	std::vector<std::string> files = {};
	for (auto const& dir_entry : std::filesystem::directory_iterator{ plugins })
		if (dir_entry.path().extension() == ".dll") {

			std::stringstream ss;

			ss << dir_entry.path().filename();
			std::string s;
			ss >> s;
			std::string ans(s.begin() + 1, s.end() - 1);
			files.push_back(ans);
		}
	return files;
};

std::map<std::string, Function> get_map_of_functions(std::vector<std::string>& dll_names) {
	std::map<std::string, Function> map_of_func;
	Function add("+", 2, Add, 0);
	Function sub("-", 2, Sub, 0);
	Function mul("*", 2, Mul, 1);
	Function div("+", 2, Div, 1);
	Function brace_op("(", 0, nullptr, -1);
	Function brace_cl(")", 0, nullptr, -1);
	map_of_func["+"] = add;
	map_of_func["-"] = sub;
	map_of_func["*"] = mul;
	map_of_func["/"] = div;
	map_of_func["("] = brace_op;
	map_of_func[")"] = brace_cl;
	HINSTANCE load;
	using fun_str = std::string(*)();
	using fun_int = int(*)();
	for (std::string tmp : dll_names) {
		std::stringstream ss;
		ss << "./plugins/";
		ss << tmp;
		std::string s;
		ss >> s;
		std::wstring ws(s.begin(), s.end());
		load = LoadLibrary(ws.c_str());
		if (!load) {
			std::cout << "Lib wasn't succesfuly loaded" << std::endl;
			continue;
		}
		fun_str name_fun_ptr = (fun_str)GetProcAddress(load, "GetName");
		if (!name_fun_ptr) {
			std::cout << "Lib Troubles with name" << std::endl;
			continue;
		}
		fun_str arity_fun_ptr = (fun_str)GetProcAddress(load, "GetArity");
		if (!arity_fun_ptr) {
			std::cout << "Lib Troubles with arity" << std::endl;
			continue;
		}
		fun fun_ptr = (fun)GetProcAddress(load, "GetFun");
		if (!fun_ptr) {
			std::cout << "Lib Troubles with fun" << std::endl;
			continue;
		}
		int arity;
		if (arity_fun_ptr() == "unary")
arity = 1;
		else
		arity = 2;
		fun_int priority_int = (fun_int)GetProcAddress(load, "GetPriority");
		if (!priority_int) {
			std::cout << "Lib Troubles with priority" << std::endl;
			continue;
		}


		map_of_func[name_fun_ptr()] = Function(name_fun_ptr(), arity, fun_ptr, priority_int());
	}
	return map_of_func;
}


std::vector<std::string> Calculator::Parse(std::string& str) {
	std::vector<std::string> parsed;
	unsigned int ch = 0;//counter
	int balance = 0;//for braces balance
	while (ch < str.size()) {

		if (str[ch] == ' ') {
			++ch;
			continue;
		}

		if (balance < 0)
			throw "incorrect closing braces";

		//writing a word
		if (str[ch] >= 'a' && str[ch] <= 'z') {
			std::string s;//transformation char into string
			std::stringstream ss;
			while (str[ch] >= 'a' && str[ch] <= 'z') {
				ss << str[ch];
				++ch;
			}
			ss >> s;
			parsed.push_back(s);
		}
		//single-character operators and brackets
		if (str[ch] == '+' || str[ch] == '-' || str[ch] == '*' || str[ch] == '/' || str[ch] == '^') {

			std::string s;//transformation char into string
			std::stringstream ss;
			ss << str[ch];
			ss >> s;
			parsed.push_back(s);
			++ch;

			continue;
		}
		if (str[ch] == '(' || str[ch] == ')') {
			std::string s;//transformation char into string
			std::stringstream ss;
			ss << str[ch];
			ss >> s;
			parsed.push_back(s);
			if (str[ch] == '(')
				++balance;
			if (str[ch] == ')')
				--balance;
			++ch;
			continue;
		}
		//writing a word
		if (int(str[ch]) >= '0' && int(str[ch]) <= '9' && ch < str.size()) {
			int number = 0;
			while (int(str[ch]) >= 48 && int(str[ch]) <= 57 && ch < str.size()) {
				number = number * 10 + int(str[ch]) - 48;
				++ch;
			}
			parsed.push_back(std::to_string(number));
		}
	}
	if (balance > 0)
		throw "incorrect opening braces";
	if (balance < 0)
		throw "incorrect closing braces";
	return parsed;
}

std::vector<std::string> Calculator::Postfix_Entry(std::vector<std::string>& parsed, std::map < std::string, Function>& map_of_func) {
	std::stack<std::string> stack;
	std::vector<std::string> answer;
	//checking for the correctness of the sequence
	for (int i = 0; i < parsed.size() - 1; ++i) {
		if (std::isalpha(parsed[i][0])) {
			if (std::isalpha(parsed[i + 1][0]))
				throw "Too many functions in a row";
			if (i + 1 == parsed.size() - 1)
				throw "Missing operands in the end of string";
			if (parsed[i + 1][0] != '(')
				throw "Missing bracket after function";
		}
		else if (std::isdigit(parsed[i][0])) {
			if (std::isdigit(parsed[i + 1][0]))
				throw "Too many operands in a row";
			if (parsed[i + 1][0] == '(')
				throw "Operand without operation before opening bracket";
			if (i + 1 == parsed.size() - 1 && (parsed[i + 1][0]) != ')')
				throw "Missing operands in the end of string";
		}
		else if (parsed[i][0] == '+' || parsed[i][0] == '-' || parsed[i][0] == '*' || parsed[i][0] == '/' || parsed[i][0] == '^') {
			if (i == 0)
				throw "The sequence can`t start within an operation";
			if (parsed[i + 1][0] == '+' || parsed[i + 1][0] == '-' || parsed[i + 1][0] == '*' || parsed[i + 1][0] == '/' || parsed[i + 1][0] == '^')
				throw "Too many operations in a row";

		}
		else if (parsed[i][0] == '(' && parsed[i + 1][0] == ')')
			throw "Empty brackets";

	}


	if (parsed[0] == "-")
		answer.push_back("0");

	for (std::string& tmp : parsed) {
		if (tmp[0] >= '0' && tmp[0] <= '9') {
			answer.push_back(tmp);
			continue;
		}
		if (tmp == "(") {
			stack.push("(");
			if (*(&tmp + 1) == "-")
				answer.push_back("0");
			continue;
		}
		if (tmp == ")") {
			std::string prom;
			while (stack.top() != "(") {
				prom = stack.top();
				stack.pop();
				answer.push_back(prom);
			}
			stack.pop();
			continue;
		}
		else {
			std::string prom;
			while (!stack.empty() && map_of_func[stack.top()].GetPriority() >= map_of_func[tmp].GetPriority()) {
				prom = stack.top();
				stack.pop();
				answer.push_back(prom);
			}
			stack.push(tmp);
			continue;
		}
	}
	std::string intermediate;
	while (!stack.empty()) {
		intermediate = stack.top();
		stack.pop();
		answer.push_back(intermediate);
	}
	return answer;
}

double Calculator::Postfix_Calculate(std::vector<std::string>& str, std::map < std::string, Function>& map_of_func) {
	std::stack<double> stack;
	
	for (auto tmp : str) {
		if (tmp[0] >= '0' && tmp[0] <= '9') {
			stack.push(static_cast<double>(stoi(tmp)));
			continue;
		}
		else {
			if (map_of_func.count(tmp) == 0)
				throw "Function not found";
			if (map_of_func[tmp].GetArity() == 2) {
				double a = stack.top();
				stack.pop();
				double b = stack.top();
				stack.pop();
				fun func;
				func = map_of_func[tmp].GetFun();
				stack.push(func(std::vector<double>{b,a}));
			}
			if (map_of_func[tmp].GetArity() == 1) {
				double a = stack.top();
				stack.pop();
				fun func;
				func = map_of_func[tmp].GetFun();
				stack.push(func(std::vector<double>{a}));
			}
		}
	}
	return stack.top();
}

double Calculator::Calculate(std::string& string) {
	std::vector<std::string> dll_names = get_plugins_filenames();
	std::map < std::string, Function> map_of_func = get_map_of_functions(dll_names);
	std::vector<std::string> parsed = Parse(string);
	parsed = Postfix_Entry(parsed, map_of_func);
	return Postfix_Calculate(parsed, map_of_func);
}


