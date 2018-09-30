#ifndef CAS_SIMPLIFY_H
#define CAS_SIMPLIFY_H

// simplification methods

#include "expr.h"

namespace cas {
	void simplify(expr& e);		

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
	}
}

#endif
