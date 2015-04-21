
#include "dw_simulation_tester.hh"

string filename_Gl;

static const vector<string> DIRS = {
                            // "test automata//small_timbuk//" ,
                             "test automata//moderate_artmc_timbuk//" // ,
                            // "test automata//forester_redblack_timbuk//32843200//" ,
                            // "test automata//forester_redblack_timbuk//33465936//" ,
                            // "test automata//forester_redblack_timbuk//33559760//" ,
                            // "test automata//forester_redblack_timbuk//33578272//" ,
                            // "test automata//forester_redblack_timbuk//33636192//"  //,
                            // "test automata//artmc_timbuk//" ,
                            // "test automata//yun_yun_mata//" ,
                            // "test automata//nfas_from_richard_timbuk//" ,
                            // "test automata//nfas_from_richard_without_start_timbuk//" ,
                            // "test automata//random_tabakov_vardi_timbuk//small_sample//"
                            //   "test automata//random_tabakov_vardi_timbuk//small_sample_mail//"
                                  };

 // static vector<const char*>DIRS = {"test automata//debugging_tests//"};
 // static vector<const char*>DIRS = {"test automata/small automata/testing downward simulation/la=2//"};
 // static vector<const char*>DIRS = {"test automata/small automata/testing 3VL//"};
 // static vector<const char*>DIRS = {"test automata/error_trigging_automata/"};
 // static vector<const char*>DIRS = {"test automata/artmc_selection_to_minimize_in_two_steps/"};

static void printSummary(bool result, unsigned int failure, unsigned int NUMB_TESTS, unsigned int la, Success* success, Reduction* reduction, Timeout* timeout) {
    std::cout << "\nSummary: I tried to apply the following Procedure to a total of " << NUMB_TESTS << " automata: "
              << "computation of the maximal dw-" << la << " sim followed by a Reduction method of quotienting "
              << "with dw-" << la << " sim + pruning with P(id,dw-" << la << " sim). "
              << "For " << timeout->total << " (" << ((float) timeout->total / (float) NUMB_TESTS)*100
              << "%) of those automata, the Procedure was aborted due to a 'timeout' (" << TIMEOUT/60 << "min) "
              << "in the computation of the relation. ";
    if (result)
        std::cout << "In all of the other " << success->total << " (" << ((float) success->total / (float) NUMB_TESTS)*100
                  << "%) tests, the Procedure was successful, i.e., the Reduction (effective or not) preserved the language.\n";
    else
        std::cout << "In " << failure << "(" << ((float) failure / (float) NUMB_TESTS)*100
                  << "%) of the tests, the Reduction did NOT preserve the language.\n";
    std::cout << "The following results are relative to the automata for which the Reduction method successfully preserved the language:\n"
              << " - prior to the Reduction, the automata had, on average, " << success->initial_avg_q << " states, "
              << success->initial_avg_delta << " transitions, " << success->initial_avg_sigma << " symbols, average rank per symbol of "
              << success->initial_avg_ranking << " and transition density of " << success->initial_avg_dens << ". "
              << success->initial_greatest_q << ", " << success->initial_greatest_delta << ", " << success->initial_greatest_sigma << ", "
              << success->initial_greatest_ranking << " and " << success->initial_greatest_dens
              << " were the greatest number of states, number of transitions, number of symbols, average rank per symbol and transition density observed;\n"
              << " - the computed relation contained, on average, " << success->avg_sizes_relations[0] << " pairs of states (including pairs with the special initial state ψ);\n"
              << " - the computation of the relation took, on average, " << success->avg_numbs_refinements[0] << " matrix-refinements and " << success->avg_times_relations[0]
              << " sec per automaton. The quotienting took, on average, "
              << success->avg_times_quotientings[0] << " sec, the pruning took " << success->avg_times_prunings[0] << " sec and the consistency check took "
              << success->avg_time_consCheck << " sec.\n"
              << " - " << ((float) reduction->total / (float) success->total) * (float) 100 << "% "
              << "of the automata effectively became smaller (smaller number of states or smaller number of transitions) after applying the Reduction method;\n"
              << " - on those automata that effectively became smaller, after the reduction the automata had, on average, "
              << reduction->avg_q_reduction << "% of its states and " << reduction->avg_delta_reduction << "% of its transitions;\n"
              << " - in terms of states, the most significant reduction happened for an automaton whose number of states dropped to "
              << reduction->greatest_q_reduction << "%;\n"
              << " - in terms of transitions, the most significant reduction happened for an automaton whose number of transitions dropped to "
              << reduction->greatest_delta_reduction << "%;\n";
    if (timeout->total > 0)
              std::cout << "Additional data: The smallest number of states in an automaton that generated a 'timeout' was of "
              << timeout->smallest_q << ", the smallest number of transitions was " << timeout->smallest_delta
              << " and the smallest transition density was " << timeout->smallest_dens << ".\n";

    delete success;
    delete reduction;
    delete timeout;
}

