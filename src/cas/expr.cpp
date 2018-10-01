#include "expr.h"
#include <algorithm>
#include <numeric>
#include <cmath>

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

	namespace {
		constexpr double PI = 3.14159265358979;
		constexpr double E  = 2.7182818;

		double eval_constant(const std::string &name) {
			if (name == "pi") return PI;
			else if (name == "ee") return E;
			else return std::numeric_limits<double>::signaling_NaN();
		}

		double eval_func(const expr& e) {
			// Not implemented TODO
			return 0;
		}
	}

	double eval(const expr& e) {
		switch (e.etype) {
			case expr_type::equal:
				return eval(e.rhs());
			case expr_type::var:
				throw std::runtime_error("Cannot evaluate unspecified variable " + e.strvalue);
			case expr_type::constant:
				return eval_constant(e.strvalue);
			case expr_type::func:
				return eval_func(e);
			case expr_type::num:
				return e.value;
			case expr_type::add:
				return std::accumulate(e.params.cbegin(), e.params.cend(), 0.0, [](const auto &a, const auto &b){return eval(a) + eval(b);});
			case expr_type::mul:
				return std::accumulate(e.params.cbegin(), e.params.cend(), 1.0, [](const auto &a, const auto &b){return eval(a) * eval(b);});
			case expr_type::pow:
				return std::pow(eval(e.lhs()), eval(e.rhs()));
			case expr_type::none:
				return 0;
			default:
				return 0;
		}
	}

	void substitude(expr &e, const expr& s) {
		if (!is_equal(s)) throw std::runtime_error("Substitude s argument must be an equality.");
		while (for_all_expr(e, [&](auto &f){
			if (f == s.lhs()) {
				f = s.rhs();
			}
		})) {;}
	}

}
