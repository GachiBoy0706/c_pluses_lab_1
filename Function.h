#pragma once
#include <string>
#include <vector>

using fun = double (*)(std::vector<double>);

class Function {
private:
	std::string name;
	std::vector<int> operands;
	int arity;
	fun func;
	int priority;
public:
	Function() = default;
	Function(std::string name, int arity, fun func, int priority)
		: name(name), arity(arity), func(func), priority(priority) {};
	int GetArity() { return arity; };
	fun GetFun() { return func; };
	std::string GetName() { return name; };
	int GetPriority() { return priority; };
};