static void printSummary_DifferentLookaheads(bool result_consCheck, unsigned int max_la, float lvata_time, MetaData* metadata, TestData* testData, Timeout* timeout) {
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
              //delete testData;
              //delete timeout;
   /* for (unsigned int i=0; i<max_la; i++) {
        delete testData[i];
        delete timeout[i];
    }*/

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

    MetaData* metaData = new MetaData();  // Stores data about the input automata.
    float lvata_time = 0;                     // Stores the avg time taken by libvata to compute ordinary dw simulations.
    //TestData* testData = new TestData[max_la];      // Stores experimental data about the experiments for each la.
    //for (unsigned int i=0; i<max_la; i++)
        //testData[i].set(1,1,1);
    //Timeout* timeout = new Timeout[max_la];         // Stores timeout data about the experiments for each la

    //bool consistencySuccess = true;

    for (unsigned int i=0; i<NUMB_TESTS; i++) {

        /* Automata parsing and data recording. */
        string autStr = VATA::Util::ReadFile(filenames.at(i));

        filename_Gl = filenames.at(i);
        stateDict stateDict;
        /*tuple<*/Automaton/*,unsigned int, unsigned int> parsing_tuple*/ aut = parseFromString(autStr,stateDict,false,false);
        //Automaton aut = std::get<0>(parsing_tuple);
        const unsigned int numb_states_ = /*std::get<1>(parsing_tuple);*/ getNumbUsedStates(aut);   // What about the unreachable states?
        const unsigned int numb_transitions = getNumbTransitions(aut);
        const unsigned int numb_symbols_ = /*std::get<2>(parsing_tuple);*/ getNumbSymbols(aut);
        const state greatest_state = getGreatestUsedState(aut);
        const symbol greatest_symbol = getGreatestUsedSymbol(aut);
        vector<typerank> ranks = getRanks(aut, /*numb_symbols*/ greatest_symbol+1);
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

        //printStateBinRelation(lv_sim, numb_states, stateDict);

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
                //timeout[la-1].inc();
                //timeout[la-1].checkSmallest(numb_states, getNumbTransitions(aut), getTransitionDensity(aut));
                timedOut_with_3VL = true;
            }
            elapsed = std::chrono::high_resolution_clock::now() - start;
            float time_sim_with_3VL = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
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
            float time_sim_without_3VL = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

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

            //printW(w_without_3vl,numb_states,stateDict); std::cout << "\n ";

            if (!timedOut_with_3VL && !timedOut_without_3VL
                && /*getSizeOfRel(w_with_3vl, w_with_3vl.size()) != getSizeOfRel(w_without_3vl, w_without_3vl.size())*/
                    w_with_3vl != w_without_3vl )
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

            /*unsigned int dw_sim_size = getSizeOfRel(dw_sim,numb_states);
            std::cout << "\nThe dw-" << la << " simulation has " << dw_sim_size << " pairs ";*/

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

            const float transDens_pruned = (float) numb_transitions_pruned / ((float) numb_states_pruned * (float) numb_symbols_) /*getTransitionDensity(aut_pruned)*/;
            float transDens_red;
            if (transDens<=0)
                transDens_red = 100;
            else
                transDens_red = (transDens_pruned / transDens) * (float) 100;

            start = std::chrono::high_resolution_clock::now();
            bool result  = equiv(aut_pruned, aut);
            //bool result2 = VATA::ExplicitTreeAut::CheckInclusion(aut, aut_pruned);
            elapsed = std::chrono::high_resolution_clock::now() - start;
            float time_consCheck = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
            std::cout << "the consistency check took " << time_consCheck << " seconds.\n";

            if (result/*1 && result2*/) {
               std::cout << "The reduction techniques preserved the language and the quotiented+pruned automaton has "
                         << states_red << "% of its states, " << transitions_red << "% of its transitions and "
                         << transDens_red << "% of its transition density. \n";
            }
            else {
               std::cout << "The reduction techniques did NOT preserve the language!\n";
               //consistencySuccess = false;
            }

            //testData[la-1].updateAvgTimes({time_sim0}, {time_quot}, {time_prun}, {time_consCheck});
            //testData[la-1].checkGreatestReductions(states_red, transitions_red, transDens_red);
            //testData[la-1].updateAvgReductions(states_red, transitions_red, transDens_red);

        }
        std::cout << endl;

    }

    //printSummary_DifferentLookaheads(consistencySuccess, max_la, lvata_time, metaData, testData, timeout);

}


