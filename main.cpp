#include "Calculator.h"
#include <Windows.h>


int main() {
	std::string str;
	std::getline(std::cin, str);
	std::vector<std::string> par;
	Calculator calc;
	double ans;
	try {
		ans = calc.Calculate(str);
	}
	catch(const char* error_message){
		std::cout << error_message << std::endl;
		return -1;
	}
	
	std::cout << ans;
	return 0;
}