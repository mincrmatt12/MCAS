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
	return 0;
}
