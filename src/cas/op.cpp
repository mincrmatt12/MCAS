#include "op.h"

namespace cas::op {

	expr add(const expr& l, const expr& r) {
		return expr(expr_type::add, expr(l), expr(r));
	}

	expr subtract(const expr& l, const expr& r) {
		return expr(expr_type::add, expr(l), std::move(expr(expr_type::mul, -1.0, expr(r))));
	}

	expr multiply(const expr& l, const expr& r) {
		return expr(expr_type::mul, expr(l), expr(r));
	}

	expr divide(const expr& l, const expr& r) {
		return expr(expr_type::mul, expr(l), std::move(expr(expr_type::pow, expr(r), -1.0)));
	}

	expr raiseto(const expr& l, const expr& r) {
		return expr(expr_type::pow, expr(l), expr(r));
	}

}
