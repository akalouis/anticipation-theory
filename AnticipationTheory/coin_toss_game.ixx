module;
export module coin_toss_game;

import "pch.h";
import game;

export namespace coin_toss_game
{
	struct Transition;
	enum class TransitionAlias : unsigned char
	{
		Win,
		Loss
	};

	struct State
	{
		bool is_initial;
		TransitionAlias result; // only used for terminal states

		auto operator <=> (const State&) const = default;

		bool is_terminal() const;
		std::vector<Transition> transitions() const;

		static State initial() { return State{ true, TransitionAlias::Win }; }
	};

	struct Transition
	{
		float probability;
		State to;
		TransitionAlias alias;
	};

	State initial_state() { return State{ true, TransitionAlias::Win }; }

	bool State::is_terminal() const { return !is_initial; }

	std::vector<Transition> State::transitions() const
	{
		std::vector<Transition> result;
		if (!is_initial) return result; // terminal states have no transitions

		// From initial state: Win, Loss with equal probability (perfect binary game)
		result.push_back({ 0.5f, State{ false, TransitionAlias::Win }, TransitionAlias::Win });
		result.push_back({ 0.5f, State{ false, TransitionAlias::Loss }, TransitionAlias::Loss });

		// validate probabilities
		float total = 0.0f;
		for (const auto& ts : result)
			total += ts.probability;
		if (abs(total - 1.0f) > 0.001f)
			throw std::runtime_error("Invalid probabilities");

		return result;
	}

	std::string tostr(TransitionAlias alias)
	{
		switch (alias)
		{
		case TransitionAlias::Win: return "Win";
		case TransitionAlias::Loss: return "Loss";
		}
		return "Unknown";
	};
}

export namespace game
{
	GameAnalysis<coin_toss_game::State> analyze_coin_toss_game()
	{
		return analyze<coin_toss_game::State>([](const coin_toss_game::State& state)
			{
				return
					!state.is_initial && state.result == coin_toss_game::TransitionAlias::Win
					? 1.0f
					: 0.0f;
			});
	}

	coin_toss_game::State run_coin_toss_game(const std::function<size_t(const coin_toss_game::State&)> onChoice)
	{
		return run<coin_toss_game::State>(onChoice);
	}

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
			std::string state_name = state.is_initial ? "Initial" : coin_toss_game::tostr(state.result);

			if (node.a > 0.0f)
				printf("%s\t\t%.2f\t%.2f\t\t%.2f\n", state_name.c_str(), node.d_local, node.d_global, node.a);
		}
	}
}