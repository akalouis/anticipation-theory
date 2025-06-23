module;
export module game;

import "pch.h";

export namespace game
{
	struct StateNode
	{
		float d_local = 0.0f;	// intrinsic desire
		float d_global = 0.0f;	// propagated desire
		float a = 0.0f;			// anticipation, measured engagement of given state

		struct StepNode
		{
			float a = 0.0f;
			float reach_probability = 0.0f;
		};

		std::map<size_t, StepNode> steps; // propagated anticipation for game design score, step -> current_total_anticipation
	};

	template<typename state_t> void traverseR(const state_t& s, std::function<void(const state_t& s)> onState)
	{
		std::set<state_t> visited;
		std::function<void(const state_t&)> recurse;
		recurse = [&](const state_t& s)
			{
				if (visited.find(s) != visited.end()) return;
				visited.insert(s).second;
				for (const auto& ts : s.transitions())
					recurse(ts.to);
				onState(s);
			};
		recurse(s);
	};
	template<typename state_t> std::vector<state_t> serializeR(state_t start_state)
	{
		std::vector<state_t> states_serialized;

		traverseR<state_t>(
			start_state,
			[&](const state_t& s)
			{
				states_serialized.push_back(s);
			});

		return states_serialized;
	};
	template<typename state_t> struct GameAnalysis
	{
		std::vector<state_t> states;
		std::vector<state_t> states_R;
		std::map<state_t, StateNode> stateNodes;
		double game_design_score;
	};
	template<typename state_t> GameAnalysis<state_t> analyze(
		std::function<float(const state_t&)> compute_intrinsic_desire
	)
	{
		auto start_state = state_t::initial();

		std::vector<state_t> states;
		std::vector<state_t> states_R;
		std::map<state_t, StateNode> stateNodes;
		double game_design_score = 0.0;

		states_R = serializeR(start_state);
		std::reverse_copy(states_R.begin(), states_R.end(), std::back_inserter(states));

		auto buildNodes = [&]()
			{
				for (const auto& state : states)
					stateNodes[state] = StateNode{};
			};
		auto seedD = [&]()
			{
				for (const auto& state : states)
				{
					auto& node = stateNodes[state];
					node.d_local = compute_intrinsic_desire(state);
					//if (state.hp1 != 0 && state.hp2 == 0)		// player1 alive && player2 dead	
					//	node.d_local = 1.0f;					// player1 wins
				}
			};
		auto propagateD = [&]()
			{
				for (const auto& state : states_R)
				{
					auto& node = stateNodes[state];
					if (node.d_global) throw std::runtime_error("Global D already seeded");
					node.d_global = node.d_local;

					for (const auto& ts : state.transitions())
					{
						auto& tnode = stateNodes[ts.to];
						node.d_global += tnode.d_global * ts.probability;
					}
				}
			};
		auto compute_A = [&]()
			{
				for (const auto& state : states_R)
				{
#ifdef _DEBUG
					//if (state == State(5, 1) || state == State(1, 1)) __debugbreak();
#endif

					auto& node = stateNodes[state];

					const auto transitions = state.transitions();
					const size_t transition_count = transitions.size();
					if (transition_count == 0)
					{
						node.a = 0.0f;
						continue;
					}

					auto compute_A = [&]()
						{
							// 1. basically weighted std deviation
							// 2. note we are using D: D_perspective
							//	  D_perspective = D(s2) - D(s1)

							float sum_pd = 0.0f;
							for (const auto& ts : transitions)
							{
								auto& tnode = stateNodes[ts.to];
								float perspective_desire = tnode.d_global - node.d_global;
								sum_pd += ts.probability * perspective_desire;  // sum_pe += P ¡¿ D
							}

							float avg_pd = sum_pd;

							float weighted_variance = 0.0f;
							for (const auto& ts : transitions)
							{
								float perspective_desire = stateNodes[ts.to].d_global - node.d_global;
								float diff = perspective_desire - avg_pd; // diff = D - avg(PD)
								float diff_sq = diff * diff;

								weighted_variance += ts.probability * diff_sq;
							}

							// No need to divide by transition_count since we're using weighted sum
							return std::sqrt(weighted_variance);
						};
					node.a = compute_A();
				}
			};
		auto compute_gamedesign_score = [&]()
			{
				auto propagate_reach_p = [&]()
					{
						stateNodes[start_state].steps[0].reach_probability = 1.0f;
						for (const auto& state : states)
						{
							auto& node = stateNodes[state];
							for (const auto& ts : state.transitions())
							{
								auto& tnode = stateNodes[ts.to];
								for (auto& [si, step] : node.steps)
									tnode.steps[si + 1].reach_probability += step.reach_probability * ts.probability;
							}
						}
					};
				auto propagate_A = [&]()
					{
						stateNodes[start_state].steps[0].a = stateNodes[start_state].a;
						for (const auto& state : states)
						{
							auto& node = stateNodes[state];
							for (const auto& ts : state.transitions())
							{
								auto& tnode = stateNodes[ts.to];
								for (auto& [si, step] : node.steps)
								{
									// Propagate current accumulated A and add target state's A once
									tnode.steps[si + 1].a += step.a * ts.probability + tnode.a * step.reach_probability * ts.probability;
								}
							}
						}
					};

				double total_end_probability = 0.0;
				auto sum = [&]()
					{
						for (const auto& state : states)
						{
							auto& node = stateNodes[state];
							if (state.is_terminal())
							{
								for (const auto& [si, step] : node.steps)
								{
									total_end_probability += step.reach_probability;
									game_design_score += step.a / si;
								}
								//game_design_score += node.a_propagated * node.reach_probability 
							}
						}

						// almost same
						bool almost_same = std::abs(total_end_probability - 1.0f) < 0.0001f;
						if (!almost_same)
							throw std::runtime_error("Bug found in probability propagation");
					};

				propagate_reach_p();
				propagate_A();
				sum();
			};

		buildNodes();
		seedD();
		propagateD();
		compute_A();
		compute_gamedesign_score();

		GameAnalysis<state_t> result{};
		result.states = std::move(states);
		result.states_R = std::move(states_R);
		result.stateNodes = std::move(stateNodes);
		result.game_design_score = game_design_score;
		return result;
	}
	template<typename state_t> state_t run(const std::function<size_t(const state_t&)> onChoice)
	{
		state_t current_state = state_t::initial();
		while (!current_state.is_terminal())
		{
			size_t choice;
			choice = onChoice(current_state);
			current_state = current_state.transitions()[choice].to;
		}

		return current_state;
	}


