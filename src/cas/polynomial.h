// polynomial.h
//
// polynomial tools
//
// degree(expr) --> degree of polynomial

#ifndef CAS_POLYNOMIAL_H
#define CAS_POLYNOMIAL_H

#include "expr.h"

namespace cas::polynomial {
	/// Get the degree of the expression.
	int degree(const expr& e);
}

#endif
