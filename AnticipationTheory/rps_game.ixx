module;
export module rps_game;

import "pch.h";
import game;

export namespace rps_game
{
	struct Transition;
	enum class TransitionAlias : unsigned char
	{
		Win,
		Draw,
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

		// From initial state: Win, Draw, Loss with equal probability
		result.push_back({ 1.0f / 3.0f, State{ false, TransitionAlias::Win }, TransitionAlias::Win });
		result.push_back({ 1.0f / 3.0f, State{ false, TransitionAlias::Draw }, TransitionAlias::Draw });
		result.push_back({ 1.0f / 3.0f, State{ false, TransitionAlias::Loss }, TransitionAlias::Loss });

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
		case TransitionAlias::Draw: return "Draw";
		case TransitionAlias::Loss: return "Loss";
		}
		return "Unknown";
	};
}

export namespace game
{
	GameAnalysis<rps_game::State> analyze_rps_game()
	{
		return analyze<rps_game::State>([](const rps_game::State& state)
			{
				return
					!state.is_initial && state.result == rps_game::TransitionAlias::Win
					? 1.0f
					: 0.0f;
			});
	}
	rps_game::State run_rps_game(const std::function<size_t(const rps_game::State&)> onChoice)
	{
		return run<rps_game::State>(onChoice);
	}

	template<typename state_t> void rps_dump_most_fun_moments(const std::vector<state_t>& states, std::map<state_t, StateNode>& stateNodes)
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
			std::string state_name = state.is_initial ? "Initial" : rps_game::tostr(state.result);

			if (node.a > 0.0f)
				printf("%s\t\t%.2f\t%.2f\t\t%.2f\n", state_name.c_str(), node.d_local, node.d_global, node.a);
		}
	}
}