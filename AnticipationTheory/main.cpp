import "pch.h";
import game;
import rps_game;
import coin_toss_game;
import hpgame_v1;
import hpgame_rage;
import lanegame;

using namespace game;

int rps_program()
{
	auto analysis = analyze<rps_game::Game>();
	dump_most_fun_moments<rps_game::Game>(analysis);
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
	auto analysis = analyze<coin_toss_game::Game>();
	dump_most_fun_moments<coin_toss_game::Game>(analysis);
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
	using namespace hpgame;

	auto config = Game::config_t();
	auto initial_state = Game::initial_state();
	auto analysis = game::analyze<Game>(initial_state, Game::compute_intrinsic_desire, config, 5);

	printf("Most engaging moments(sorted by sum(A))\n");
	dump_most_fun_moments<Game>(analysis);

	printf("Game design score): %f\n", analysis.game_design_score);
	printf("Game design score(simulated): %f\n", game::compute_gamedesign_score_simulation<Game>(
		[&](const State& s)
		{
			return analysis.stateNodes[s].sum_A();
		}, config));

	printf("note: hpgame is a benchmark model for 1vs1 & action packed games.\n"
		"  1. empirically correct result. most fun when both players low HP, least fun when HP gap large.\n"
		"  2. high in A1 and low in A2~A5, confirming immediate tactical focus over strategic planning.\n"
	);
	return 0;
}
int hpgame_interactive_program()
{
	using namespace hpgame;

	auto config = Game::config_t();
	auto initial_state = Game::initial_state();
	auto analysis = game::analyze<hpgame::Game>(initial_state, Game::compute_intrinsic_desire, config);
	auto final_state = game::run<hpgame::Game>(
		initial_state,
		[&](const hpgame::State& s)
		{
			printf("Player1.Hp:%d\tPlayer2.Hp:%d\t", s.hp1, s.hp2);
			printf("Fun:%.2f\n", analysis.stateNodes[s].sum_A());

			auto transitions = Game::get_transitions(Game::config_t(), s);
			for (size_t i = 0; i < transitions.size(); i++)
				printf("%lld: %s\n", i, hpgame::tostr(transitions[i].alias).c_str());

			size_t choice; std::cin >> choice; return choice;
		},
		config);

	if (final_state.hp1 == 0)
		std::cout << "Player2 wins" << std::endl;
	else
		std::cout << "Player1 wins" << std::endl;

	return 0;
}
int hpgame_rage_analyze_program()
{
	using namespace hpgame_rage;

	auto config = Config();
	auto initial_state = Game::initial_state();
	auto analysis = game::analyze<Game>(initial_state, Game::compute_intrinsic_desire, config, 5);
	printf("Most engaging moments(sorted by sum(A))\n");
	dump_most_fun_moments<Game>(analysis);

	printf("Game design score(sum(A1~5)): %f\n", analysis.game_design_score);
	printf("Game design score(simulated, sum(A1~5)): %f\n",
		game::compute_gamedesign_score_simulation<Game>(
		[&](const hpgame_rage::State& s)
		{
			return analysis.stateNodes[s].sum_A();
		}, config
	));
	return 0;
}
int hpgame_rage_find_optimal_critchance()
{
	std::cout << "Analyzing optimal critical hit chance for rage mechanics..." << std::endl;
	game::hpgame_rage_find_optimal_critical_chance(0.0f, 0.40f, 0.01f);
	return 0;
}
int hpgame_rage_compare_mechanics_program()
{
	using namespace hpgame_rage;

	printf("[hpgame_rage] Comparing different rage mechanics configurations...\n\n");

	std::vector<Config> configs;

	// Configuration 1: Original (accumulating rage from both attack and defense)
	Config config1;
	config1.critical_chance = 0.13f;
	config1.rage_spendable = false;
	config1.rage_dmg_multiplier = 1;
	config1.rage_increase_on_attack_dmg = true;
	config1.rage_increase_on_received_dmg = true;
	configs.push_back(config1);

	// Configuration 2: Spendable rage
	Config config2;
	config2.critical_chance = 0.13f;
	config2.rage_spendable = true;
	config2.rage_dmg_multiplier = 1;
	config2.rage_increase_on_attack_dmg = true;
	config2.rage_increase_on_received_dmg = true;
	configs.push_back(config2);

	// Configuration 3: Offensive rage only (increase only when dealing damage)
	Config config3;
	config3.critical_chance = 0.13f;
	config3.rage_spendable = false;
	config3.rage_dmg_multiplier = 1;
	config3.rage_increase_on_attack_dmg = true;
	config3.rage_increase_on_received_dmg = false;
	configs.push_back(config3);

	// Configuration 4: Defensive rage only (increase only when receiving damage)
	Config config4;
	config4.critical_chance = 0.13f;
	config4.rage_spendable = false;
	config4.rage_dmg_multiplier = 1;
	config4.rage_increase_on_attack_dmg = false;
	config4.rage_increase_on_received_dmg = true;
	configs.push_back(config4);

	// Configuration 5: Spendable offensive rage
	Config config5;
	config5.critical_chance = 0.13f;
	config5.rage_spendable = true;
	config5.rage_dmg_multiplier = 1;
	config5.rage_increase_on_attack_dmg = true;
	config5.rage_increase_on_received_dmg = false;
	configs.push_back(config5);

	// Configuration 6: Spendable defensive rage
	Config config6;
	config6.critical_chance = 0.13f;
	config6.rage_spendable = true;
	config6.rage_dmg_multiplier = 1;
	config6.rage_increase_on_attack_dmg = false;
	config6.rage_increase_on_received_dmg = true;
	configs.push_back(config6);

	// Configuration 7: Higher damage multiplier
	Config config7;
	config7.critical_chance = 0.13f;
	config7.rage_spendable = false;
	config7.rage_dmg_multiplier = 2;
	config7.rage_increase_on_attack_dmg = true;
	config7.rage_increase_on_received_dmg = true;
	configs.push_back(config7);

	// Configuration 8: Spendable high multiplier rage
	Config config8;
	config8.critical_chance = 0.13f;
	config8.rage_spendable = true;
	config8.rage_dmg_multiplier = 2;
	config8.rage_increase_on_attack_dmg = true;
	config8.rage_increase_on_received_dmg = true;
	configs.push_back(config8);

	// Results table header
	printf("Config\tCrit%%\tRageSpend\tDmgMult\tOnAttack\tOnReceive\t\tA1~5 Sum\tSimulated\n");
	printf("------\t-----\t---------\t-------\t--------\t---------\t--------\t--------\t---------\n");

	size_t config_index = 1;
	double best_score = 0.0;
	size_t best_config = 0;

	// Test each configuration
	for (const auto& config : configs)
	{
		auto initial_state = Game::initial_state();
		auto analysis = game::analyze<Game>(initial_state, Game::compute_intrinsic_desire, config, 5);
		double sum_score = analysis.game_design_score;

		// Calculate simulated score
		double simulated_score = game::compute_gamedesign_score_simulation<Game>(
			[&](const hpgame_rage::State& s)
			{
				return analysis.stateNodes[s].sum_A();
			}, config
		);

		// Print results
		printf("%zu\t%.2f\t%s\t\t%d\t%s\t\t%s\t\t%.6f\t%.6f\n",
			config_index,
			config.critical_chance * 100,
			config.rage_spendable ? "Yes" : "No",
			config.rage_dmg_multiplier,
			config.rage_increase_on_attack_dmg ? "Yes" : "No",
			config.rage_increase_on_received_dmg ? "Yes" : "No",
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
	printf("Critical Hit: %.2f%%\n", configs[best_config - 1].critical_chance * 100);
	printf("Spend Rage on Critical: %s\n", configs[best_config - 1].rage_spendable ? "Yes" : "No");
	printf("Rage Damage Multiplier: %d\n", configs[best_config - 1].rage_dmg_multiplier);
	printf("Gain Rage on Attack: %s\n", configs[best_config - 1].rage_increase_on_attack_dmg ? "Yes" : "No");
	printf("Gain Rage on Receiving Damage: %s\n", configs[best_config - 1].rage_increase_on_received_dmg ? "Yes" : "No");

	return 0;
}
int hpgame_rage_optimized_program()
{
	using namespace hpgame_rage;
	Config config;
	config.critical_chance = 0.13f; // Example critical chance
	config.rage_spendable = false; // Rage is not spent on critical hits
	config.rage_dmg_multiplier = 2; // Rage damage multiplier
	config.rage_increase_on_attack_dmg = true; // Increase rage when dealing damage
	config.rage_increase_on_received_dmg = true; // Increase rage when receiving damage
	printf("[hpgame_rage] Analyzing optimized rage game...\n");
	auto initial_state = Game::initial_state();
	auto analysis = game::analyze<Game>(initial_state, Game::compute_intrinsic_desire, config, 5);
	printf("Most engaging moments(sorted by sum(A))\n");
	dump_most_fun_moments<Game>(analysis);
	printf("Game design score(sum(A1~5)): %f\n", analysis.game_design_score);
	printf("Game design score(simulated, sum(A1~5)): %f\n", game::compute_gamedesign_score_simulation<Game>(
		[&](const hpgame_rage::State& s)
		{
			return analysis.stateNodes[s].sum_A();
		}, config
	));

	puts("note: hpgame_rage_optimized is a rage mechanics game with optimized parameters.\n"
		"  1. more than +50% increase in GDS.\n"
		"  2. if this game is fun, this is an empirical proof that GDS matches human engagement.\n"
	);


	// let's print precalculated A1-A5 values for external usage.
	//const ANTICIPATION_TABLE = {
	//    // Format: "hp1,hp2,rage1,rage2": [A1,A2,A3,A4,A5,SUM]
	//    "5,5,0,0": 
	//    "5,4,1,1": 
	//    "4,4,2,2": 
	//    "2,2,3,3": 
	//    "1,1,5,5": 
	//    // ... (all calculated states)
	//};

	// Print precalculated anticipation table for JavaScript usage
	auto printAnticipationTableForJS = [&]()
		{
			std::cout << "// Precalculated Anticipation values for HpGame_Rage_optimized" << std::endl;
			std::cout << "const ANTICIPATION_TABLE = {" << std::endl;
			std::cout << "    // Format: \"hp1,hp2,rage1,rage2\": [A1,A2,A3,A4,A5,SUM]" << std::endl;

			for (const auto& state : analysis.states)
			{
				auto& node = analysis.stateNodes[state];

				// Format key
				std::string key = std::to_string(state.hp1) + "," +
					std::to_string(state.hp2) + "," +
					std::to_string(state.rage1) + "," +
					std::to_string(state.rage2);

				// Format values
				std::cout << "    \"" << key << "\": [" <<
					node.a[0] << "," <<
					node.a[1] << "," <<
					node.a[2] << "," <<
					node.a[3] << "," <<
					node.a[4] << "," <<
					(node.sum_A()) << "]," << std::endl;
			}

			std::cout << "};" << std::endl;
			std::cout << std::endl;

			// Print usage example
			std::cout << "// Usage in JavaScript:" << std::endl;
			std::cout << "// const key = `${p1_hp},${p2_hp},${p1_rage},${p2_rage}`;" << std::endl;
			std::cout << "// const [A1,A2,A3,A4,A5,SUM] = ANTICIPATION_TABLE[key] || [0,0,0,0,0,0];" << std::endl;
		};
	printAnticipationTableForJS();

	return 0;
}
int optimal_two_turn_game_program()
{

	return 0;
}
int experiment_hpgame_rage_find_optimal_crit_per_config()
{
	using namespace hpgame_rage;
	printf("[hpgame_rage] Testing ALL possible rage configurations...\n\n");

	// Define parameter ranges
	const float min_crit = 0.05f;         // 5%
	const float max_crit = 0.40f;         // 40%
	const float crit_step = 0.01f;        // 1% steps

	// Generate all possible configurations systematically
	std::vector<Config> all_configs;

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
					Config config;
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
		Config config;
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

		for (float crit = min_crit; crit <= max_crit; crit += crit_step)
		{
			Config config = base_config;
			config.critical_chance = crit;

			auto initial_state = Game::initial_state();
			auto analysis = game::analyze<Game>(initial_state, Game::compute_intrinsic_desire, config, 5);

			double sum_score = analysis.game_design_score;
			if (sum_score > best_score)
			{
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

	auto best_config = best_result.config;
	auto initial_state = Game::initial_state();
	auto final_analysis = game::analyze<Game>(initial_state, Game::compute_intrinsic_desire, best_config);

	printf("\nMost engaging moments with best configuration:\n");
	dump_most_fun_moments<Game>(final_analysis);

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
		hpgame_rage_optimized,

		optimal_two_turn_game,

	};

	//return hpgame_rage_find_optimal_critchance();
	//return experiment_hpgame_rage_find_optimal_crit_per_config();

	switch (hpgame_rage_optimize_critchance) // change this to run different programs
	{
	case rock_paper_scissors: return rps_program();
	case cointoss: return cointoss_program();
	case hpgame: return hpgame_program();
	case hpgame_interactive: return hpgame_interactive_program();
	case hpgame_rage: return hpgame_rage_analyze_program();
	case hpgame_rage_optimize_critchance: return hpgame_rage_find_optimal_critchance();
	case hpgame_rage_compare_mechanics: return hpgame_rage_compare_mechanics_program();
	case hpgame_rage_optimized: return hpgame_rage_optimized_program();
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
