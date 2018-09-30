// op.h
//
// Operators on expressions (not cas simplify/solve/etc though)

#ifndef CAS_OP_H
#define CAS_OP_H

#include "expr.h"

namespace cas::op {

	expr add(const expr& l, const expr& r);
	expr subtract(const expr& l, const expr& r);
	expr multiply(const expr& l, const expr& r);
	expr divide(const expr& l, const expr& r);
	expr raiseto(const expr& l, const expr& r);
}

#endif
