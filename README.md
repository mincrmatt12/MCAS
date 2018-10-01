# MCAS
I was convinced to try to write a cas. This is the result.

## Usage

CAS library is located in `cas/`, built with cmake right now but it is pure c++17 w/ stl.
Frontend (small test setup) is in `frontend/`.

CMake is currently building the test setup if you want to play with it

## Design

A good amount of design ideas were taken from my compiler project, MLang (the similarities between simplifying expressions
and optimizing code are uncanny, but not exactly unexpected).

I hope to get this thing to be able to at least solve equations, calculus stuff will almost certainly come later.

### Simplification

Almost exactly the same as MLang; keep applying simplification methods on every expression in the tree until no modifications are made.
Ran from the `cas::simplify` method, specific techniques are in `cas::simpl`. Domain specific techniques go in `cas::simpl::<whatever>`.

### Solving

Plan is to first make sure vars are on the right side of the equation, then do a BFS for the offending variable.
Keep drilling down, removing operations on both sides, using simplify heavily.

E.G:

```
  +
 + 3  
2 x
```

Would select the right then left to reverse, causing -3 and -2 to be added to both sides. After running `cas::simplfy` this should be a simple expression.

(todo: formatting)
