#ifndef CAS_SIMPLIFY_H
#define CAS_SIMPLIFY_H

// simplification methods

#include "expr.h"

namespace cas {
	void simplify(expr& e);

	namespace {
		template <typename ...V>
		struct all_exprs_callable {
			static constexpr bool v = (std::is_invocable_v<V, expr &> && ...);
		};

		template<typename ...F>
		bool call_f(expr &e, F&&... f) {
			return (f(e) || ...);	
		}
	}

	template<typename ...T>
	auto simplify_with_techniques(expr& f, T&&... funcs) -> decltype((void)all_exprs_callable<T...>::v, void()) {
		while (for_all_expr(f, [&](auto &e){return call_f(e, funcs...);})) {;}
	};

	namespace simpl {
		// Collect things that are distributive, in the form of ba+ca = (b+c)a
		//
		// Find all muls in an add.
		int collect_distributive(expr& e);

		// Collect constants in add/mul/pow
		// (numbers)
		int collect_constants(expr &e);

		// Flatten trees (add+(add+add)) --> (add+add+add)
		// Not pow as it is always 2param
		int flatten(expr &e);

		// Reorganize multiplies (x*y*2) --> (2*x*y)
		//
		// numbers first, then constants, then vars in alph order, then whatever else
		int reorganize_constants(expr &e);

		// Coalesce operands
		//
		// Like distributive, but for singular expressions
		int coalesce_operands(expr &e);

		// Identities
		//
		// Like (anything)^1 = (anything)
		// or ^0 = 1
		//
		// Or 0*asdf = whatever
		//
		// Might add function identities here as well, or mightplace them in 
		// different functions.
		int sub_identities(expr &e);
	}
}

#endif
