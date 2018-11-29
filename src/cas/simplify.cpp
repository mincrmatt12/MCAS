#include "simplify.h"
#include "op.h"
#include "polynomial.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <numeric>
#include <cmath>

namespace cas {
	void simplify(expr &f) {
		simplify_with_techniques(f, simpl::collect_constants, simpl::flatten, simpl::reorganize_constants);
		simplify_with_techniques(f, simpl::collect_constants,  simpl::collect_distributive, simpl::flatten, simpl::reorganize_constants, 
					    simpl::coalesce_operands,  simpl::sub_identities);
	}

	namespace simpl {
		int flatten(expr &e) {
			// Only runs on add/mul
			if (!(is_mul(e) || is_add(e))) return 0;

			int modifications = 0;

			// Check if there is only param
			if (e.params.size() == 1) {
				// Replace e with its child.
				e = expr(e.params.front());
				return 1;
			}

			// Go through the children of this tree and
			// adopt all of the same type, adding them.
			//
			// Valid as std::list's iterators are not invalidated by appends
			// as it is a linked list.
			for (auto i = e.params.begin(); i != e.params.end();) {
				if (i->etype == e.etype) {
					e.params.splice(i, i->params);
					i = e.params.erase(i);
					++modifications;
				}
				else {
					++i;
				}
			}
			if (modifications > 0) std::cout << "flattened " << modifications << std::endl;

			return modifications;
		}

		int collect_constants(expr &e) {
			// Only runs on add/mul/pow
			if (is_pow(e)) {
				// Powers are evaluated if and only if:
				// - the operand is a num
				// - the power is a num
				// - result is an integer
				if (is_num(e.lhs()) && is_num(e.rhs())) {
					double result = std::pow(e.lhs().value, e.rhs().value);
					if (std::trunc(result) == result) { // Is the result an integer (truncate == result)
						e = result;
						return 1;
					}
				}
				return 0;
			}
			if (!(is_mul(e) || is_add(e))) return 0;

			int modifications = 0;
			double constant_value = is_mul(e) ? 1 : 0;
			auto marker = e.params.begin();
			
			if ((modifications += std::count_if(e.params.begin(), e.params.end(), [&](auto &f){
				if (is_num(f)) {
					if (is_add(e)) constant_value += f.value;
					if (is_mul(e)) constant_value *= f.value;
					return true;
				}
				return false;
			})) > 0) {
				// Remove all constants and add constant_value
				for (auto i = e.params.begin(); i != e.params.end();) {
					if (is_num(*i)) {
						i = e.params.erase(i);
						marker = i;
					}
					else {
						++i;
					}
				}
			}

			// Is constant_value equal to 1 (or 0 for add)?
			// (accounts for rounding error)
			//
			// Also check if there are more things, making an empty add/mul is a bad idea as the adopter will
			// fix that
			if (modifications > 0) std::cout << "folded " << modifications << " constants." << std::endl;
			if (e.params.size() >= 1 && std::abs(constant_value - (is_mul(e) ? 1 : 0)) < std::numeric_limits<double>::epsilon()) {
				return modifications; // Do not re-add the constant, as it a no-op
			}
			else {
				if (modifications == 1) modifications = 0; // Avoid infinite loop, but still allow above condition to fire
				e.params.insert(marker, constant_value);
				return modifications;
			}
		}

