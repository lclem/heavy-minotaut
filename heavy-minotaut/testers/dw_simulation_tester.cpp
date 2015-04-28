
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file defining a collection of functions that perform tests   *
 *  to minimization techniques based on the computation of the lookahead downward   *
 *  simulation. At the end of each minimization, a consistency check is performed   *
 *  (a check that the language has been preserved).                                 *
 * 																					*
 ************************************************************************************/

#include "dw_simulation_tester.hh"

string filename_Gl;

static const vector<string> DIRS = {
                            // "test automata//small_timbuk//" ,
                             "test automata//moderate_artmc_timbuk//" ,
                            // "test automata//forester_redblack_timbuk//32843200//" ,
                            // "test automata//forester_redblack_timbuk//33465936//" ,
                            // "test automata//forester_redblack_timbuk//33559760//" ,
                            // "test automata//forester_redblack_timbuk//33578272//" ,
                            // "test automata//forester_redblack_timbuk//33636192//" ,
                            // "test automata//artmc_timbuk//"
                            };


static void printSummary_DifferentLookaheads(bool result_consCheck, unsigned int max_la,
                                             float lvata_time, MetaData* metadata, TestData* testData,
                                             Timeout* timeout) {
    std::cout << "\nSummary: For a total of " << metadata->total << " automata and for each la=1.." << max_la
              << ", I applied the following Procedure: "
              << "computation of the maximal dw-la sim followed by a Reduction method of quotienting "
              << "with dw-la sim + pruning with P(id,dw-la sim). \n"
              << "The automata had, on average, " << metadata->initial_avg_q << " states, "
              << metadata->initial_avg_delta << " transitions, " << metadata->initial_avg_sigma << " symbols, "
              << metadata->initial_avg_ranking << " of symbol ranking and " << metadata->initial_avg_dens
              << " of transition density. "
              << "The greatest number of states, number of transitions, number of symbols, symbol ranking and "
              << "transition density observed were, respectively, "
              << metadata->initial_greatest_q << ", " << metadata->initial_greatest_delta << ", "
              << metadata->initial_greatest_sigma << ", " << metadata->initial_greatest_ranking << ", "
              << metadata->initial_greatest_dens << ". "
              << "On average, for each automaton, " << (metadata->initial_avg_transOverlap).at(0)
              << "% of the transitions overlap with some other transition (i.e., at least one of their children states is also part, at the same position, of another transition with the same parent and symbol). On average, for each transition that overlaps, the percentage of children that are shared with a different transition with the same parent and symbol is "
              << (metadata->initial_avg_transOverlap).at(1)
              << "%. And, on average, for each transition that overlaps, each of its children states appears, at that same position, in "
              << (metadata->initial_avg_transOverlap).at(2) << " additional transitions with the same parent and symbol. In absolute terms, "
              << (metadata->initial_greatest_transOverlap).at(0) << ", "
              << (metadata->initial_greatest_transOverlap).at(1) << " and "
              << (metadata->initial_greatest_transOverlap).at(2) << " were the greatest measures found. \n";
              for (unsigned int la=0; la<max_la; la++) {
                std::cout << " - Results for la=" << la+1 << ":\n"
                << (timeout[la].total) << "(" << ((float) timeout[la].total / (float) metadata->total)*100
                << "%) of the tests were aborted due to timeout (" << TIMEOUT/60 << "min). "
                << "The following results are relative to those " << testData[la].total << " tests which were not aborted: \n"
                << "On average, the dw-" << la+1 << " simulation contained "
                << testData[la].avg_sizes_relations.at(0) << " pairs and took "
                << testData[la].avg_numbs_refinements.at(0) << " refinements and "
                << testData[la].avg_times_relations.at(0) << " seconds to compute ";
                if (la==0)
                    std::cout << "(libvata took, on average, " << lvata_time << " sec to compute it)";
                std::cout << ". \nOn average, the quotienting took " << testData[la].avg_times_quotientings.at(0) << " sec, "
                << "the pruning took " << testData[la].avg_times_prunings.at(0) << " sec and the consistency check took "
                << testData[la].avg_time_consCheck << " sec. \n"
                << "Reducing the automata had as result that, "
                << "on average, the number of states was reduced to " << testData[la].avg_q_reduction
                << "%, the number of transitions was reduced to " << testData[la].avg_delta_reduction
                << "% and the transition density at the end was " <<testData[la].avg_transDens_reduction<<"% of the initial value."
                << " The greatest reductions in terms of states, transitions and transition density were, respectively, "
                << testData[la].greatest_q_reduction << "%, " << testData[la].greatest_delta_reduction << "% and "
                << testData[la].greatest_transDens_reduction << "%. \n";
                if (timeout[la].total > 0)
                    std::cout << "Timeout data: The smallest number of states in an automaton that generated a timeout was of "
                          << timeout[la].smallest_q << ", the smallest number of transitions was " << timeout[la].smallest_delta
                          << " and the smallest transition density was " << timeout[la].smallest_dens << ".\n";
              }
              if (result_consCheck)
                  std::cout << "\nAll consistency checks performed in this test were successful. \n";
              else
                  std::cout << "\nNOT all consistency checks performed in this test were successful! \n";

    delete metadata;

}

