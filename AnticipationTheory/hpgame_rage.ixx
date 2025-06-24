module;
export module hpgame_rage;
import "pch.h";
import game;

export namespace hpgame_rage
{
	// Game parameters structure to hold configurable values
	struct GameParams
	{
		float critical_chance = 0.10f;
		float hit_chance = 0.45f;
		float miss_chance = 0.45f;

		// rage mechanics
		bool rage_spendable = false;       // Whether rage is spent on critical hits
		int rage_dmg_multiplier = 1;       // Multiplier for rage-enhanced damage

		bool rage_increase_on_attack_dmg = true;  // Whether dealing damage increases rage
		bool rage_increase_on_received_dmg = true; // Whether receiving damage increases rage


		// Constructor that ensures probabilities sum to 1.0
		GameParams(float critical = 0.10f)
		{
			critical_chance = critical;
			hit_chance = miss_chance = (1.0f - critical_chance) / 2.0f;

			// Validate probabilities
			if (!is_valid())
				throw std::runtime_error("Invalid probabilities: critical_chance + hit_chance + miss_chance must equal 1.0");
		}

		// Validate probabilities
		bool is_valid() const {
			return std::abs((critical_chance + hit_chance + miss_chance) - 1.0f) < 0.001f;
		}
	};


	struct Transition;
	enum class TransitionAlias : unsigned char
	{
		AttackAndMiss,
		CriticalAndMiss,
		MissAndAttack,
		MissAndCritical,
		AttackAndAttack,
		AttackAndCritical,
		CriticalAndAttack,
		CriticalAndCritical
	};

	struct State
	{
		unsigned char hp1; // player1 hp
		unsigned char hp2; // player2 hp
		unsigned char rage1; // player1 rage
		unsigned char rage2; // player2 rage

		static GameParams params; // Game parameters

		auto operator <=> (const State&) const = default;

		bool is_terminal() const;
		std::vector<Transition> transitions() const;
		static State initial() { return State{ 5, 5, 0, 0 }; }
	};

	GameParams State::params = GameParams(); // Initialize static member

	struct Transition
	{
		float probability;
		State to;
		TransitionAlias alias;
	};

	State initial_state() { return State{ 5, 5, 0, 0 }; }

	bool State::is_terminal() const { return hp1 <= 0 || hp2 <= 0; }




