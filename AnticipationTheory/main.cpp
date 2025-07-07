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
	printf("Game design score: %f\n\n\n", analysis.game_design_score);
	puts("Note: Rock-Paper-Scissors serves as a benchmark for single-turn games with perfect information.\n"
		"  1. Achieves A1 = 0.471, which is 94.2% of the theoretical maximum (0.5).\n"
		"  2. Shows zero A2-A5 components, confirming no multi-turn anticipation exists.\n"
		"  3. Validates our theoretical bounds for single-step binary games.\n"
		"  4. Demonstrates that classic games can approach mathematical optimality.\n"
	);	return 0;
}
int cointoss_program()
{
	auto analysis = analyze<coin_toss_game::Game>();
	dump_most_fun_moments<coin_toss_game::Game>(analysis);
	printf("Game design score: %f\n\n\n", analysis.game_design_score);
	puts("Note: Coin toss represents the theoretical optimum for single-turn binary games.\n"
		"  1. Achieves perfect A1 = 0.5, the mathematical maximum for this game class.\n"
		"  2. Proves our anticipation bounds through actual implementation.\n"
		"  3. Establishes the baseline for comparing all single-turn game designs.\n"
		"  4. Confirms that 50% win probability maximizes player anticipation.\n"
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

	printf("Game design score: %f\n", analysis.game_design_score);
	printf("Game design score(simulated): %f\n", game::compute_gamedesign_score_simulation<Game>(
		[&](const State& s)
		{
			return analysis.stateNodes[s].sum_A();
		}, config));

	puts("Note: HpGame serves as a benchmark for 1v1 action-focused games.\n"
		"  1. Most engaging states occur when both players have low HP, least engaging with large HP gaps.\n"
		"  2. High A1 with low A2-A5 components, confirming immediate tactical focus over strategic depth.\n"
		"  3. Validates intuitive game design principles through quantitative measurement.\n"
		"  4. Play the live demo here: https://akalouis.github.io/anticipation-theory/Html/hpgame.html\n"
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
				printf("%lld: %s\n", i, Game::tostr(transitions[i].alias).c_str());

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

	puts("Note: HpGame Rage demonstrates novel rage/critical hit mechanics.\n"
		"  1. Rage accumulates when dealing or receiving damage, enabling strategic depth.\n"
		"  2. Critical hits deal additional damage scaled by accumulated rage.\n"
		"  3. Shows higher A2-A5 components, indicating long-term strategic anticipation.\n"
	);
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

	puts("Note: `hpgame_rage_optimized` uses parameters algorithmically optimized for Game Design Score (GDS).\n"
		"  1. Its GDS is over 50% higher than the `hpgame` baseline.\n"
		"  2. The optimization was purely data-driven, no subjective parameter tuning was made.\n"
		"  3. This is the top-performing `HpGame` variant analyzed so far (as of Jul 2025).\n"
		"  4. If you find this game more engaging than `hpgame`, it serves as strong empirical evidence for our GDS metric and the Theory of Anticipation (ToA).\n"
		"  5. Play the live demo here: https://akalouis.github.io/anticipation-theory/Html/hpgame_rage_optimized.html\n"
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
	//printAnticipationTableForJS();

	return 0;
}
int optimal_two_turn_game_program()
{

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

	switch (hpgame_rage_optimized) // Change this to run different programs
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
	}
}