module;
export module hpgame_v1;

import "pch.h";
import game;

export namespace hpgame
{
	struct Transition;
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

		bool is_terminal() const;
		std::vector<Transition> transitions() const;

		static State initial() { return State{ 5, 5 }; }
	};
	struct Transition
	{
		float probability;
		State to;
		TransitionAlias alias;
	};

	State initial_state() { return State{ 5, 5 }; }

	bool State::is_terminal() const { return hp1 <= 0 || hp2 <= 0; }
	std::vector<Transition> State::transitions() const
	{
		std::vector<Transition> result;
		if (hp1 == 0) return result;
		if (hp2 == 0) return result;

		// win draw loss version
		result.push_back({ 1.0f / 3.0f, State{ hp1, (unsigned char)(hp2 - 1) }, TransitionAlias::AttackAndMiss });
		result.push_back({ 1.0f / 3.0f, State{ (unsigned char)(hp1 - 1), (unsigned char)(hp2 - 1) }, TransitionAlias::BothAttack });
		result.push_back({ 1.0f / 3.0f, State{ (unsigned char)(hp1 - 1), hp2 }, TransitionAlias::MissAndAttack });

		// win & loss version
		//result.push_back({ 1.0f / 2.0f, State{ hp1, (unsigned char)(hp2 - 1) }, TransitionAlias::AttackAndMiss });
		//result.push_back({ 1.0f / 2.0f, State{ (unsigned char)(hp1 - 1), hp2 }, TransitionAlias::MissAndAttack });

		// validate probabilities
		float total = 0.0f;
		for (const auto& ts : result)
			total += ts.probability;
		if (total != 1.0f)
			throw std::runtime_error("Invalid probabilities");

		// normalize probabilities
		for (auto& ts : result)
			ts.probability /= total;

		return result;
	}

	std::string tostr(TransitionAlias alias)
	{
		switch (alias)
		{
		case TransitionAlias::AttackAndMiss: return "AttackAndMiss";
		case TransitionAlias::BothAttack: return "BothAttack";
		case TransitionAlias::MissAndAttack: return "AttackReceived";
		}
		return "Unknown";
	};
}
export namespace game
{
	GameAnalysis<hpgame::State> analyze_hpgame() { return analyze<hpgame::State>([](const hpgame::State& state)
		{
			return
				state.hp1 != 0 && state.hp2 == 0 // player1 alive && player2 dead, win condition
				? 1.0f
				: 0.0f;
		});
	}

	hpgame::State run_hpgame(const std::function<size_t(const hpgame::State&)> onChoice)
	{
		return run<hpgame::State>(onChoice);
	}

	template<typename state_t> void hpgame_dump_most_fun_moments(const std::vector<state_t>& states, std::map<state_t, StateNode>& stateNodes)
	{
		std::vector<state_t> states_sorted_by_a = states;
		std::sort(states_sorted_by_a.begin(), states_sorted_by_a.end(),
			[&](const state_t& a, const state_t& b)
			{
				return stateNodes[a].a > stateNodes[b].a;
			});

		printf("Most fun moments(sorted by A)\n");
		printf("Player1.Hp\tPlayer2.Hp\tD_local\tD_global\tA\n");
		for (const auto& state : states_sorted_by_a)
		{
			auto& node = stateNodes[state];

			if (node.a > 0.0f)
				printf("%d\t\t%d\t\t%.2f\t%.2f\t\t%.2f\n", state.hp1, state.hp2, node.d_local, node.d_global, node.a);
		}
	}

	template<typename state_t> void hpgame_dump_most_fun_moments_a12345(
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
				return (stateNodes[a].a + stateNodes2[a].a + stateNodes3[a].a + stateNodes4[a].a + stateNodes5[a].a)
				 > (stateNodes[b].a + stateNodes2[b].a + stateNodes3[b].a + stateNodes4[b].a + stateNodes5[b].a);

			});
		printf("Most fun moments(sorted by A1+A2+A3+A4+A5)\n");
		printf("Player1.Hp\tPlayer2.Hp\tD_local\tD_global\tA1\tA2\tA3\tA4\tA5\tSUM\n");
		for (const auto& state : states_sorted_by_a12345)
		{
			auto& node = stateNodes[state];
			auto& node2 = stateNodes2[state];
			auto& node3 = stateNodes3[state];
			auto& node4 = stateNodes4[state];
			auto& node5 = stateNodes5[state];
			if (node.a > 0.0f)
				printf("%d\t\t%d\t\t%.2f\t%.2f\t\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",
					state.hp1, state.hp2, node.d_local, node.d_global,
					node.a, node2.a, node3.a, node4.a, node5.a,
					node.a + node2.a + node3.a + node4.a + node5.a
					
				);
		}
	}

}