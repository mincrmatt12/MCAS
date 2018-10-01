// expr.h
//
// expression definitions of the tree type used to represent math exprs
//
// yipee

#ifndef CAS_EXPR_H
#define CAS_EXPR_H

#include <list>
#include <string>
#include <memory>
#include <algorithm>

namespace cas {

#define CAS_ENUM_EXPRESSIONS(o) \
	o(add) o(mul) o(pow) o(equal) \
	o(num) o(var) o(func) o(none) o(constant)

#define o(n) n,

	enum struct expr_type {
		CAS_ENUM_EXPRESSIONS(o)
	};

#undef o

	struct expr;

	namespace detail {
		template <typename ...V>
		struct all_exprs {
			static constexpr bool v = (std::is_convertible_v<V, expr> && ...);
		};
	}

	typedef std::list<expr> expr_vec;

	struct expr {
		expr_type etype = expr_type::none;
		std::string strvalue{};
		double value{};

		expr_vec params;

		expr(expr_type &&m, expr_vec &&v) : etype(std::move(m)), params(std::move(v)) {}
		template<typename ...T, typename=std::enable_if_t<detail::all_exprs<T...>::v>>
		expr(expr_type &&m, T&&... args)  : etype(std::move(m)), params{ std::move(args)... } {};

		expr(expr_type &&m, std::string &&value) : etype(std::move(m)), strvalue(std::move(value)), params() {}
		template<typename ...T, typename=std::enable_if_t<detail::all_exprs<T...>::v>>
		expr(expr_type &&m, std::string &&value, T&&...args) : etype(std::move(m)), strvalue(std::move(value)), params{ std::move(args)... } {}
		expr(expr_type &&m, std::string &&value, expr_vec &&v) : etype(std::move(m)), strvalue(std::move(value)), params(std::move(v)) {}
		expr(const double& value) : etype(expr_type::num), value(value), params() {}

		const expr& lhs() const;
		const expr& rhs() const;
		expr&       lhs();
		expr&	    rhs();

		bool operator==(const expr& other) const;
		bool operator==(const double& other) const;
		bool operator!=(const expr& other) const {return !operator==(other);};
		bool operator!=(const double& other) const {return !operator==(other);};

		bool operator< (const expr& other) const; // note: not a cas like system, just internal ordering

		// TODO: add operator= for double conversion
	};

#define o(n) static inline bool is_##n(const expr& i) {return i.etype == expr_type::n;}

	CAS_ENUM_EXPRESSIONS(o)

#undef o

	namespace {
		template<typename Func, typename... Params>
		bool call_v(Func &&f, Params&&... parms) {
			if constexpr (std::is_void_v<decltype(f(std::forward<Params>(parms)...))>) return (f(std::forward<Params>(parms)...), false);
			else return f(std::forward<Params>(parms)...);
		}
	}

	template<typename Func>
	bool for_all_expr(expr& e, Func &&f) {
		return call_v(f, e) || std::any_of(e.params.begin(), e.params.end(), [&](auto &ee){return for_all_expr(ee, f);});
	}

	// Evaluate the expression. (simplify does this if it can be sure it will be integers, this function evaluates 
	// and can return a fractional value)
	double eval(const expr& e);
	
	// Substitude an expression into another. 
	//
	// s should be an equals with the left hand side being something to replace.
	//
	// e.g.:
	//
	// e: (x+2)
	// s: (x=2)
	// |
	// e: (2+2)
	//
	// You should probably run simplify on e after this function is done.
	void substitude(expr& e, const expr& s);
}

#endif
