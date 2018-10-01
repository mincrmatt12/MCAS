#include "postfix.h"
#include <tuple>
#include <iostream>

std::tuple<cas::expr, std::size_t> parse_internal(std::string s) {
	switch (s[0]) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
number:
			{
				std::size_t size = s.find_first_of(' ');
				if (size == std::string::npos) size = s.size();
				return {cas::expr(std::stod(s)), size};
			}
		case '+':
			if (s[1] != ' ') goto number;
			{
				std::size_t cursor = 2;
				auto first = parse_internal(s.substr(cursor));
				cursor += std::get<1>(first) + 1;
				auto second = parse_internal(s.substr(cursor));
				cursor += std::get<1>(second);
				return {cas::op::add(std::get<0>(first), std::get<0>(second)), cursor};
			}
		case '-':
			if (s[1] != ' ') goto number;
			{
				std::size_t cursor = 2;
				auto first = parse_internal(s.substr(cursor));
				cursor += std::get<1>(first) + 1;
				auto second = parse_internal(s.substr(cursor));
				cursor += std::get<1>(second);
				return {cas::op::subtract(std::get<0>(first), std::get<0>(second)), cursor};
			}
		case '*':
			{
				std::size_t cursor = 2;
				auto first = parse_internal(s.substr(cursor));
				cursor += std::get<1>(first) + 1;
				auto second = parse_internal(s.substr(cursor));
				cursor += std::get<1>(second);
				return {cas::op::multiply(std::get<0>(first), std::get<0>(second)), cursor};
			}
		case '/':
			{
				std::size_t cursor = 2;
				auto first = parse_internal(s.substr(cursor));
				cursor += std::get<1>(first) + 1;
				auto second = parse_internal(s.substr(cursor));
				cursor += std::get<1>(second);
				return {cas::op::divide(std::get<0>(first), std::get<0>(second)), cursor};
			}
		case '^':
			{
				std::size_t cursor = 2;
				auto first = parse_internal(s.substr(cursor));
				cursor += std::get<1>(first) + 1;
				auto second = parse_internal(s.substr(cursor));
				cursor += std::get<1>(second);
				return {cas::op::raiseto(std::get<0>(first), std::get<0>(second)), cursor};
			}
		case '=':
			{
				std::size_t cursor = 2;
				auto first = parse_internal(s.substr(cursor));
				cursor += std::get<1>(first) + 1;
				auto second = parse_internal(s.substr(cursor));
				cursor += std::get<1>(second);
				return {cas::expr(cas::expr_type::equal, std::get<0>(first), std::get<0>(second)), cursor};
			}
		default:
			{
				// todo: functions
				std::size_t x = s.find_first_of(' ');
				std::string text = s.substr(0, x);
				std::size_t s = text.size();

				if (text == "pi" || text == "ee") {
					return {cas::expr(cas::expr_type::constant, std::move(text)), s};
				}
				else {
					return {cas::expr(cas::expr_type::var, std::move(text)), s};
				}
			}

	}
}

cas::expr parse(std::string &s) {
	return std::get<0>(parse_internal(s));
}
