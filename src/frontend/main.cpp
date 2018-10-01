#include <expr.h>
#include "postfix.h"
#include "display.h"
#include <simplify.h>

int main(int argc, char ** argv) {
	std::string input;
	std::getline(std::cin, input);

	cas::expr parsed = parse(input);
	display(parsed);
	std::cout << "\n";
	cas::simplify(parsed);
	display(parsed);
	std::cout << "\n";

	while (true) {
		std::getline(std::cin, input);
		if (input == "subst") {
			std::cout << "input new expr" << std::endl;
			std::getline(std::cin, input);
			cas::expr sub = parse(input);

			cas::substitude(parsed, sub);
			cas::simplify(parsed);
			display(parsed);
			std::cout << std::endl;
		}
		else if (input == "eval") {
			std::cout << cas::eval(parsed) << std::endl;
		}
		else {
			break;
		}
	}

	return 0;
}
