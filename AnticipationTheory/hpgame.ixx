module;
export module hpgame_v1;
import "pch.h";
import game;

export namespace hpgame
{
	enum class TransitionAlias : unsigned char
	{
		AttackAndMiss,
		BothAttack,
		MissAndAttack
	};
	struct State
	{
		unsigned char hp1; // player1 hp
		unsigned char hp2; // player2 hp
		auto operator <=> (const State&) const = default;
	};
	struct Transition { float probability; State to; TransitionAlias alias; };

	struct Game
	{
		typedef game::EmptyConfig config_t;
		typedef State state_t;

		static State initial_state() { return State{ 5, 5 }; }
		static bool is_terminal_state(const State& s) { return s.hp1 <= 0 || s.hp2 <= 0; }
		static std::vector<Transition> get_transitions(game::EmptyConfig, const State& state)
		{
			std::vector<Transition> result;
			if (state.hp1 == 0) return result;
			if (state.hp2 == 0) return result;

			// win draw loss version
			result.push_back({ 1.0f / 3.0f, State{ state.hp1, (unsigned char)(state.hp2 - 1) }, TransitionAlias::AttackAndMiss });
			result.push_back({ 1.0f / 3.0f, State{ (unsigned char)(state.hp1 - 1), (unsigned char)(state.hp2 - 1) }, TransitionAlias::BothAttack });
			result.push_back({ 1.0f / 3.0f, State{ (unsigned char)(state.hp1 - 1), state.hp2 }, TransitionAlias::MissAndAttack });

			// win & loss version (commented)
			//result.push_back({ 1.0f / 2.0f, State{ state.hp1, (unsigned char)(state.hp2 - 1) }, TransitionAlias::AttackAndMiss });
			//result.push_back({ 1.0f / 2.0f, State{ (unsigned char)(state.hp1 - 1), state.hp2 }, TransitionAlias::MissAndAttack });

			game::sanitize_transitions(result);
			return result;
		}
		static float compute_intrinsic_desire(const State& state)
		{
			return state.hp1 != 0 && state.hp2 == 0 ? 1.0f : 0.0f; // player1 alive && player2 dead, win condition
		}
		static std::string tostr(const State& s) { return "HP1:" + std::to_string(s.hp1) + " HP2:" + std::to_string(s.hp2); }
	};

	std::string tostr(TransitionAlias alias)
	{
		switch (alias)
		{
		case TransitionAlias::AttackAndMiss: return "AttackAndMiss";
		case TransitionAlias::BothAttack: return "BothAttack";
		case TransitionAlias::MissAndAttack: return "AttackReceived";
		}
		return "Unknown";
	}
}

export namespace game
{
	template<typename state_t> void hpgame_dump_most_fun_moments(const std::vector<state_t>& states, std::map<state_t, StateNode>& stateNodes)
	{
		std::vector<state_t> states_sorted_by_a = states;
		std::sort(states_sorted_by_a.begin(), states_sorted_by_a.end(),
			[&](const state_t& a, const state_t& b)
			{
				return stateNodes[a].a > stateNodes[b].a;
			});

		printf("Most engaging moments(sorted by A)\n");
		printf("Player1.Hp\tPlayer2.Hp\tD_local\tD_global\tA\n");
		for (const auto& state : states_sorted_by_a)
		{
			auto& node = stateNodes[state];

			if (node.a > 0.0f)
				printf("%d\t\t%d\t\t%.2f\t%.2f\t\t%.2f\n", state.hp1, state.hp2, node.d_local, node.d_global, node.a);
		}
	}

	template<typename state_t> void hpgame_dump_most_fun_moments_a12345(const std::vector<state_t>& states, std::map<state_t, StateNode>& stateNodes)
	{
		std::vector<state_t> states_sorted_by_a12345 = states;
		std::sort(states_sorted_by_a12345.begin(), states_sorted_by_a12345.end(),
			[&](const state_t& a, const state_t& b)
			{
				return stateNodes[a].sum_A() > stateNodes[b].sum_A();

			});
		printf("Player1.Hp\tPlayer2.Hp\tD_global\tA1\tA2\tA3\tA4\tA5\tSUM\n");
		for (const auto& state : states_sorted_by_a12345)
		{
			auto& node = stateNodes[state];
			if (node.sum_A() > 0.0f)
				printf("%d\t\t%d\t\t%.2f\t\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",
					state.hp1, state.hp2, node.d_global,
					node.a[0], node.a[1], node.a[2], node.a[3], node.a[4],
					node.sum_A()

				);
		}
	}

}