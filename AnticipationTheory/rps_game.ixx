module;
export module rps_game;

import "pch.h";
import game;

using namespace game;

export namespace rps_game
{
	enum class Code : unsigned char
	{
		Initial,
		Win,
		Draw,
		Loss
	};
	struct State
	{
		Code value;
		auto operator <=> (const State&) const = default;
	};
	struct Transition { float probability; State to; };

	struct Game
	{
		typedef game::EmptyConfig Config;
		typedef State State;

		static State initial_state() { return State{ Code::Initial }; }
		static bool is_terminal_state(const State& s) { return s.value != Code::Initial; }
		static std::vector<Transition> get_transitions(game::EmptyConfig, const State& state)
		{
			if (state.value != Code::Initial) return {};

			std::vector<Transition> result;
			result.push_back({ 1.0f / 3.0f, State{ Code::Win }, });
			result.push_back({ 1.0f / 3.0f, State{ Code::Draw }, });
			result.push_back({ 1.0f / 3.0f, State{ Code::Loss }, });
			sanitize_transitions(result);
			return result;
		}
		static float compute_intrinsic_desire(const State& state) { return state.value == Code::Win ? 1.0f : 0.0f; }
		static std::string tostr(const State& s)
		{
			switch (s.value)
			{
			case Code::Initial: return "Initial";
			case Code::Win: return "Win";
			case Code::Draw: return "Draw";
			case Code::Loss: return "Loss";
			}
			return "Unknown";
		};
	};
}