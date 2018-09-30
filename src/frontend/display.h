#ifndef DISPLAY_H
#define DISPLAY_H

#include <expr.h>
#include <iostream>

static void display(const cas::expr &e) {
	switch (e.etype) {
		case cas::expr_type::add:
			std::cout << "(";
			for (const auto &f : e.params) {
				display(f);
				if (f != e.rhs()) {
					std::cout << " + ";
				}
			}
			std::cout << ")";
			break;
		case cas::expr_type::mul:
			if (cas::is_num(e.lhs())) {
				if (e.lhs().value != -1) {
					display(e.lhs());
				}
				else std::cout << "-";
				std::cout << "(";
				for (auto f = ++e.params.cbegin(); f != e.params.cend(); ++f) {
					display(*f);
					if (*f != e.rhs()) std::cout << " * ";
				}
				std::cout << ")";
			}
			else {
				std::cout << "(";
				for (const auto &f : e.params) {
					display(f);
					if (f != e.rhs()) std::cout << " * ";
				}
				std::cout << ")";
			}
			break;
		case cas::expr_type::pow:
			std::cout << "(";
			display(e.lhs());
			std::cout << ")^";
			display(e.rhs());
			break;
		case cas::expr_type::num:
			std::cout << e.value;
			break;
		case cas::expr_type::constant:
		case cas::expr_type::var:
		case cas::expr_type::func:
			std::cout << e.strvalue;
			if (!cas::is_func(e)) break;
			std::cout << "(";
			for (auto f = ++e.params.cbegin(); f != e.params.cend(); ++f) {
				if (*f != e.rhs()) std::cout << ", ";
				display(*f);
			}
			std::cout << ")";
			break;
		default:
			std::cout << "<unk>";
			break;
	}
}

#endif
