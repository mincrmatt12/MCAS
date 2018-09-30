#include "polynomial.h"
#include <algorithm>
#include <numeric>
#include <cmath>

int cas::polynomial::degree(const expr& e) {
	switch (e.etype) {
		case expr_type::constant:
		case expr_type::num:
			return 0;
		case expr_type::var:
			return 1;
		case expr_type::mul:
			{
				int deg = degree(e.params.front());
				for (auto i = ++e.params.cbegin(); i != e.params.cend(); ++i) {
					deg *= degree(*i);
				}
				return deg;
			}
		case expr_type::pow:
			{
				if (!is_num(e.rhs())) {
					return degree(e.lhs());
				} 
				else if (std::trunc(e.rhs().value) != e.rhs().value) {
					return degree(e.lhs());
				}
				else {
					return degree(e.lhs()) * std::max(1, static_cast<int>(e.rhs().value));
				}
			}
		case expr_type::add:
			return degree(*std::max_element(e.params.cbegin(), e.params.cend(), [](const auto &a, const auto &b){return degree(a) < degree(b);}));
		case expr_type::none:
			return 0;
		case expr_type::func:
			return 1;
		default:
			return 1;
	}
}