	template<typename state_t>
	double compute_gamedesign_score_simulation(
		const std::function<float(const state_t&)>& state_to_anticipation)
	{
		size_t game_count = 10'000;
		double total_anticipation = 0.0;

		for (size_t gi = 0; gi < game_count; gi++)
		{
			std::random_device rd;
			std::mt19937 rng(rd());

			double total_anticipation_for_thisgame = 0.0;
			double avg_anticipation_for_thisgame = 0.0;
			size_t total_turns = 0;

			state_t current_state = state_t::initial();
			current_state = run<state_t>(
				[&](const state_t& s)
				{
					float anticipation = state_to_anticipation(s);
					total_anticipation_for_thisgame += anticipation;
					total_turns++;

					auto transitions = s.transitions();
					std::vector<float> probabilities;
					for (const auto& ts : transitions)
						probabilities.push_back(ts.probability);

					std::discrete_distribution<size_t> dist(probabilities.begin(), probabilities.end());
					return dist(rng);
				});

			// Add final state anticipation
			total_anticipation_for_thisgame += state_to_anticipation(current_state);
			//total_turns++;

			avg_anticipation_for_thisgame = total_anticipation_for_thisgame / total_turns;
			total_anticipation += avg_anticipation_for_thisgame;
		}

		double score = total_anticipation / game_count;
		return score;
	}

	// Convenience overload that uses StateNode map
	template<typename state_t>
	double compute_gamedesign_score_simulation(std::map<state_t, StateNode>& stateNodes)
	{
		return compute_gamedesign_score_simulation<state_t>(
			[&stateNodes](const state_t& s) { return stateNodes[s].a; });
	}
}
