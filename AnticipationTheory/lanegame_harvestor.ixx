module;
export module lanegame_harvester;

import "pch.h";
import game;
import hpgame_v1;

export namespace lanegame_harvester
{
    struct Transition;

    struct State
    {
        int hp1;                // Player 1 HP
        int hp2;                // Player 2 HP
        int gold1;              // Player 1 gold
        int gold2;              // Player 2 gold
        int respawn1;           // Respawn counter for player 1 (counts down)
        int respawn2;           // Respawn counter for player 2 (counts down)
        int minions_remaining;  // Total minions remaining (16 -> 0)

        auto operator <=> (const State&) const = default;

        bool is_terminal() const { return minions_remaining <= 0; }
        std::vector<Transition> transitions() const;
        static State initial() { return State{ 5, 5, 0, 0, 0, 0, 8 }; }

        // Get the gold value for the current minion
        int current_minion_gold() const
        {
            int minion_number = 8 - minions_remaining + 1;

            // Pattern: 1, 2, 3, 4, 1, 2, 3, 4, ...
            //return ((minion_number - 1) % 4) + 1;
            return 1;
        }
    };

    struct Transition
    {
        float probability;
        State to;
    };

    std::vector<Transition> State::transitions() const
    {
        std::vector<Transition> result;

        // Terminal state check
        if (is_terminal())
            return result;

        // Get the gold value for the current minion
        int minion_gold = current_minion_gold();

        // Process next minion interaction
        State next_state = *this;
        next_state.minions_remaining--;

        // Handle respawn counters
        if (respawn1 > 0) next_state.respawn1--;
        if (respawn2 > 0) next_state.respawn2--;

        // Basic combat follows hpgame style - damaging each other
        if (respawn1 == 0 && respawn2 == 0)
        {
            // Both players active

            // Option 1: Player 1 gets gold, Player 2 takes damage
            State s1 = next_state;
            s1.gold1 += minion_gold;
            s1.hp2 -= 1;

            // Check for death
            if (s1.hp2 <= 0) {
                s1.hp2 = 5;
                s1.respawn2 = 3;
            }

            result.push_back({ 0.5f, s1 });

            // Option 2: Player 2 gets gold, Player 1 takes damage
            State s2 = next_state;
            s2.gold2 += minion_gold;
            s2.hp1 -= 1;

            // Check for death
            if (s2.hp1 <= 0) {
                s2.hp1 = 5;
                s2.respawn1 = 3;
            }

            result.push_back({ 0.5f, s2 });
        }
        else if (respawn1 == 0 && respawn2 > 0) {
            // Only player 1 active
            State s1 = next_state;
            s1.gold1 += minion_gold;  // Variable gold based on minion
            result.push_back({ 1.0f, s1 });
        }
        else if (respawn1 > 0 && respawn2 == 0) {
            // Only player 2 active
            State s1 = next_state;
            s1.gold2 += minion_gold;  // Variable gold based on minion
            result.push_back({ 1.0f, s1 });
        }
        else {
            // Both respawning, just advance state
            result.push_back({ 1.0f, next_state });
        }

        return result;
    }
}

export namespace game
{
    GameAnalysis<lanegame_harvester::State> analyze_lanegame_harvester()
    {
        return analyze<lanegame_harvester::State>([](const lanegame_harvester::State& state)
            {
                // Game is won if player 1 has more gold at the end
                if (state.is_terminal()) {
                    return state.gold1 > state.gold2 ? 1.0f : 0.0f;
                }
                return 0.0f;
            });
    }

    lanegame_harvester::State run_lanegame_harvester(const std::function<size_t(const lanegame_harvester::State&)> onChoice)
    {
        return run<lanegame_harvester::State>(onChoice);
    }

    template<typename state_t> void lanegame_harvester_dump_most_fun_moments(const std::vector<state_t>& states, std::map<state_t, StateNode>& stateNodes)
    {
        std::vector<state_t> states_sorted_by_a = states;
        std::sort(states_sorted_by_a.begin(), states_sorted_by_a.end(),
            [&](const state_t& a, const state_t& b)
            {
                return stateNodes[a].a > stateNodes[b].a;
            });

        printf("Most fun moments(sorted by A)\n");
        printf("P1.HP\tP2.HP\tP1.Gold\tP2.Gold\tP1.Resp\tP2.Resp\tMinions\tD_local\tD_global\tA\n");
        printf("-----\t-----\t-------\t-------\t-------\t-------\t-------\t-------\t--------\t-------\n");

        for (const auto& state : states_sorted_by_a)
        {
            auto& node = stateNodes[state];

            if (node.a > 0.0f)
                printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%.2f\n",
                    state.hp1, state.hp2,
                    state.gold1, state.gold2,
                    state.respawn1, state.respawn2,
                    state.minions_remaining,
                    node.d_local, node.d_global, node.a);

            // Only show top 25 states to keep output manageable
            if (&state - &states_sorted_by_a[0] >= 25) break;
        }
    }

    template<typename state_t> void lanegame_harvester_dump_most_fun_moments_a12345(
        const std::vector<state_t>& states,
        std::map<state_t, StateNode>& stateNodes,
        std::map<state_t, StateNode>& stateNodes2,
        std::map<state_t, StateNode>& stateNodes3,
        std::map<state_t, StateNode>& stateNodes4,
        std::map<state_t, StateNode>& stateNodes5)
    {
        std::vector<state_t> states_sorted_by_a12345 = states;
        std::sort(states_sorted_by_a12345.begin(), states_sorted_by_a12345.end(),
            [&](const state_t& a, const state_t& b)
            {
                return (stateNodes[a].a + stateNodes2[a].a + stateNodes3[a].a + stateNodes4[a].a + stateNodes5[a].a) >
                    (stateNodes[b].a + stateNodes2[b].a + stateNodes3[b].a + stateNodes4[b].a + stateNodes5[b].a);
            });

        printf("Most fun moments(sorted by A1+A2+A3+A4+A5)\n");
        printf("P1.HP\tP2.HP\tP1.Gold\tP2.Gold\tP1.Resp\tP2.Resp\tMinions\tD_loc\tD_glob\tA1\tA2\tA3\tA4\tA5\tA_total\n");
        printf("-----\t-----\t-------\t-------\t-------\t-------\t-------\t-----\t------\t---\t---\t---\t---\t---\t-------\n");

        for (const auto& state : states_sorted_by_a12345)
        {
            auto& node = stateNodes[state];
            auto& node2 = stateNodes2[state];
            auto& node3 = stateNodes3[state];
            auto& node4 = stateNodes4[state];
            auto& node5 = stateNodes5[state];

            float a_total = node.a + node2.a + node3.a + node4.a + node5.a;

            if (node.a > 0.0f)
                printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",
                    state.hp1, state.hp2,
                    state.gold1, state.gold2,
                    state.respawn1, state.respawn2,
                    state.minions_remaining,
                    node.d_local, node.d_global,
                    node.a, node2.a, node3.a, node4.a, node5.a,
                    a_total);

            // Only show top 25 states to keep output manageable
            if (&state - &states_sorted_by_a12345[0] >= 25) break;
        }
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

        printf("Game design score(A1 only): %f\n", analysis.game_design_score);
        printf("Game design score(sum(A1~5)): %f\n", analysis.game_design_score +
            analysis2.game_design_score +
            analysis3.game_design_score +
            analysis4.game_design_score +
            analysis5.game_design_score);

        return 0;
    }
}