tuple<bool, unsigned int, float, MetaData*, TestData*, Timeout*>
    CheckDifferentLookaheads(unsigned int max_la) {

    auto start = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type.
    float time_sec;

    vector<filename> filenames = getAllFilenames(DIRS);
    const unsigned int NUMB_TESTS = filenames.size();

    MetaData* metaData = new MetaData();  // Stores data about the input automata.
    float lvata_time = 0;                     // Stores the avg time taken by libvata to compute ordinary dw simulations.
    TestData* testData = new TestData[max_la];      // Stores experimental data about the experiments for each la.
    for (unsigned int i=0; i<max_la; i++)
        testData[i].set(1,1,1);
    Timeout* timeout = new Timeout[max_la];         // Stores timeout data about the experiments for each la

    bool consistencySuccess = true;

    for (unsigned int i=0; i<NUMB_TESTS; i++) {

        /* Automata parsing and data recording. */
        string autStr = VATA::Util::ReadFile(filenames.at(i));

        filename_Gl = filenames.at(i);
        stateDict stateDict;
        /*tuple<*/Automaton/*,unsigned int, unsigned int> parsing_tuple*/ aut = parseFromString(autStr,stateDict,false,false);
        //Automaton aut = std::get<0>(parsing_tuple);
        const unsigned int numb_states_ = /*std::get<1>(parsing_tuple)*/ getNumbUsedStates(aut);   // What about the unreachable states?
        const unsigned int numb_transitions = getNumbTransitions(aut);
        const unsigned int numb_symbols_ = /*std::get<2>(parsing_tuple)*/ getNumbSymbols(aut);
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
            /*pair<vector<vector<bool> >,*/ float/*>*/ refin;

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
            //dw_sim = answer.first;
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

            const float transDens_pruned = (float) numb_transitions_pruned / ((float) numb_states_pruned * (float) numb_symbols_) /*getTransitionDensity(aut_pruned)*/;
            float transDens_red;
            if (transDens<=0)
                transDens_red = 100;
            else
                transDens_red = (transDens_pruned / transDens) * (float) 100;

            start = std::chrono::high_resolution_clock::now();
            bool result = equiv(aut_pruned, aut);
            //bool result2 = VATA::ExplicitTreeAut::CheckInclusion(aut, aut_pruned);
            elapsed = std::chrono::high_resolution_clock::now() - start;
            float time_consCheck = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
            std::cout << "the consistency check took " << time_consCheck << " seconds.\n";

            if (result/*1 && result2*/) {
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


/* A Test function that focus essentially on checking if the lookahead dw simulation algorithm presented
   in this project is consistent. This is done by computing a la-simulation S for a given automaton,
   quotienting it with S, pruning it with P(id,S) and then checking if the resulting automaton is
   equivalent to the initial one (which is done using libvata's algorithm for lang. inclusion in
   both directions). */
static void CheckConsistency(unsigned int la) {
    auto start = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type
    //float time_sec;

    bool result = true;
    vector<filename> filenames = getAllFilenames(DIRS);
    const unsigned int NUMB_TESTS = filenames.size();
    Success* success = new Success(1,1,1);
    unsigned int failure = 0;
    Reduction* reduction = new Reduction();
    Timeout* timeout = new Timeout();

    for (unsigned int i=0; i<NUMB_TESTS; i++) {
        std::cout << "  Test " << i+1 << " - " << filenames.at(i) << ":\n";
        string autStr = VATA::Util::ReadFile(filenames.at(i));
        filename_Gl = filenames.at(i);

        //Create the dictionary for translating state names to internal state numbers.
        stateDict stateDict;
        /*tuple<*/Automaton/*,unsigned int, unsigned int> parsing_tuple*/ aut = parseFromString(autStr,stateDict,false,false);
        //Automaton aut = std::get<0>(parsing_tuple);
        const unsigned int numb_states_ = /*std::get<1>(parsing_tuple)*/ getNumbUsedStates(aut);   // What about the unreachable states?
        const unsigned int numb_transitions = getNumbTransitions(aut);
        const unsigned int numb_symbols_ = /*std::get<2>(parsing_tuple)*/ getNumbSymbols(aut);
        const state greatest_state = getGreatestUsedState(aut);
        const symbol greatest_symbol = getGreatestUsedSymbol(aut);
        const float avg_ranking = getAvgRank(aut);
        const float initial_dens = getTransitionDensity(aut);

        printAutData(aut,true);

        vector<bool> column(/*numb_states*/greatest_state+1);
        vector<vector<bool> > dw_sim(/*numb_states*/ greatest_state+1,column);
        /*pair<vector<vector<bool> >,*/ float/*>*/ refin;
        vector<typerank> ranks = getRanks(aut,/*numb_symbols*/greatest_symbol+1);

        start = std::chrono::high_resolution_clock::now();
        try
        {
            refin = dw_simulation(aut, la, dw_sim, /*numb_states*/ greatest_state+1, ranks);
        }
        catch (timeout_& e) {
            std::cout << "TIMEOUT: The test took longer than "
                      << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
            timeout->inc();
            timeout->checkSmallest(numb_states_,
                                   getNumbTransitions(aut), getTransitionDensity(aut));
            continue;
        }
        elapsed = std::chrono::high_resolution_clock::now() - start;
        float time_relation = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

        //dw_sim = answer.first;
        std::cout << "The dw-" << la << " simulation has "
                  << getSizeOfRel(dw_sim, /*numb_states*/ greatest_state+1) << " and ";
        //printW(dw_sim,numb_states,stateDict); std::cout << ". ";
        std::cout << "took " << refin << " matrix-refinements to compute and "
                  << time_relation << " seconds. ";

        start = std::chrono::high_resolution_clock::now();
        Automaton aut_quot
                = quotientAutomaton(aut,transClosure(dw_sim,/*numb_states*/greatest_state+1),/*numb_states*/greatest_state+1);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        float time_quotienting = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
        std::cout << "\nThe quotienting took " << time_quotienting << " seconds, ";

        start = std::chrono::high_resolution_clock::now();
        Automaton aut_pruned =
                prune(aut_quot,generateIdRelation(/*numb_states*/greatest_state+1),dw_sim,true);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        float time_pruning = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
        std::cout << "the pruning took " << time_pruning << " seconds and ";

        const unsigned int numb_states_pruned = getNumbUsedStates(aut_pruned);
        float states_red;       // states_red = how many (%) of the initial states are still present after the reduction.
        if (numb_states_==0)
            states_red = 100;
        else
            states_red = ((float) numb_states_pruned / (float) numb_states_) * (float) 100;
        const unsigned int numb_transitions_pruned = getNumbTransitions(aut_pruned);
        float transitions_red;  // transitions_red = how many (%) of the initial transitions are still present after the reduction.
        if (numb_transitions==0)
            transitions_red = 100;
        else
            transitions_red = ((float) numb_transitions_pruned / (float) numb_transitions) * (float) 100;

        start = std::chrono::high_resolution_clock::now();
        //aut = removeInitialState(aut);
        //aut_pruned = removeInitialState(aut_pruned);
        bool result = equiv(aut_pruned, aut);
        //bool result2 = VATA::ExplicitTreeAut::CheckInclusion(aut, aut_pruned);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        float time_consCheck = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
        std::cout << "the consistency check took " << time_consCheck << " seconds.\n";

        if (result/*1 && result2*/) {
           std::cout << "The reduction techniques preserved the language and the quotiented+pruned automaton has " << std::to_string(states_red) << "% of its states, and " << std::to_string(transitions_red) << "% of its transitions.\n";
           success->inc();

           success->checkInitialGreatest(numb_states_, numb_transitions, numb_symbols_, avg_ranking, initial_dens);
           success->updateInitialAvg(numb_states_, numb_transitions, numb_symbols_, avg_ranking, initial_dens);
           success->updateAvgSizesRels({getSizeOfRel(dw_sim,/*numb_states*/greatest_state+1)});
           success->updateAvgNumbsRefs({refin});
           success->updateAvgTimes({time_relation}, {time_quotienting}, {time_pruning}, time_consCheck);

           if (numb_states_pruned<numb_states_ || numb_transitions_pruned<numb_transitions) {
               reduction->inc();
               reduction->checkGreatest(states_red,transitions_red);
               reduction->updateAvg(states_red,transitions_red);
           }
       }
       else {
           std::cout << "The reduction techniques did NOT preserve the language!\n";
           failure++;
           result = false;
       }

       std::cout << endl;

    }

    printSummary(result, failure, NUMB_TESTS, la, success, reduction, timeout);

}

//void minimizeInTwoSteps(unsigned int la1, unsigned int la2)
//{
//    vector<filename> filenames = getAllFilenames(DIRS);
//    const unsigned int NUMB_TESTS = filenames.size();

//    for (unsigned int i=0; i<NUMB_TESTS; i++)
//    {
//        std::cout << "  Test " << i+1 << " - " << filenames.at(i) << ":\n";
//        string autStr = VATA::Util::ReadFile(filenames.at(i));

//        stateDict stateDict;
//        tuple<Automaton,unsigned int, unsigned int> parsing_tuple = parseFromString(autStr,stateDict,false,false);
//        Automaton aut = std::get<0>(parsing_tuple);
//        unsigned int numb_states = std::get<1>(parsing_tuple);   // What about the unreachable states?
//        unsigned int numb_transitions = getNumbTransitions(aut);
//        const unsigned int numb_symbols = std::get<2>(parsing_tuple);
//        const float avg_ranking = getAvgRank(aut);

//        printAutData(aut,true);

//        // 1st Simulation Computation
//        std::cout << "Computing " << la1 << "-simulation... \n";
//        vector<bool> column(numb_states);
//        vector<vector<bool> > dw_sim(numb_states,column);
//        /*pair<vector<vector<bool> >,*/ float/*>*/ refin;
//        vector<typerank> ranks = getRanks(aut,numb_symbols);

//        auto start = std::chrono::high_resolution_clock::now();
//        try {
//            refin = dw_simulation(aut,la1,dw_sim,numb_states,ranks);
//        }
//        catch (timeout_& e) {
//            std::cout << "TIMEOUT: The test took longer than "
//                      << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
//            return;
//        }
//        auto elapsed = std::chrono::high_resolution_clock::now() - start;
//        float time_relation = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

//        //dw_sim = refin.first;
//        std::cout << "The dw-" << la1 << " simulation has " << getSizeOfRel(dw_sim,numb_states) << " and ";
//        //printW(dw_sim,numb_states,stateDict); std::cout << ". ";
//        std::cout << "took " << refin << " matrix-refinements to compute and "
//                  << time_relation << " seconds. ";

//        // 1st Reduction
//        start = std::chrono::high_resolution_clock::now();
//        Automaton aut_quot = quotientAutomaton(aut,transClosure(dw_sim,numb_states),numb_states);
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        float time_quotienting = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "\nThe quotienting took " << time_quotienting << " seconds, ";

//        start = std::chrono::high_resolution_clock::now();
//        Automaton aut_pruned_1 = prune(aut_quot,generateIdRelation(numb_states),dw_sim,true);
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        float time_pruning = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "the pruning took " << time_pruning << " seconds and ";

//        unsigned int numb_states_pruned = getNumbUsedStates(aut_pruned_1);
//        float states_red;       // states_red = how many (%) of the initial states are still present after the reduction.
//        if (numb_states==0)
//            states_red = 100;
//        else
//            states_red = ((float) numb_states_pruned / (float) numb_states) * (float) 100;
//        unsigned int numb_transitions_pruned = getNumbTransitions(aut_pruned_1);
//        float transitions_red;  // transitions_red = how many (%) of the initial transitions are still present after the reduction.
//        if (numb_transitions==0)
//            transitions_red = 100;
//        else
//            transitions_red = ((float) numb_transitions_pruned / (float) numb_transitions) * (float) 100;

//        std::cout << "the quotiented+pruned automaton has "
//                  << std::to_string(states_red) << "% of its states, and "
//                  << std::to_string(transitions_red) << "% of its transitions.\n";

//        //aut = reindexStates(aut);
//        //numb_states = getNumbUsedStates(aut);

//        // 2nd Simulation Computation
//        std::cout << "Computing " << la2 << "-simulation... \n";
//        numb_transitions = getNumbTransitions(aut_pruned_1);
//        unsigned int numb_states_pruned_1 = getNumbUsedStates(aut_pruned_1);
//        //column.resize(numb_states);
//        dw_sim.assign(numb_states,column);

//        try {
//            refin = dw_simulation(aut_pruned_1,la2,dw_sim,numb_states,ranks);
//        }
//        catch (timeout_& e) {
//            std::cout << "TIMEOUT: The test took longer than "
//                      << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
//            return;
//        }
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        time_relation = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

//        //dw_sim = answer.first;
//        std::cout << "The dw-" << la2 << " simulation has " << getSizeOfRel(dw_sim,numb_states) << " and ";
//        //printW(dw_sim,numb_states,stateDict); std::cout << ". ";
//        std::cout << "took " << refin << " matrix-refinements to compute and "
//                  << time_relation << " seconds. ";

//        // 2nd Reduction
//        start = std::chrono::high_resolution_clock::now();
//        aut_quot = quotientAutomaton(aut_pruned_1,transClosure(dw_sim,numb_states),numb_states);
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        time_quotienting = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "\nThe quotienting took " << time_quotienting << " seconds, ";

//        start = std::chrono::high_resolution_clock::now();
//        Automaton aut_pruned_2 = prune(aut_quot,generateIdRelation(numb_states),dw_sim,true);
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        time_pruning = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "the pruning took " << time_pruning << " seconds and ";

//        unsigned int numb_states_pruned_2 = getNumbUsedStates(aut_pruned_2);
//        if (numb_states_pruned_1==0)
//            states_red = 100;
//        else
//            states_red = ((float) numb_states_pruned_2 / (float) numb_states_pruned_1) * (float) 100;
//        numb_transitions_pruned = getNumbTransitions(aut_pruned_2);
//        if (numb_transitions==0)
//            transitions_red = 100;
//        else
//            transitions_red = ((float) numb_transitions_pruned / (float) numb_transitions) * (float) 100;

//        start = std::chrono::high_resolution_clock::now();
//        bool result1 = VATA::ExplicitTreeAut::CheckInclusion(aut_pruned_2, aut);
//        bool result2 = VATA::ExplicitTreeAut::CheckInclusion(aut, aut_pruned_2);
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        float time_consCheck = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "the consistency check took " << time_consCheck << " seconds.\n";

//        if (result1 && result2) {
//           std::cout << "The reduction techniques preserved the language and the quotiented+pruned automaton has " << std::to_string(states_red)
//                     << "% of its states, and " << std::to_string(transitions_red)
//                     << "% of its transitions.\n";
//       }
//       else {
//           std::cout << "The reduction techniques did NOT preserve the language!\n";
//       }
//    }
//}

void dw_simulation_tester() {
    //CheckConsistency(3);
    CheckDifferentLookaheads(4);

    //minimizeInTwoSteps(2,3);
}