/* For each automaton, tries computing dw simulation with different lookaheads. In case of success, computes
 * the corresponding quotiented and pruned automaton and checks if this is equivalent to the initial automaton
 * (consistency check). */
void CheckDifferentLookaheads_3VL(unsigned int max_la) {

    auto start = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type.
    float time_sec;

    vector<filename> filenames = getAllFilenames(DIRS);
    const unsigned int NUMB_TESTS = filenames.size();

    MetaData* metaData = new MetaData();      // Stores data about the input automata.
    float lvata_time = 0;                     // Stores the avg time taken by libvata to compute ordinary dw simulations.

    for (unsigned int i=0; i<NUMB_TESTS; i++) {

        /* Automata parsing and data recording. */
        string autStr = VATA::Util::ReadFile(filenames.at(i));

        filename_Gl = filenames.at(i);
        stateDict stateDict;
        Automaton aut = parseFromString(autStr,stateDict,false,false);
        const unsigned int numb_states_ = getNumbUsedStates(aut);
        const unsigned int numb_transitions = getNumbTransitions(aut);
        const unsigned int numb_symbols_ = getNumbSymbols(aut);
        const state greatest_state = getGreatestUsedState(aut);
        const symbol greatest_symbol = getGreatestUsedSymbol(aut);
        vector<typerank> ranks = getRanks(aut, greatest_symbol+1);
        const float avg_ranking = getAvgRank(aut);
        const float transDens = getTransitionDensity(aut);
        vector<float> transOverlaps = measureTransOverlaps(aut);
        metaData->inc();
        metaData->checkInitialGreatest(numb_states_, numb_transitions, numb_symbols_,
                                       avg_ranking, transDens, transOverlaps);
        metaData->updateInitialAvg(numb_states_, numb_transitions, numb_symbols_,
                                   avg_ranking, transDens, transOverlaps);

        std::cout << "Automaton " << i+1 << " - " << filenames.at(i) << ":\n";
        printAutData(aut, true);

        if (numb_transitions == 0)
        {
            std::cout << "This automaton has 0 transitions and therefore will be skipped. Please remove all empty automata from the sample before running this test, to make the measure values more meaningful. \n\n";
            continue;
        }

        /* Compute Libvata's ordinary dw simulation. */
        start = std::chrono::high_resolution_clock::now();
        stateDiscontBinaryRelation lv_sim = computeLVDwSimulation(aut, greatest_state+1);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        time_sec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
        unsigned int lv_sim_size = getSizeLVStateBinRelation(lv_sim, greatest_state+1);
        std::cout << "Libvata's ordinary simulation has " << lv_sim_size
                  << " pairs and took " << time_sec << " seconds to compute.\n";
        lvata_time = (lvata_time*((float) metaData->total-1) + time_sec) / (float) metaData->total;

        for (unsigned int la=1; la<=max_la; la++)
        {
            std::cout << "- Using la=" << la << ":\n";

            vector<bool> column(greatest_state+1);
            vector<vector<bool> > w_with_3vl(greatest_state+1,column);
            vector<vector<bool> > w_without_3vl = w_with_3vl;
            float refin_with_3VL, refin_without_3VL;

            // For the 3VL Debugging attempts:

            setOPT(THREE_VALUES_LOGIC, ON_V1);
            std::cout << " With 3VL:\n";
            bool timedOut_with_3VL = false;
            start = std::chrono::high_resolution_clock::now();
            try {
                refin_with_3VL = dw_simulation(aut,la,w_with_3vl,greatest_state+1,ranks);
            }
            catch (timeout_& e) {
                std::cout << "TIMEOUT: The test took longer than "
                          << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
                timedOut_with_3VL = true;
            }
            elapsed = std::chrono::high_resolution_clock::now() - start;
            float time_sim_with_3VL =
                    std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
            if (!timedOut_with_3VL)
            {
                std::cout << "This relation took " << time_sim_with_3VL
                          << " sec and " << refin_with_3VL
                          << " matrix refinements to compute and contains "
                          << getSizeOfRel(w_with_3vl,w_with_3vl.size())
                          << " pairs ";
                if (la==1) {
                    std::cout << "(";
                    if (getSizeOfRel(w_with_3vl,w_with_3vl.size()) == lv_sim_size)
                            std::cout << "(consistent with libvata's)";
                    else
                            std::cout << "(NOT consistent with libvata's)";
                    std::cout << ".";
                }
            }

            OPT[THREE_VALUES_LOGIC] = OFF;
            std::cout << "\n Without 3VL:\n";
            bool timedOut_without_3VL = false;
            start = std::chrono::high_resolution_clock::now();
            try
            {
                refin_without_3VL = dw_simulation(aut,la,w_without_3vl,greatest_state+1,ranks);
            }
            catch (timeout_& e) {
                std::cout << "TIMEOUT: The test without 3VL took longer than "
                          << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
                timedOut_without_3VL = true;
            }
            elapsed = std::chrono::high_resolution_clock::now() - start;
            float time_sim_without_3VL =
                    std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

            if (!timedOut_without_3VL)
            {
                std::cout << "This relation took " << time_sim_without_3VL
                          << " sec and " << refin_without_3VL
                          << " matrix refinements to compute and contains "
                          << getSizeOfRel(w_without_3vl,w_without_3vl.size())
                          << " pairs ";
                if (la==1) {
                    std::cout << "(";
                    if (getSizeOfRel(w_without_3vl,w_without_3vl.size()) == lv_sim_size)
                            std::cout << "consistent with libvata's";
                    else
                            std::cout << "NOT consistent with libvata's";
                    std::cout << ").";
                }
            }

            if (timedOut_with_3VL && timedOut_without_3VL)
                break;

            if (!timedOut_with_3VL && !timedOut_without_3VL && w_with_3vl != w_without_3vl )
            {
                exit_with_error("The computed relations are different!\n");
                return;
            }
            else if (!timedOut_with_3VL && !timedOut_without_3VL)
                std::cout << "\n The computed relations have " << getSizeOfRel(w_without_3vl, w_without_3vl.size())
                          << " pairs and are equal. ";

            vector<vector<bool> > dw_sim;
            if (timedOut_with_3VL)
                dw_sim = w_without_3vl;
            else
                dw_sim = w_with_3vl;

            start = std::chrono::high_resolution_clock::now();
            Automaton aut_quot =
                    quotientAutomaton(aut, transClosure(dw_sim,greatest_state+1), greatest_state+1);
            elapsed = std::chrono::high_resolution_clock::now() - start;
            float time_quot = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
            std::cout << "The quotienting took " << time_quot << " seconds, ";

            start = std::chrono::high_resolution_clock::now();
            Automaton aut_pruned = prune(aut_quot,generateIdRelation(greatest_state+1),dw_sim,true);
            elapsed = std::chrono::high_resolution_clock::now() - start;
            float time_prun = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
            std::cout << "the pruning took " << time_prun << " seconds and ";

            const unsigned int numb_states_pruned = getNumbUsedStates(aut_pruned);
            float states_red;
            if (numb_states_==0)
                states_red = 100;
            else
                states_red = ((float) numb_states_pruned / (float) numb_states_) * (float) 100;
            const unsigned int numb_transitions_pruned = getNumbTransitions(aut_pruned);
            float transitions_red;
            if (numb_transitions==0)
                transitions_red = 100;
            else
                transitions_red = ((float) numb_transitions_pruned / (float) numb_transitions) * (float) 100;

            const float transDens_pruned =
                    (float) numb_transitions_pruned / ((float) numb_states_pruned * (float) numb_symbols_);
            float transDens_red;
            if (transDens<=0)
                transDens_red = 100;
            else
                transDens_red = (transDens_pruned / transDens) * (float) 100;

            start = std::chrono::high_resolution_clock::now();
            bool result  = equiv(aut_pruned, aut);
            elapsed = std::chrono::high_resolution_clock::now() - start;
            float time_consCheck =
                    std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
            std::cout << "the consistency check took " << time_consCheck << " seconds.\n";

            if (result) {
               std::cout << "The reduction techniques preserved the language and the quotiented+pruned automaton has "
                         << states_red << "% of its states, " << transitions_red << "% of its transitions and "
                         << transDens_red << "% of its transition density. \n";
            }
            else {
               std::cout << "The reduction techniques did NOT preserve the language!\n";
            }
        }
        std::cout << endl;
    }

}


