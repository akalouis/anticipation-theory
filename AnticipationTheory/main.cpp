import "pch.h";
import game;
import rps_game;
import coin_toss_game;
import hpgame_v1;
import hpgame_rage;
import lanegame;
import lanegame_harvester;

int rps_program()
{
	printf("[rps_game] Analyzing game...\n");
	printf("\n");

	auto analysis = game::analyze_rps_game();
	game::rps_dump_most_fun_moments(analysis.states, analysis.stateNodes);
	printf("Game design score: %f\n", analysis.game_design_score);

	printf("\n");
	printf("\n");

	printf("note: rps_game is a benchmark model for single-step games with perfect information.\n"
		"  1. very high A1 (0.471...). this is indeed a good game.\n"
		"  2. achieves 94.2%% of theoretical maximum (0.5) for single-step games.\n"
		"  3. zero A2~A5, since there's no long term anticipation.\n"
	);
	return 0;
}
int cointoss_program()
{
	printf("[coin_toss_game] Analyzing game...\n");
	auto analysis = game::analyze_coin_toss_game();
	game::coin_toss_dump_most_fun_moments(analysis.states, analysis.stateNodes);
	printf("Game design score: %f\n", analysis.game_design_score);
	printf("\n");
	printf("\n");
	printf("note: coin_toss_game is the theoretical optimal single-step game.\n"
		"  1. perfect A1 (0.5). this is the mathematical maximum for single-step games.\n"
		"     given default bianry intrinsic desire(1 for win, 0 for others)\n"
		"  2. achieves 100%% of theoretical maximum for single-step binary games.\n"
	);
	return 0;
}
int hpgame_program()
{
	printf("[hpgame] Analyzing game...\n");

	auto analysis = game::analyze_hpgame();
	auto analysis2 = game::analyze<hpgame::State>([&](const hpgame::State& s) {return analysis.stateNodes[s].a; });
	auto analysis3 = game::analyze<hpgame::State>([&](const hpgame::State& s) {return analysis2.stateNodes[s].a; });
	auto analysis4 = game::analyze<hpgame::State>([&](const hpgame::State& s) {return analysis3.stateNodes[s].a; });
	auto analysis5 = game::analyze<hpgame::State>([&](const hpgame::State& s) {return analysis4.stateNodes[s].a; });

	//game::hpgame_dump_most_fun_moments(analysis.states, analysis.stateNodes);
	//game::hpgame_dump_most_fun_moments(analysis2.states, analysis2.stateNodes);
	//game::hpgame_dump_most_fun_moments(analysis3.states, analysis3.stateNodes);
	//game::hpgame_dump_most_fun_moments(analysis4.states, analysis4.stateNodes);
	//game::hpgame_dump_most_fun_moments(analysis5.states, analysis5.stateNodes);
	game::hpgame_dump_most_fun_moments_a12345(analysis.states, analysis.stateNodes, analysis2.stateNodes, analysis3.stateNodes, analysis4.stateNodes, analysis5.stateNodes);

	printf("Game design score(A1): %f\n", analysis.game_design_score);
	printf("Game design score(sum(A1~5)): %f\n", analysis.game_design_score +
		analysis2.game_design_score +
		analysis3.game_design_score +
		analysis4.game_design_score +
		analysis5.game_design_score);
	printf("Game design score(simulated): %f\n", game::compute_gamedesign_score_simulation(analysis.stateNodes));
	printf("Game design score(simulated, sum(A1~5)): %f\n", game::compute_gamedesign_score_simulation<hpgame::State>(
		[&](const hpgame::State& s)
		{
			return analysis.stateNodes[s].a +
				analysis2.stateNodes[s].a +
				analysis3.stateNodes[s].a +
				analysis4.stateNodes[s].a +
				analysis5.stateNodes[s].a;
		}
	));

	printf("note: hpgame is a benchmark model for 1vs1 & action packed games.\n"
		"  1. empirically correct result. most fun when both players low HP, least fun when HP gap large.\n"
		"  2. high in A1 and low in A2~A5, confirming immediate tactical focus over strategic planning.\n"
	);
	return 0;
}
int hpgame_interactive_program()
{
	auto analysis = game::analyze_hpgame();

	auto final_state = game::run_hpgame(
		[&](const hpgame::State& s)
		{
			printf("Player1.Hp:%d\tPlayer2.Hp:%d\t", s.hp1, s.hp2);
			printf("Fun:%.2f\n", analysis.stateNodes[s].a);

			auto transitions = s.transitions();
			for (size_t i = 0; i < transitions.size(); i++)
				printf("%lld: %s\n", i, hpgame::tostr(transitions[i].alias).c_str());

			size_t choice; std::cin >> choice; return choice;
		});

	if (final_state.hp1 == 0)
		std::cout << "Player2 wins" << std::endl;
	else
		std::cout << "Player1 wins" << std::endl;

	return 0;
}
int hpgame_rage_analyze_program()
{
	printf("[hpgame_rage] Analyzing game...\n");
	auto analysis = game::analyze_hpgame_rage();
	auto analysis2 = game::analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) {return analysis.stateNodes[s].a; });
	auto analysis3 = game::analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) {return analysis2.stateNodes[s].a; });
	auto analysis4 = game::analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) {return analysis3.stateNodes[s].a; });
	auto analysis5 = game::analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) {return analysis4.stateNodes[s].a; });
	//game::hpgame_rage_dump_most_fun_moments(analysis.states, analysis.stateNodes);
	//game::hpgame_rage_dump_most_fun_moments(analysis2.states, analysis2.stateNodes);
	//game::hpgame_rage_dump_most_fun_moments(analysis3.states, analysis3.stateNodes);
	//game::hpgame_rage_dump_most_fun_moments(analysis4.states, analysis4.stateNodes);
	//game::hpgame_rage_dump_most_fun_moments(analysis5.states, analysis5.stateNodes);
	game::hpgame_rage_dump_most_fun_moments_a12345(analysis.states, analysis.stateNodes, analysis2.stateNodes, analysis3.stateNodes, analysis4.stateNodes, analysis5.stateNodes);

	printf("Game design score(A1): %f\n", analysis.game_design_score);
	printf("Game design score(sum(A1~5)): %f\n", analysis.game_design_score +
		analysis2.game_design_score +
		analysis3.game_design_score +
		analysis4.game_design_score +
		analysis5.game_design_score);
	printf("Game design score(simulated): %f\n", game::compute_gamedesign_score_simulation(analysis.stateNodes));
	printf("Game design score(simulated, sum(A1~5)): %f\n", game::compute_gamedesign_score_simulation<hpgame_rage::State>(
		[&](const hpgame_rage::State& s)
		{
			return analysis.stateNodes[s].a +
				analysis2.stateNodes[s].a +
				analysis3.stateNodes[s].a +
				analysis4.stateNodes[s].a +
				analysis5.stateNodes[s].a;
		}
	));
	return 0;
}
int hpgame_rage_find_optimal_critchance()
{
	// Find optimal critical hit chance
	std::cout << "Analyzing optimal critical hit chance for rage mechanics..." << std::endl;

	game::hpgame_rage_find_optimal_critical_chance(0.0f, 0.40f, 0.01f);

	auto params = hpgame_rage::GameParams(0.11f); // Example: if 15% was determined to be optimal
	auto analysis = game::analyze_hpgame_rage();

	// Print overall results
	std::cout << "\nFinal Game Design Score with optimal critical chance: "
		<< std::fixed << analysis.game_design_score << std::endl;

	// Optional: Dump most fun moments 
	hpgame_dump_most_fun_moments(analysis.states, analysis.stateNodes);
	return 0;
}
int hpgame_rage_compare_mechanics_program()
{
	printf("[hpgame_rage] Comparing different rage mechanics configurations...\n\n");

	// Create array of configurations to test
	std::vector<hpgame_rage::GameParams> configs;

	// Configuration 1: Original (accumulating rage from both attack and defense)
	hpgame_rage::GameParams config1;
	config1.critical_chance = 0.13f;
	config1.rage_spendable = false;
	config1.rage_dmg_multiplier = 1;
	config1.rage_increase_on_attack_dmg = true;
	config1.rage_increase_on_received_dmg = true;
	configs.push_back(config1);

	// Configuration 2: Spendable rage
	hpgame_rage::GameParams config2;
	config2.critical_chance = 0.13f;
	config2.rage_spendable = true;
	config2.rage_dmg_multiplier = 1;
	config2.rage_increase_on_attack_dmg = true;
	config2.rage_increase_on_received_dmg = true;
	configs.push_back(config2);

	// Configuration 3: Offensive rage only (increase only when dealing damage)
	hpgame_rage::GameParams config3;
	config3.critical_chance = 0.13f;
	config3.rage_spendable = false;
	config3.rage_dmg_multiplier = 1;
	config3.rage_increase_on_attack_dmg = true;
	config3.rage_increase_on_received_dmg = false;
	configs.push_back(config3);

	// Configuration 4: Defensive rage only (increase only when receiving damage)
	hpgame_rage::GameParams config4;
	config4.critical_chance = 0.13f;
	config4.rage_spendable = false;
	config4.rage_dmg_multiplier = 1;
	config4.rage_increase_on_attack_dmg = false;
	config4.rage_increase_on_received_dmg = true;
	configs.push_back(config4);

	// Configuration 5: Spendable offensive rage
	hpgame_rage::GameParams config5;
	config5.critical_chance = 0.13f;
	config5.rage_spendable = true;
	config5.rage_dmg_multiplier = 1;
	config5.rage_increase_on_attack_dmg = true;
	config5.rage_increase_on_received_dmg = false;
	configs.push_back(config5);

	// Configuration 6: Spendable defensive rage
	hpgame_rage::GameParams config6;
	config6.critical_chance = 0.13f;
	config6.rage_spendable = true;
	config6.rage_dmg_multiplier = 1;
	config6.rage_increase_on_attack_dmg = false;
	config6.rage_increase_on_received_dmg = true;
	configs.push_back(config6);

	// Configuration 7: Higher damage multiplier
	hpgame_rage::GameParams config7;
	config7.critical_chance = 0.13f;
	config7.rage_spendable = false;
	config7.rage_dmg_multiplier = 2;
	config7.rage_increase_on_attack_dmg = true;
	config7.rage_increase_on_received_dmg = true;
	configs.push_back(config7);

	// Configuration 8: Spendable high multiplier rage
	hpgame_rage::GameParams config8;
	config8.critical_chance = 0.13f;
	config8.rage_spendable = true;
	config8.rage_dmg_multiplier = 2;
	config8.rage_increase_on_attack_dmg = true;
	config8.rage_increase_on_received_dmg = true;
	configs.push_back(config8);

	// Results table header
	printf("Config\tCrit%%\tRageSpend\tDmgMult\tOnAttack\tOnReceive\tA1\t\tA1~5 Sum\tSimulated\n");
	printf("------\t-----\t---------\t-------\t--------\t---------\t--------\t--------\t---------\n");

	size_t config_index = 1;
	double best_score = 0.0;
	size_t best_config = 0;

	// Test each configuration
	for (const auto& config : configs) {
		// Set the current configuration
		hpgame_rage::State::params = config;

		// Run analysis
		auto analysis = game::analyze_hpgame_rage();
		auto analysis2 = game::analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) { return analysis.stateNodes[s].a; });
		auto analysis3 = game::analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) { return analysis2.stateNodes[s].a; });
		auto analysis4 = game::analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) { return analysis3.stateNodes[s].a; });
		auto analysis5 = game::analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) { return analysis4.stateNodes[s].a; });

		// Calculate scores
		double a1_score = analysis.game_design_score;
		double sum_score = a1_score + analysis2.game_design_score + analysis3.game_design_score +
			analysis4.game_design_score + analysis5.game_design_score;

		// Calculate simulated score
		double simulated_score = game::compute_gamedesign_score_simulation<hpgame_rage::State>(
			[&](const hpgame_rage::State& s) {
				return analysis.stateNodes[s].a + analysis2.stateNodes[s].a + analysis3.stateNodes[s].a +
					analysis4.stateNodes[s].a + analysis5.stateNodes[s].a;
			}
		);

		// Print results
		printf("%zu\t%.2f\t%s\t\t%d\t%s\t\t%s\t\t%.6f\t%.6f\t%.6f\n",
			config_index,
			config.critical_chance * 100,
			config.rage_spendable ? "Yes" : "No",
			config.rage_dmg_multiplier,
			config.rage_increase_on_attack_dmg ? "Yes" : "No",
			config.rage_increase_on_received_dmg ? "Yes" : "No",
			a1_score,
			sum_score,
			simulated_score);

		// Track best configuration based on sum score
		if (sum_score > best_score) {
			best_score = sum_score;
			best_config = config_index;
		}

		config_index++;
	}

	printf("\nBest configuration: %zu with sum score: %.6f\n", best_config, best_score);

	// Show detailed analysis of best configuration
	hpgame_rage::State::params = configs[best_config - 1];
	auto best_analysis = game::analyze_hpgame_rage();
	// game design score
	printf("Best configuration game design score: %.6f\n", best_analysis.game_design_score);


	printf("\nBest configuration %zu:\n", best_config);
	printf("Critical Hit: %.2f%%\n", configs[best_config - 1].critical_chance * 100);
	printf("Spend Rage on Critical: %s\n", configs[best_config - 1].rage_spendable ? "Yes" : "No");
	printf("Rage Damage Multiplier: %d\n", configs[best_config - 1].rage_dmg_multiplier);
	printf("Gain Rage on Attack: %s\n", configs[best_config - 1].rage_increase_on_attack_dmg ? "Yes" : "No");
	printf("Gain Rage on Receiving Damage: %s\n", configs[best_config - 1].rage_increase_on_received_dmg ? "Yes" : "No");

	printf("\nMost fun moments with best configuration (Config %zu):\n", best_config);
	game::hpgame_rage_dump_most_fun_moments(best_analysis.states, best_analysis.stateNodes);

	return 0;
}
int optimal_two_turn_game_program()
{

	return 0;
}
int experiment_hpgame_rage_find_optimal_crit_per_config()
{
	printf("[hpgame_rage] Testing ALL possible rage configurations...\n\n");

	// Define parameter ranges
	const float min_crit = 0.05f;         // 5%
	const float max_crit = 0.40f;         // 40%
	const float crit_step = 0.01f;        // 1% steps

	// Generate all possible configurations systematically
	std::vector<hpgame_rage::GameParams> all_configs;

	// Boolean parameters: spendable, attack, defense
	bool spendable_options[] = { false, true };
	bool attack_options[] = { false, true };
	bool defense_options[] = { false, true };

	// Multiplier options: 1 through 5
	int multiplier_options[] = { 1, 2, 3, 4, 5 };

	// Generate all combinations
	for (bool spendable : spendable_options) {
		for (bool attack : attack_options) {
			for (bool defense : defense_options) {
				// Skip invalid configuration where no rage accumulation is possible
				if (!attack && !defense) continue;

				for (int multiplier : multiplier_options) {
					hpgame_rage::GameParams config;
					config.rage_spendable = spendable;
					config.rage_dmg_multiplier = multiplier;
					config.rage_increase_on_attack_dmg = attack;
					config.rage_increase_on_received_dmg = defense;
					all_configs.push_back(config);
				}
			}
		}
	}

	printf("Testing %zu total configurations...\n", all_configs.size());

	// Track overall best configuration
	size_t best_config_index = 0;
	float best_config_crit = 0.0f;
	double best_config_score = 0.0;

	// Store results
	struct Result {
		hpgame_rage::GameParams config;
		float best_crit;
		double best_score;

		// Generate a name for this configuration
		std::string get_name() const {
			std::string name;

			if (config.rage_dmg_multiplier > 1)
				name += "Dmg x" + std::to_string(config.rage_dmg_multiplier) + " ";

			if (config.rage_spendable)
				name += "Spend ";

			if (config.rage_increase_on_attack_dmg && config.rage_increase_on_received_dmg)
				name += "Both";
			else if (config.rage_increase_on_attack_dmg)
				name += "Attack";
			else if (config.rage_increase_on_received_dmg)
				name += "Defense";

			return name;
		}
	};
	std::vector<Result> results;

	// Progress tracking
	int total_configs = (int)all_configs.size();
	int completed = 0;
	int last_percent = -1;
	printf("Progress: ");

	// Test each configuration
	for (const auto& base_config : all_configs) {
		// Find optimal crit for this configuration
		float best_crit = min_crit;
		double best_score = 0.0;

		for (float crit = min_crit; crit <= max_crit; crit += crit_step) {
			// Update config with current crit
			hpgame_rage::GameParams config = base_config;
			config.critical_chance = crit;

			// Set the current configuration
			hpgame_rage::State::params = config;

			// Run analysis
			auto analysis = game::analyze_hpgame_rage();
			auto analysis2 = game::analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) { return analysis.stateNodes[s].a; });
			auto analysis3 = game::analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) { return analysis2.stateNodes[s].a; });
			auto analysis4 = game::analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) { return analysis3.stateNodes[s].a; });
			auto analysis5 = game::analyze<hpgame_rage::State>([&](const hpgame_rage::State& s) { return analysis4.stateNodes[s].a; });

			// Calculate sum score
			double sum_score = analysis.game_design_score +
				analysis2.game_design_score +
				analysis3.game_design_score +
				analysis4.game_design_score +
				analysis5.game_design_score;

			// Update best crit if score is better
			if (sum_score > best_score) {
				best_score = sum_score;
				best_crit = crit;
			}
		}

		// Store the result
		Result result = { base_config, best_crit, best_score };
		results.push_back(result);

		// Update best overall if needed
		if (best_score > best_config_score) {
			best_config_score = best_score;
			best_config_crit = best_crit;
			best_config_index = results.size() - 1;
		}

		// Update progress
		completed++;
		int percent_done = (completed * 100) / total_configs;
		if (percent_done > last_percent) {
			printf("%d%%...", percent_done);
			fflush(stdout);
			last_percent = percent_done;
		}
	}

	printf("done!\n\n");

	// Sort results by score (descending)
	std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
		return a.best_score > b.best_score;
		});

	// Print sorted results table
	printf("Ranked Configurations:\n");
	printf("Rank\tConfig\t\tCrit%%\tRageSpend\tDmgMult\tOnAttack\tOnReceive\tScore\n");
	printf("----\t------------\t-----\t---------\t-------\t--------\t---------\t-----------\n");

	for (size_t i = 0; i < results.size(); i++) {
		const auto& result = results[i];
		std::string name = result.get_name();

		printf("%zu\t%-12s\t%.2f\t%s\t\t%d\t%s\t\t%s\t\t%.6f\n",
			i + 1,
			name.c_str(),
			result.best_crit * 100,
			result.config.rage_spendable ? "Yes" : "No",
			result.config.rage_dmg_multiplier,
			result.config.rage_increase_on_attack_dmg ? "Yes" : "No",
			result.config.rage_increase_on_received_dmg ? "Yes" : "No",
			result.best_score);

		// Only show top 20 results to keep output manageable
		if (i >= 19) {
			printf("... plus %zu more configurations (showing top 20 only)\n", results.size() - 20);
			break;
		}
	}

	// Show the very best configuration
	const auto& best_result = results[0];
	printf("\nOverall Best Configuration:\n");
	printf("Critical Hit%%: %.2f%%\n", best_result.best_crit * 100);
	printf("Rage Spendable: %s\n", best_result.config.rage_spendable ? "Yes" : "No");
	printf("Damage Multiplier: %d\n", best_result.config.rage_dmg_multiplier);
	printf("Rage on Attack: %s\n", best_result.config.rage_increase_on_attack_dmg ? "Yes" : "No");
	printf("Rage on Defense: %s\n", best_result.config.rage_increase_on_received_dmg ? "Yes" : "No");
	printf("Score: %.6f\n", best_result.best_score);

	// Set the best configuration for detailed analysis
	hpgame_rage::State::params = best_result.config;
	hpgame_rage::State::params.critical_chance = best_result.best_crit;
	auto final_analysis = game::analyze_hpgame_rage();

	printf("\nMost fun moments with best configuration:\n");
	game::hpgame_rage_dump_most_fun_moments(final_analysis.states, final_analysis.stateNodes);

	return 0;
}
int lanegame_analyze_program()
{
	printf("[lanegame] Analyzing game...\n");
	auto analysis = game::analyze_lanegame();
	auto analysis2 = game::analyze<lanegame::State>([&](const lanegame::State& s) { return analysis.stateNodes[s].a; });
	auto analysis3 = game::analyze<lanegame::State>([&](const lanegame::State& s) { return analysis2.stateNodes[s].a; });
	auto analysis4 = game::analyze<lanegame::State>([&](const lanegame::State& s) { return analysis3.stateNodes[s].a; });
	auto analysis5 = game::analyze<lanegame::State>([&](const lanegame::State& s) { return analysis4.stateNodes[s].a; });
	game::lanegame_dump_most_fun_moments(analysis.states, analysis.stateNodes);
	game::lanegame_dump_most_fun_moments(analysis2.states, analysis2.stateNodes);
	game::lanegame_dump_most_fun_moments(analysis3.states, analysis3.stateNodes);
	game::lanegame_dump_most_fun_moments(analysis4.states, analysis4.stateNodes);
	game::lanegame_dump_most_fun_moments(analysis5.states, analysis5.stateNodes);
	game::lanegame_dump_most_fun_moments_a12345(analysis.states, analysis.stateNodes, analysis2.stateNodes, analysis3.stateNodes, analysis4.stateNodes, analysis5.stateNodes);
	printf("Game design score: %f\n", analysis.game_design_score);
	printf("Game design score(sum(A1~5)): %f\n", analysis.game_design_score +
		analysis2.game_design_score +
		analysis3.game_design_score +
		analysis4.game_design_score +
		analysis5.game_design_score);

	printf("note: lanegame serves as a benchmark model for MOBA-style strategic games.\n"
		"  1. most engaging when early disadvantage states with recovery potential,\n"
		"     while terminal or heavily imbalanced states generate minimal player interest.\n"
		"  2. reflecting MOBA's emphasis on strategic depth through high A2-A5 values with moderate A1,\n"
		"     contrasting immediate mechanical responsiveness found in action-oriented games.\n"
		"  3. first quantitative measurement of strategic narrative engagement versus action-packed\n"
		"     engagement in competitive games, demonstrating ToA's objective analysis capabilities.\n"
		"  4. resolves the 'game-ending button problem' where maximizing immediate uncertainty\n"
		"     through high-stakes mechanics creates high A1 but poor overall experience.\n"
		"     Higher order anticipation components A2-A5 capture strategic depth and narrative flow,\n"
		"     further validating ToA's comprehensive engagement measurement framework.\n"
		"  5. deeper games require systematic design methodology beyond intuitive approaches.\n"
		"     A1 remains intuitive for designers, while higher order anticipation A2-A5 becomes\n"
		"     increasingly complex, requiring nested dynamic programming computations that exceed\n"
		"     conventional design intuition and necessitate mathematical analysis tools.\n"
		"  6. opening new possibilities for advancing strategic game design innovation.\n"
		"     A1 mechanics represent thoroughly explored design space, while A2-A5 and higher-order\n"
		"     engagement patterns remain largely unexplored due to computational complexity.\n"
		"     ToA provides essential mathematical framework for systematic exploration of engagement\n"
		"     possibilities that could surpass current industry benchmarks such as League of Legends.\n"
	);
	return 0;
}
int lanegame_harvester_analyze_program()
{
	printf("[lanegame_harvester] Analyzing game...\n");
	auto analysis = game::analyze_lanegame_harvester();
	auto analysis2 = game::analyze<lanegame_harvester::State>([&](const lanegame_harvester::State& s) { return analysis.stateNodes[s].a; });
	auto analysis3 = game::analyze<lanegame_harvester::State>([&](const lanegame_harvester::State& s) { return analysis2.stateNodes[s].a; });
	auto analysis4 = game::analyze<lanegame_harvester::State>([&](const lanegame_harvester::State& s) { return analysis3.stateNodes[s].a; });
	auto analysis5 = game::analyze<lanegame_harvester::State>([&](const lanegame_harvester::State& s) { return analysis4.stateNodes[s].a; });
	game::lanegame_harvester_dump_most_fun_moments(analysis.states, analysis.stateNodes);
	game::lanegame_harvester_dump_most_fun_moments(analysis2.states, analysis2.stateNodes);
	game::lanegame_harvester_dump_most_fun_moments(analysis3.states, analysis3.stateNodes);
	game::lanegame_harvester_dump_most_fun_moments(analysis4.states, analysis4.stateNodes);
	game::lanegame_harvester_dump_most_fun_moments(analysis5.states, analysis5.stateNodes);
	game::lanegame_harvester_dump_most_fun_moments_a12345(analysis.states, analysis.stateNodes, analysis2.stateNodes, analysis3.stateNodes, analysis4.stateNodes, analysis5.stateNodes);
	printf("Game design score: %f\n", analysis.game_design_score);
	printf("Game design score(sum(A1~5)): %f\n", analysis.game_design_score +
		analysis2.game_design_score +
		analysis3.game_design_score +
		analysis4.game_design_score +
		analysis5.game_design_score);
	return 0;
}