	std::vector<Transition> State::transitions() const
	{
		std::vector<Transition> result;
		if (hp1 <= 0) return result;
		if (hp2 <= 0) return result;

		// Get probabilities from the static parameters
		const float critical_chance = params.critical_chance;
		const float hit_chance = params.hit_chance;
		const float miss_chance = params.miss_chance;


		// Helper function to calculate rage after an action
		auto calculate_next_rage = [&](unsigned char current_rage, bool dealt_damage, bool received_damage) -> unsigned char
			{
				unsigned char next_rage = current_rage;

				// Increase rage when dealing damage if enabled
				if (dealt_damage && params.rage_increase_on_attack_dmg) {
					next_rage++;
				}

				// Increase rage when receiving damage if enabled
				if (received_damage && params.rage_increase_on_received_dmg) {
					next_rage++;
				}

				return next_rage;
			};

		// Player 1 attacks, Player 2 misses
		result.push_back({
			hit_chance * miss_chance,
			State{
				hp1,
				static_cast<unsigned char>(std::max(0, hp2 - 1)),
				calculate_next_rage(rage1, true, false),  // P1 dealt damage
				calculate_next_rage(rage2, false, true)   // P2 received damage
			},
			TransitionAlias::AttackAndMiss
			});

		// Player 1 critical hits, Player 2 misses
		result.push_back({
			critical_chance * miss_chance,
			State{
				hp1,
				static_cast<unsigned char>(std::max(0, hp2 - (1 + rage1 * params.rage_dmg_multiplier))),
				// If rage_spendable is true, reset rage1 to 0, otherwise calculate new rage
				static_cast<unsigned char>(params.rage_spendable ? 0 : calculate_next_rage(rage1, true, false)),
				calculate_next_rage(rage2, false, true)   // P2 received damage
			},
			TransitionAlias::CriticalAndMiss
			});

		// Player 1 misses, Player 2 attacks
		result.push_back({
			miss_chance * hit_chance,
			State{
				static_cast<unsigned char>(std::max(0, hp1 - 1)),
				hp2,
				calculate_next_rage(rage1, false, true),  // P1 received damage
				calculate_next_rage(rage2, true, false)   // P2 dealt damage
			},
			TransitionAlias::MissAndAttack
			});

		// Player 1 misses, Player 2 critical hits
		result.push_back({
			miss_chance * critical_chance,
			State{
				static_cast<unsigned char>(std::max(0, hp1 - (1 + rage2 * params.rage_dmg_multiplier))),
				hp2,
				calculate_next_rage(rage1, false, true),  // P1 received damage
				// If rage_spendable is true, reset rage2 to 0, otherwise calculate new rage
				static_cast<unsigned char>(params.rage_spendable ? 0 : calculate_next_rage(rage2, true, false))
			},
			TransitionAlias::MissAndCritical
			});

		// Both players attack
		result.push_back({
			hit_chance * hit_chance,
			State{
				static_cast<unsigned char>(std::max(0, hp1 - 1)),
				static_cast<unsigned char>(std::max(0, hp2 - 1)),
				calculate_next_rage(rage1, true, true),   // P1 dealt and received damage
				calculate_next_rage(rage2, true, true)    // P2 dealt and received damage
			},
			TransitionAlias::AttackAndAttack
			});

		// Player 1 attacks, Player 2 critical hits
		result.push_back({
			hit_chance * critical_chance,
			State{
				static_cast<unsigned char>(std::max(0, hp1 - (1 + rage2 * params.rage_dmg_multiplier))),
				static_cast<unsigned char>(std::max(0, hp2 - 1)),
				calculate_next_rage(rage1, true, true),   // P1 dealt and received damage
				// If rage_spendable is true, reset rage2 to 0, otherwise calculate new rage
				static_cast<unsigned char>(params.rage_spendable ? 0 : calculate_next_rage(rage2, true, false))
			},
			TransitionAlias::AttackAndCritical
			});

		// Player 1 critical hits, Player 2 attacks
		result.push_back({
			critical_chance * hit_chance,
			State{
				static_cast<unsigned char>(std::max(0, hp1 - 1)),
				static_cast<unsigned char>(std::max(0, hp2 - (1 + rage1 * params.rage_dmg_multiplier))),
				// If rage_spendable is true, reset rage1 to 0, otherwise calculate new rage
				static_cast<unsigned char>(params.rage_spendable ? 0 : calculate_next_rage(rage1, true, false)),
				calculate_next_rage(rage2, true, true)    // P2 dealt and received damage
			},
			TransitionAlias::CriticalAndAttack
			});

		// Both players critical hit
		result.push_back({
			critical_chance * critical_chance,
			State{
				static_cast<unsigned char>(std::max(0, hp1 - (1 + rage2 * params.rage_dmg_multiplier))),
				static_cast<unsigned char>(std::max(0, hp2 - (1 + rage1 * params.rage_dmg_multiplier))),
				// If rage_spendable is true, reset rage1 to 0, otherwise calculate new rage
				static_cast<unsigned char>(params.rage_spendable ? 0 : calculate_next_rage(rage1, true, false)),
				// If rage_spendable is true, reset rage2 to 0, otherwise calculate new rage
				static_cast<unsigned char>(params.rage_spendable ? 0 : calculate_next_rage(rage2, true, false))
			},
			TransitionAlias::CriticalAndCritical
			});

		// Validate probabilities
		float total = 0.0f;
		for (const auto& ts : result)
			total += ts.probability;

		// Normalize probabilities (this handles any minor floating point precision issues)
		for (auto& ts : result)
			ts.probability /= total;

		return result;
	}



	//std::vector<Transition> State::transitions() const
	//{
	//	std::vector<Transition> result;
	//	if (hp1 <= 0) return result;
	//	if (hp2 <= 0) return result;

	//	// Given
	//	// 45% chance to hit
	//	// 10% chance to critical
	//	// 45% chance to miss

	//	// Using the Rage__DoubleHit__IncreasingRage variation from the original code

	//	// One player hits, one misses
	//	result.push_back({ 0.45f * 0.45f, State{hp1, static_cast<unsigned char>(std::max(0, hp2 - 1)),
	//		static_cast<unsigned char>(rage1 + 1), static_cast<unsigned char>(rage2 + 1)},
	//		TransitionAlias::AttackAndMiss });  // P1 Attack & P2 Miss

