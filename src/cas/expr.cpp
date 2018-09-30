#include "expr.h"
#include <algorithm>

namespace cas {
	const expr& expr::lhs() const {
		return params.front();
	}

	const expr& expr::rhs() const {
		return params.back();
	}

	expr& expr::lhs() {
		return params.front();
	}

	expr& expr::rhs() {
		return params.back();
	}

	bool expr::operator==(const expr& other) const {
		if (other.etype != this->etype) return false;
		switch (this->etype) {
			case expr_type::num:
				return this->value == other.value;
			case expr_type::constant:
			case expr_type::var:
			case expr_type::func:
				if (this->strvalue != other.strvalue) return false;
				if (!is_func(*this)) return true;

				[[fallthrough]];
			default:
				if (this->params.size() != other.params.size()) return false;

				return this->params == other.params;
		}
	}

	bool expr::operator==(const double& other) const {
		return is_num(*this) && this->value == other;
	}

	bool expr::operator<(const expr& other) const {
		auto to_tuple  = [](const expr& e){
			switch (e.etype) {
				case expr_type::num:
					return std::make_tuple(e.etype, e.value, std::string(), expr_vec());
				case expr_type::constant:
				case expr_type::var:
					return std::make_tuple(e.etype, 0.0, e.strvalue, expr_vec());
				case expr_type::func:
					return std::make_tuple(e.etype, 0.0, e.strvalue, expr_vec(e.params));
				default:
					return std::make_tuple(e.etype, 0.0, std::string(), expr_vec(e.params));

			}
		};

		return to_tuple(*this) < to_tuple(other);
	}
}