tuple<bool, unsigned int, float, MetaData*, TestData*, Timeout*>
    CheckDifferentLookaheads(unsigned int max_la) {

    auto start = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type.
    float time_sec;

    vector<filename> filenames = getAllFilenames(DIRS);
    const unsigned int NUMB_TESTS = filenames.size();

    MetaData* metaData = new MetaData();      // Stores data about the input automata.
    float lvata_time = 0;                     // Stores the avg time taken by libvata to compute ordinary dw simulations.
    TestData* testData = new TestData[max_la];   // Stores experimental data about the experiments for each la.
    for (unsigned int i=0; i<max_la; i++)
        testData[i].set(1,1,1);
    Timeout* timeout = new Timeout[max_la];      // Stores timeout data about the experiments for each la

    bool consistencySuccess = true;

    for (unsigned int i=0; i<NUMB_TESTS; i++) {

        /* Automata parsing and data recording. */
        string autStr = VATA::Util::ReadFile(filenames.at(i));

        filename_Gl = filenames.at(i);
        stateDict stateDict;
        Automaton aut = parseFromString(autStr,stateDict,false,false);
        const unsigned int numb_states_ = getNumbUsedStates(aut);
        const unsigned int numb_transitions = getNumbTransitions(aut);
        const unsigned int numb_symbols_ = getNumbSymbols(aut);
        const state greatest_state = getGreatestUsedState(aut);
        const symbol greatest_symbol = getGreatestUsedSymbol(aut);
        vector<typerank> ranks = getRanks(aut, greatest_symbol+1);
        const float avg_ranking = getAvgRank(aut);
        const float transDens = getTransitionDensity(aut);
        vector<float> transOverlaps = measureTransOverlaps(aut);
        metaData->inc();
        metaData->checkInitialGreatest(numb_states_, numb_transitions, numb_symbols_, avg_ranking, transDens, transOverlaps);
        metaData->updateInitialAvg(numb_states_, numb_transitions, numb_symbols_, avg_ranking, transDens, transOverlaps);

        std::cout << "Automaton " << i+1 << " - " << filenames.at(i) << ":\n";
        printAutData(aut, true);

        if (numb_transitions == 0) {
            std::cout << "This automaton has 0 transitions and therefore will be skipped. Please remove all empty automata from the sample before running this test, to make the measure values more meaningful. \n\n";
            continue;
        }

        /* Compute Libvata's ordinary dw simulation. */
        start = std::chrono::high_resolution_clock::now();
        stateDiscontBinaryRelation lv_sim = computeLVDwSimulation(aut, greatest_state+1);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        time_sec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
        unsigned int lv_sim_size = getSizeLVStateBinRelation(lv_sim, greatest_state+1);
        std::cout << "Libvata's ordinary simulation has " << lv_sim_size
                  << " pairs and took " << time_sec << " seconds to compute.\n";
        lvata_time =
                (lvata_time*((float) metaData->total-1) + time_sec) / (float) metaData->total;

        for (unsigned int la=1; la<=max_la; la++) {
            std::cout << "- Using la=" << la << ":\n";

            vector<bool> column(greatest_state+1);
            vector<vector<bool> > dw_sim(greatest_state+1,column);
            float refin;

            start = std::chrono::high_resolution_clock::now();
            try {
                refin = dw_simulation(aut,la,dw_sim,greatest_state+1,ranks);
            }
            catch (timeout_& e) {
                std::cout << "TIMEOUT: The test took longer than "
                          << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
                timeout[la-1].inc();
                timeout[la-1].checkSmallest(numb_states_, numb_transitions, transDens);
                la = max_la;
                continue;
            }
            elapsed = std::chrono::high_resolution_clock::now() - start;
            float time_sim = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
            unsigned int dw_sim_size = getSizeOfRel(dw_sim,greatest_state+1);
            std::cout << "\nThe dw-" << la << " simulation has " << dw_sim_size << " pairs ";

            if (la==1) {
                std::cout << "(";
                if (dw_sim_size == lv_sim_size)
                        std::cout << "consistent with libvata's";
                else
                        std::cout << "NOT consistent with libvata's";
                std::cout << ") ";
            }

            std::cout << "and its computation took " << refin
                      << " matrix-refinements and " << time_sim << " seconds, ";
            testData[la-1].inc();
            testData[la-1].updateAvgSizesRels({dw_sim_size});
            testData[la-1].updateAvgNumbsRefs({refin});

            start = std::chrono::high_resolution_clock::now();
            Automaton aut_quot
                    = quotientAutomaton(aut,transClosure(dw_sim,greatest_state+1),greatest_state+1);
            elapsed = std::chrono::high_resolution_clock::now() - start;
            float time_quot = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
            std::cout << "the quotienting took " << time_quot << " seconds, ";

            start = std::chrono::high_resolution_clock::now();
            Automaton aut_pruned = prune(aut_quot,generateIdRelation(greatest_state+1),dw_sim,true);
            elapsed = std::chrono::high_resolution_clock::now() - start;
            float time_prun = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
            std::cout << "the pruning took " << time_prun << " seconds and ";

            const unsigned int numb_states_pruned = getNumbUsedStates(aut_pruned);
            float states_red;
            if (numb_states_==0)
                states_red = 100;
            else
                states_red = ((float) numb_states_pruned / (float) numb_states_) * (float) 100;
            const unsigned int numb_transitions_pruned = getNumbTransitions(aut_pruned);
            float transitions_red;
            if (numb_transitions==0)
                transitions_red = 100;
            else
                transitions_red = ((float) numb_transitions_pruned / (float) numb_transitions) * (float) 100;

            const float transDens_pruned =
                    (float) numb_transitions_pruned / ((float) numb_states_pruned * (float) numb_symbols_);
            float transDens_red;
            if (transDens<=0)
                transDens_red = 100;
            else
                transDens_red = (transDens_pruned / transDens) * (float) 100;

            start = std::chrono::high_resolution_clock::now();
            bool result = equiv(aut_pruned, aut);
            elapsed = std::chrono::high_resolution_clock::now() - start;
            float time_consCheck =
                    std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
            std::cout << "the consistency check took " << time_consCheck << " seconds.\n";

            if (result) {
               std::cout << "The reduction techniques preserved the language and the quotiented+pruned automaton has "
                         << states_red << "% of its states, " << transitions_red
                         << "% of its transitions and "
                         << transDens_red << "% of its transition density. \n";
            }
            else {
               std::cout << "The reduction techniques did NOT preserve the language!\n";
               consistencySuccess = false;
            }

            testData[la-1].updateAvgTimes({time_sim}, {time_quot}, {time_prun}, {time_consCheck});
            testData[la-1].checkGreatestReductions(states_red, transitions_red, transDens_red);
            testData[la-1].updateAvgReductions(states_red, transitions_red, transDens_red);

        }
        std::cout << endl;

    }

    printSummary_DifferentLookaheads(consistencySuccess, max_la, lvata_time, metaData, testData, timeout);

    return std::make_tuple(consistencySuccess, max_la, lvata_time, metaData, testData, timeout);
}


void dw_simulation_tester() {

    CheckDifferentLookaheads(4);

}