		int collect_distributive(expr &e) {
			// Only runs on 100% unleaded addfuel(tm)
			if (!is_add(e)) return 0;

			// Find largest set of non-dependent chains of vars
			//
			// E.g.
			//
			// ac + ad + bc + bd
			// =
			// <1> + <2> + <3> + <4>
			// a: <1>, <2>
			// b: <3>, <4>
			// c: <1>, <3>
			// d: <2>, <4>
			//
			// Greedy algorithm picks longest and first, so get a
			//
			// c: <3>
			// d: <4>
			//
			// longest is now b, no other chains remain use them.
			
			std::map<expr, std::list<expr_vec::iterator>> candidates{};
			
			// First pass: create above structure.
			int modifications = 0;
			
			{
			std::set<expr> unique_check{};

			for (auto i = e.params.begin(); i != e.params.end(); ++i) {
				if (is_mul(*i) && i->params.size() == 1) continue;
				if (!is_mul(*i)) {
					if (!is_num(*i)) candidates[*i].push_back(i);
					continue;
				}
				// Make sure the multiplicands are unique, otherwise this creates problems with the algorithm.
				// These will be taken care of in another function and converted to powers, which are fine in this system
				unique_check.clear();
				for (auto &f : i->params) {
					if (!unique_check.insert(f).second) {
						goto bad;
					}
				}

				// Categorize based on all multiplicands (assume flattened tree structure by now hopefully)
				for (auto &f : i->params) {
					if (!is_num(f)) candidates[f].push_back(i);
				}
bad:
				continue;
			}
			}

			// Now implement the greedy HUNGRY OMNIVOUROUS A NI MA L T H A T E  A  T  S
			// to actually do the stuff.
			
			while (!candidates.empty()) {
				auto i = std::max_element(candidates.begin(), candidates.end(), [&](auto &a, auto &b) {
									return a.second.size() < b.second.size();
				});

				if (i->second.size() == 1) {
					break; // If largest size == 1, nothing else can be done
				}

				// Now, remove all other entries corresponding to the same entry.
				for (auto &[k, v] : candidates) {
					if (k == i->first) continue;
					for (auto j = v.begin(); j != v.end(); ) {
						if (std::find(i->second.begin(), i->second.end(), *j) == i->second.end()) ++j;
						else j = v.erase(j);
					}
				}

				// Garbage collect entries, removing them if their lists are empty
				for (auto j = candidates.begin(); j != candidates.end(); ) {
					if (j->first == i->first) {
						++j;
						continue;
					}
					if (j->second.empty()) {
						j = candidates.erase(j);
					}
					else ++j;
				}	

				// Now, create a new expression.
				expr adder(expr_type::add);

				// Go through each position in i.second, remove the reference to expr 
				for (auto j : i->second) {
					if (!is_mul(*j)) {
						adder.params.push_back(1.0);
						e.params.erase(j);
						continue;
					}
					for (auto k = j->params.begin(); k != j->params.end();) {
						if (*k == i->first) k = j->params.erase(k);
						else ++k;
					}
					// Add j to adder
					adder.params.push_back(expr(*j));
					e.params.erase(j);
				}
				e.params.push_back(op::multiply(expr(adder), expr(i->first)));
				++modifications;
				candidates.erase(i->first);
			}
			if (modifications > 0) std::cout << "distributed " << modifications << " terms." << std::endl;
			return modifications;
		}

		int reorganize_constants(expr &e) {
			// todo: reorg by degree (also impl degree( function in polynomials
			if (!(is_mul(e) || is_add(e))) return 0;
			auto compare = [](const expr &a, const expr &b){
				return std::make_tuple(polynomial::degree(a), a.strvalue) < std::make_tuple(polynomial::degree(b), b.strvalue);
			};

			if (std::is_sorted(e.params.begin(), e.params.end(), compare)) return 0;
			e.params.sort(compare);
			std::cout << "reorganized" << std::endl;
			return 1;
		}

		int coalesce_operands(expr &e) {
			if (!(is_mul(e))) return 0;

			// Coalesce operands, e.g.:
			//
			// x * x = x^2
			
			std::map<expr, double> values{};
			std::map<expr, std::size_t> counts{};
			int modifications = 0;
			for (auto &i : e.params) {
				if (is_pow(i) && is_num(i.rhs())) values[i] += i.rhs().value;
				else ++values[i];
				++counts[i];
			}

			while (!counts.empty()) {
				auto i = std::max_element(counts.begin(), counts.end(), [](auto &a, auto &b){return a.second < b.second;});

				if (i->second == 1) break;

				++modifications;
				e.params.remove(i->first); // Why didn't I check the docs for this first? lol
				e.params.push_back(op::raiseto (i->first, expr(values[i->first])));

				counts.erase(i);
			}
			std::cout << "coalesced " << modifications << std::endl;

			return modifications;
		}

		int sub_identities(expr &e) {
			int modifications = 0;
			switch (e.etype) {
				case expr_type::pow:
					if (is_pow(e.lhs())) {
						e = op::raiseto(e.lhs().lhs(), op::multiply(e.lhs().rhs(), e.rhs()));
						++modifications;
					}
					if (is_num(e.rhs())) {
						if (e.rhs().value == 0.0) { e = 1.0; ++modifications; }
						if (e.rhs().value == 1.0) { e = e.lhs(); ++ modifications; }
					}
					break;
				case expr_type::mul:
					if (is_num(e.lhs())) {
						if (e.lhs().value == 0.0) { e = 0.0; ++modifications; }
					}
					break;
				default:
					break;
			}
			std::cout << "substituded identities * " << modifications << std::endl;
			return modifications;
		}
	}
}
