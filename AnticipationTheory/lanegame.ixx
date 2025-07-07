module;
export module lanegame;
import "pch.h";
import game;
using namespace game;

export namespace lanegame
{
	struct State
	{
		int exp1;               // Player 1 experience
		int exp2;               // Player 2 experience
		int minions_remaining;  // Total minions remaining
		auto operator <=> (const State&) const = default;
	};
	struct Transition { float probability; State to; };

	struct Game
	{
		typedef game::EmptyConfig config_t;
		typedef State state_t;

		static State initial_state() { return State{ 0, 0, 10 }; }
		static bool is_terminal_state(const State& s) { return s.minions_remaining <= 0; }
		static std::vector<Transition> get_transitions(game::EmptyConfig, const State& state)
		{
			std::vector<Transition> result;
			if (is_terminal_state(state)) return result;

			// Rock-Paper-Scissors style outcomes
			// Each turn reduces minion count by 2
			State next_state = state;
			next_state.minions_remaining -= 2;

			// [1] Win: Player 1 kills minion, Player 2 couldn't kill minion
			State win_state = next_state;
			win_state.exp1 += 1;
			result.push_back({ 1.0f / 3.0f, win_state });

			// [2] Draw: Both players kill their respective minions
			State draw_state = next_state;
			draw_state.exp1 += 1;
			draw_state.exp2 += 1;
			result.push_back({ 1.0f / 3.0f, draw_state });

			// [3] Loss: Player 1 couldn't kill minion, Player 2 kills minion
			State loss_state = next_state;
			loss_state.exp2 += 1;
			result.push_back({ 1.0f / 3.0f, loss_state });

			sanitize_transitions(result);
			return result;
		}
		static float compute_intrinsic_desire(const State& state)
		{
			// Victory condition: player with higher experience wins
			return is_terminal_state(state) && state.exp1 > state.exp2 ? 1.0f : 0.0f;
		}
		static std::string tostr(const State& s)
		{
			return "Exp1:" + std::to_string(s.exp1) +
				" Exp2:" + std::to_string(s.exp2) +
				" Minions:" + std::to_string(s.minions_remaining);
		}
	};
}

export namespace game
{
	template<typename state_t>
	void lanegame_dump_most_fun_moments(const std::vector<state_t>& states, std::map<state_t, StateNode>& stateNodes)
	{
		std::vector<state_t> states_sorted_by_a = states;
		std::sort(states_sorted_by_a.begin(), states_sorted_by_a.end(),
			[&](const state_t& a, const state_t& b)
			{
				return stateNodes[a].a > stateNodes[b].a;
			});

		printf("Most fun moments(sorted by A)\n");
		printf("P1.Exp\tP2.Exp\tMinions\tD_local\tD_global\tA\n");
		printf("------\t------\t-------\t-------\t--------\t-------\n");

		for (const auto& state : states_sorted_by_a)
		{
			auto& node = stateNodes[state];

			if (node.a > 0.0f)
				printf("%d\t%d\t%d\t%.2f\t%.2f\t\t%.2f\n",
					state.exp1, state.exp2, state.minions_remaining,
					node.d_local, node.d_global, node.a);

			// Limit output for readability
			if (&state - &states_sorted_by_a[0] >= 25) break;
		}
	}

	template<typename state_t>
	void lanegame_dump_most_fun_moments_a12345(
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
				return (stateNodes[a].a + stateNodes2[a].a + stateNodes3[a].a + stateNodes4[a].a + stateNodes5[a].a) >
					(stateNodes[b].a + stateNodes2[b].a + stateNodes3[b].a + stateNodes4[b].a + stateNodes5[b].a);
			});

		printf("Most fun moments(sorted by A1+A2+A3+A4+A5)\n");
		printf("P1.Exp\tP2.Exp\tMinions\tD_loc\tD_glob\tA1\tA2\tA3\tA4\tA5\tA_total\n");
		printf("------\t------\t-------\t-----\t------\t---\t---\t---\t---\t---\t-------\n");

		for (const auto& state : states_sorted_by_a12345)
		{
			auto& node = stateNodes[state];
			auto& node2 = stateNodes2[state];
			auto& node3 = stateNodes3[state];
			auto& node4 = stateNodes4[state];
			auto& node5 = stateNodes5[state];

			float a_total = node.a + node2.a + node3.a + node4.a + node5.a;

			if (node.a > 0.0f)
				printf("%d\t%d\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",
					state.exp1, state.exp2, state.minions_remaining,
					node.d_local, node.d_global,
					node.a, node2.a, node3.a, node4.a, node5.a,
					a_total);

			// Limit output for readability
			if (&state - &states_sorted_by_a12345[0] >= 25) break;
		}
	}
}