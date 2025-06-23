# anticipation-theory

**Compute "Objective Fun" of Game Design via the Theory of Anticipation.**

A C++17 library for quantifying player engagement by measuring “anticipation,” a mathematically measured metric for in-game fun.

---

## Overview

Theory of Anticipation (ToA) is a new theory for objectively evaluating game design & all general narrative media(engaging level of youtube video, novel, movie, ...).

Provides a **non‑subjective**, **optimizable**, and **analytically tractable** metric for player engagement (“fun”). Given any discrete game with probabilistic state transitions and defined player objectives, ToA computes how much fun each state generates.

### Inputs

1. **Game Model**

   * **States**: Game states.
   * **Transitions**: Transition probabilities $P(s \to s')$ between states.

2. **Intrinsic Desire** (`D_local`)

   Win condition. More precisely, the root source of fun. All outputs are computed from these values. 
   Use win=1, others=0 for **objective** analysis (recommended). Custom values enable research and experimentation.

### Outputs

* **Global Desire** D_global for each state
* **Anticipation** A1 for each state
* **Nested Anticipation Values** A2,A3,A4,... for each state
* **Game Design Score**: scalar average of `A1+A2+A3+A4+...` across all reachable states, weighted by reachability.

### Core Foundation

$$\mu = \sum_{s'} P(s \to s') \cdot D_{global}(s')$$
$$A_1(s) = \sqrt{\sum_{s'} P(s \to s') \cdot (D_{global}(s') - \mu)^2}$$

This is the Local Anticipation formula, the core foundation of anticipation theory. Global and nested values are just algorithmic expansion of this (dynamic programming and recursion).

Please refer to the white paper for details and derivation(located in /LaTeX folder).

---

## Features

* **Compute D_global(Global Desire)** for each state.
* **Compute Anticipation via Dynamic Programming** for each state.
* **Decompose Nested Anticipation** of anticipation (A2, A3, …).
* **Game Design Score**: non-subjective, reproducible metric for entire game.
* **Enumerate Top Fun Moments**: list states sorted by anticipation.
* **Interactive Console Simulation**: step through state transitions and simulate the game in realtime.
* **Optimizer Extension (TBD)**: integrate genetic algorithms to maximize design score.

---

## Quickstart

```cpp
```

## License

MIT License. See [LICENSE](./LICENSE) for details.

## Contact

For consulting inquiries & offers: IGFrYS5sb3VpcyBhdCBvdXRsb29rIGRvdCBjb20g (base64)