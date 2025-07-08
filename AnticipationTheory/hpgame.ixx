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
		typedef game::EmptyConfig Config;
		typedef State State;

		static State initial_state() { return State{ 5, 5 }; }
		static bool is_terminal_state(const State& s) { return s.hp1 <= 0 || s.hp2 <= 0; }
		static std::vector<Transition> get_transitions(Config, const State& state)
		{
			std::vector<Transition> result;
			if (is_terminal_state(state)) return result;

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
		static std::string tostr(TransitionAlias alias)
		{
			switch (alias)
			{
			case TransitionAlias::AttackAndMiss: return "AttackAndMiss";
			case TransitionAlias::BothAttack: return "BothAttack";
			case TransitionAlias::MissAndAttack: return "AttackReceived";
			}
			return "Unknown";
		}
	};
}