	//	result.push_back({ 0.10f * 0.45f, State{hp1, static_cast<unsigned char>(std::max(0, hp2 - (1 + rage1))),
	//		static_cast<unsigned char>(rage1), static_cast<unsigned char>(rage2 + (1 + rage1))},
	//		TransitionAlias::CriticalAndMiss });  // P1 Critical & P2 Miss

	//	result.push_back({ 0.45f * 0.45f, State{static_cast<unsigned char>(std::max(0, hp1 - 1)), hp2,
	//		static_cast<unsigned char>(rage1 + 1), static_cast<unsigned char>(rage2 + 1)},
	//		TransitionAlias::MissAndAttack });  // P1 Miss & P2 Attack

	//	result.push_back({ 0.45f * 0.10f, State{static_cast<unsigned char>(std::max(0, hp1 - (1 + rage2))), hp2,
	//		static_cast<unsigned char>(rage1 + (1 + rage2)), static_cast<unsigned char>(rage2)},
	//		TransitionAlias::MissAndCritical });  // P1 Miss & P2 Critical

	//	// Both players hit
	//	result.push_back({ 0.45f * 0.45f, State{static_cast<unsigned char>(std::max(0, hp1 - 1)),
	//		static_cast<unsigned char>(std::max(0, hp2 - 1)),
	//		static_cast<unsigned char>(rage1 + 1), static_cast<unsigned char>(rage2 + 1)},
	//		TransitionAlias::AttackAndAttack });  // Both Attack

	//	result.push_back({ 0.45f * 0.10f, State{static_cast<unsigned char>(std::max(0, hp1 - (1 + rage2))),
	//		static_cast<unsigned char>(std::max(0, hp2 - 1)),
	//		static_cast<unsigned char>(rage1 + (1 + rage2)), static_cast<unsigned char>(rage2 + 1)},
	//		TransitionAlias::AttackAndCritical });  // P1 Attack & P2 Critical

	//	result.push_back({ 0.10f * 0.45f, State{static_cast<unsigned char>(std::max(0, hp1 - 1)),
	//		static_cast<unsigned char>(std::max(0, hp2 - (1 + rage1))),
	//		static_cast<unsigned char>(rage1 + 1), static_cast<unsigned char>(rage2 + rage1 + 1)},
	//		TransitionAlias::CriticalAndAttack });  // P1 Critical & P2 Attack

	//	result.push_back({ 0.10f * 0.10f, State{static_cast<unsigned char>(std::max(0, hp1 - (1 + rage2))),
	//		static_cast<unsigned char>(std::max(0, hp2 - (1 + rage1))),
	//		static_cast<unsigned char>(rage1 + 1 + rage2), static_cast<unsigned char>(rage2 + 1 + rage1)},
	//		TransitionAlias::CriticalAndCritical });  // Both Critical

	//	// validate probabilities
	//	float total = 0.0f;
	//	for (const auto& ts : result)
	//		total += ts.probability;

	//	// normalize probabilities
	//	for (auto& ts : result)
	//		ts.probability /= total;

	//	return result;
	//}

	std::string tostr(TransitionAlias alias)
	{
		switch (alias)
		{
		case TransitionAlias::AttackAndMiss: return "AttackAndMiss";
		case TransitionAlias::CriticalAndMiss: return "CriticalAndMiss";
		case TransitionAlias::MissAndAttack: return "MissAndAttack";
		case TransitionAlias::MissAndCritical: return "MissAndCritical";
		case TransitionAlias::AttackAndAttack: return "AttackAndAttack";
		case TransitionAlias::AttackAndCritical: return "AttackAndCritical";
		case TransitionAlias::CriticalAndAttack: return "CriticalAndAttack";
		case TransitionAlias::CriticalAndCritical: return "CriticalAndCritical";
		}
		return "Unknown";
	};
}



export namespace game
{
	GameAnalysis<hpgame_rage::State> analyze_hpgame_rage()
	{
		return analyze<hpgame_rage::State>([](const hpgame_rage::State& state)
			{
				return
					state.hp1 != 0 && state.hp2 == 0 // player1 alive && player2 dead, win condition
					? 1.0f
					: 0.0f;
			});
	}

	hpgame_rage::State run_hpgame_rage(const std::function<size_t(const hpgame_rage::State&)> onChoice)
	{
		return run<hpgame_rage::State>(onChoice);
	}


