#include "Calculator.h"
#include <Windows.h>


int main() {
	std::string str;
	std::getline(std::cin, str);
	std::vector<std::string> par;
	Calculator calc;
	double ans;
	while (str != "") {
		try {
			ans = calc.Calculate(str);
		}
		catch (const char* error_message) {
			std::cout << error_message << std::endl;
			//return -1;
			std::getline(std::cin, str);
			continue;
		}
		
		std::cout << ans << std::endl;
		std::getline(std::cin, str);
		
	}
	return 0;
}