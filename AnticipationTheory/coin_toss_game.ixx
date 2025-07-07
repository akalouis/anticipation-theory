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
        typedef game::EmptyConfig config_t;
        typedef State state_t;

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
            case StateCode::Win: return "Win";
            case StateCode::Loss: return "Loss";
            case StateCode::Initial: return "Initial";
            }
            return "Unknown";
        }
    };
}

export namespace game
{
	template<typename state_t> void coin_toss_dump_most_fun_moments(const std::vector<state_t>& states, std::map<state_t, StateNode>& stateNodes)
	{
		std::vector<state_t> states_sorted_by_a = states;
		std::sort(states_sorted_by_a.begin(), states_sorted_by_a.end(),
			[&](const state_t& a, const state_t& b)
			{
				return stateNodes[a].a > stateNodes[b].a;
			});

		printf("Most fun moments(sorted by A)\n");
		printf("State\t\tD_local\tD_global\tA\n");
		for (const auto& state : states_sorted_by_a)
		{
			auto& node = stateNodes[state];
			std::string state_name = coin_toss_game::Game::tostr(state);

			if (node.a > 0.0f)
				printf("%s\t\t%.2f\t%.2f\t\t%.2f\n", state_name.c_str(), node.d_local, node.d_global, node.a);
		}
	}
}