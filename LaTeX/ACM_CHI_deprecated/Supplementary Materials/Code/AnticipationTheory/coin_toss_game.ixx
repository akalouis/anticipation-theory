module;
export module coin_toss_game;

import "pch.h";
import game;

export namespace coin_toss_game
{
    enum class StateCode : unsigned char
    {
        Initial,
        Win,
        Loss
    };
    struct State
    {
        StateCode value;
        auto operator <=> (const State&) const = default;
    };
    struct Transition { float probability; State to; };

    struct Game
    {
        typedef game::EmptyConfig Config;
        typedef State State;

		static State initial_state() { return State{ StateCode::Initial }; }
        static bool is_terminal_state(const State& s) { return s.value != StateCode::Initial; }
        static std::vector<Transition> get_transitions(game::EmptyConfig, const State& state)
        {
            if (is_terminal_state(state)) return {};

            std::vector<Transition> result;
            result.push_back({ 0.5f, State{ StateCode::Win } });
            result.push_back({ 0.5f, State{ StateCode::Loss } });
            return result;
        }

        static float compute_intrinsic_desire(const State& state)
        {
            return state.value == StateCode::Win ? 1.0f : 0.0f;
        }

        static std::string tostr(const State& s)
        {
            switch (s.value)
            {
            case StateCode::Initial: return "Initial";
            case StateCode::Win: return "Win";
            case StateCode::Loss: return "Loss";
            }
            return "Unknown";
        }
    };
}