	template<typename state_t> void hpgame_rage_dump_most_fun_moments(const std::vector<state_t>& states, std::map<state_t, StateNode>& stateNodes)
	{
		std::vector<state_t> states_sorted_by_a = states;
		std::sort(states_sorted_by_a.begin(), states_sorted_by_a.end(),
			[&](const state_t& a, const state_t& b)
			{
				return stateNodes[a].a > stateNodes[b].a;
			});

		printf("Most fun moments(sorted by A)\n");
		printf("Player1.Hp\tPlayer2.Hp\tRage1\tRage2\tD_local\tD_global\tA\n");
		for (const auto& state : states_sorted_by_a)
		{
			auto& node = stateNodes[state];

			if (node.a > 0.0f)
				printf("%d\t\t%d\t\t%d\t%d\t%.2f\t%.2f\t\t%.2f\n",
					state.hp1, state.hp2, state.rage1, state.rage2,
					node.d_local, node.d_global, node.a);
		}
	}

	template<typename state_t> void hpgame_rage_dump_most_fun_moments_a12345(
		const std::vector<state_t>& states,
		std::map<state_t, StateNode>& stateNodes,
		std::map<state_t, StateNode>& stateNodes2,
		std::map<state_t, StateNode>& stateNodes3,
		std::map<state_t, StateNode>& stateNodes4,
		std::map<state_t, StateNode>& stateNodes5)
	{
		std::vector<state_t> states_sorted_by_a12345 = states;
		std::sort(states_sorted_by_a12345.begin(), states_sorted_by_a12345.end(),
			[&](const state_t& a, const state_t& b)
			{
				//return stateNodes[a].a > stateNodes[b].a;

				//return (stateNodes[a].a + stateNodes2[a].a + stateNodes3[a].a) > (stateNodes[b].a + stateNodes2[b].a + stateNodes3[b].a);
				return (stateNodes[a].a + stateNodes2[a].a + stateNodes3[a].a + stateNodes4[a].a + stateNodes5[a].a) >
					(stateNodes[b].a + stateNodes2[b].a + stateNodes3[b].a + stateNodes4[b].a + stateNodes5[b].a);
			});

		printf("Most fun moments(sorted by A1+A2+A3+A4+A5)\n");
		printf("Player1.Hp\tPlayer2.Hp\tRage1\tRage2\tD_local\tD_global\tA1\tA2\tA3\tA4\tA5\tSUM\n");
		for (const auto& state : states_sorted_by_a12345)
		{
			auto& node = stateNodes[state];
			auto& node2 = stateNodes2[state];
			auto& node3 = stateNodes3[state];
			auto& node4 = stateNodes4[state];
			auto& node5 = stateNodes5[state];

			if (node.a > 0.0f)
				printf("%d\t\t%d\t\t%d\t%d\t%.2f\t%.2f\t\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",
					state.hp1, state.hp2, state.rage1, state.rage2,
					node.d_local, node.d_global,
					node.a, node2.a, node3.a, node4.a, node5.a,
					node.a + node2.a + node3.a + node4.a + node5.a
					);
		}
	}

	// Function to find optimal critical hit chance
	void hpgame_rage_find_optimal_critical_chance(float min_critical = 0.0f, float max_critical = 0.40f, float step = 0.05f)
	{
		printf("Finding optimal critical hit chance...\n");
		printf("Critical\tGame Design Score\n");

		float best_critical = min_critical;
		double best_score = 0.0;

		for (float critical = min_critical; critical <= max_critical; critical += step) {
			// Set the critical chance for this run
			hpgame_rage::State::params = hpgame_rage::GameParams(critical);

			// Run analysis
			auto analysis = analyze_hpgame_rage();
			auto analysis2 = analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) { return analysis.stateNodes[s].a; });
			auto analysis3 = analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) { return analysis2.stateNodes[s].a; });
			auto analysis4 = analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) { return analysis3.stateNodes[s].a; });
			auto analysis5 = analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) { return analysis4.stateNodes[s].a; });

			double score = analysis.game_design_score +
				analysis2.game_design_score +
				analysis3.game_design_score +
				analysis4.game_design_score +
				analysis5.game_design_score;

			printf("%.2f\t\t%.6f\n", critical, score);

			if (score > best_score) {
				best_score = score;
				best_critical = critical;
			}
		}

		printf("\nOptimal critical hit chance: %.2f with score: %.6f\n",
			best_critical, best_score);
	}
}
