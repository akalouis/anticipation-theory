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