int main()
{
	enum program_code
	{
		rock_paper_scissors,
		cointoss,
		hpgame,
		hpgame_interactive,
		hpgame_rage,
		hpgame_rage_optimize_critchance,
		hpgame_rage_compare_mechanics,
		optimal_two_turn_game,

	};

	//return hpgame_rage_find_optimal_critchance();
	//return experiment_hpgame_rage_find_optimal_crit_per_config();

	switch (hpgame_rage_compare_mechanics) // change this to run different programs
	{
	case rock_paper_scissors: return rps_program();
	case cointoss: return cointoss_program();
	case hpgame: return hpgame_program();
	case hpgame_interactive: return hpgame_interactive_program();
	case hpgame_rage: return hpgame_rage_analyze_program();
	case hpgame_rage_optimize_critchance: return hpgame_rage_find_optimal_critchance();
	case hpgame_rage_compare_mechanics: return hpgame_rage_compare_mechanics_program();
	case optimal_two_turn_game: return optimal_two_turn_game_program();
		//return experiment_hpgame_rage_find_optimal_crit_per_config();
		//return lanegame_analyze_program();
		//return lanegame_harvester_analyze_program();
	}
	//return lanegame_harvester_analyze_program();
	//return lanegame_analyze_program();
	//return hpgame_rage_find_optimal_critchance();
	//return hpgame_rage_compare_mechanics();
	//return experiment_hpgame_rage_find_optimal_crit_per_config();
	//return hpgame_program();
	//return hpgame_interactive_program();
	//return hpgame_rage_analyze_program();
}
