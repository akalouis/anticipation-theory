module;
export module two_turn_game;

import "pch.h";
import game;

export namespace two_turn_game
{
	struct Transition;
	struct State
	{
		unsigned char turn;
		unsigned int node_idx_in_turn;

		auto operator <=> (const State&) const = default;

		bool is_terminal() const;
		std::vector<Transition> transitions() const;

		static State initial() { return State{ 0, 0 }; }
	};
	struct Transition
	{
		float probability;
		State to;
	};


	State initial_state() { return State::initial(); }

	bool State::is_terminal() const { return turn >= 2; }

	std::vector<Transition> State::transitions() const
	{
		std::vector<Transition> result;

		if (turn == 0 && node_idx_in_turn == 0) {
			// turn0_ni0 -> turn1 intermediates
			result.push_back({ 0.5f, State{ 1, 0 } });
			result.push_back({ 0.5f, State{ 1, 1 } });
		}
		else if (turn == 1) {
			if (node_idx_in_turn == 0) {
				// turn1_ni0 -> turn2 terminals (30% win, 70% loss)
				result.push_back({ 0.3f, State{ 2, 0 } });
				result.push_back({ 0.7f, State{ 2, 1 } });
			}
			else if (node_idx_in_turn == 1) {
				// turn1_ni1 -> turn2 terminals (80% win, 20% loss)  
				result.push_back({ 0.8f, State{ 2, 0 } });
				result.push_back({ 0.2f, State{ 2, 1 } });
			}
		}

		return result;
	}
}

export namespace game
{
	GameAnalysis<two_turn_game::State> analyze_two_turn_game()
	{
		return analyze<two_turn_game::State>([](const two_turn_game::State& state)
			{
				return state.turn == 2 && state.node_idx_in_turn == 0 ? 1.0f : 0.0f;
			});
	}

	template<typename state_t> void two_turn_dump_most_fun_moments_a12(
		const std::vector<state_t>& states,
		std::map<state_t, StateNode>& stateNodes,
		std::map<state_t, StateNode>& stateNodes2)
	{
		std::vector<state_t> states_sorted_by_a12 = states;
		std::sort(states_sorted_by_a12.begin(), states_sorted_by_a12.end(),
			[&](const state_t& a, const state_t& b)
			{
				return (stateNodes[a].a + stateNodes2[a].a) > (stateNodes[b].a + stateNodes2[b].a);
			});

		printf("Most fun moments(sorted by A1+A2)\n");
		printf("State\t\t\tA1\tA2\tSUM\n");
		for (const auto& state : states_sorted_by_a12)
		{
			auto& node = stateNodes[state];
			auto& node2 = stateNodes2[state];
			if (node.a > 0.0f || node2.a > 0.0f)
				printf("turn%d_ni%d\t\t%.3f\t%.3f\t%.3f\n",
					state.turn, state.node_idx_in_turn,
					node.a, node2.a, node.a + node2.a);
		}
	}
}