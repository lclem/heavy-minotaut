
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file defining a collection of functions that perform tests   *
 *  to minimization techniques based on the computation of both lookahead downward  *
 *  simulation and lookahead upward simulation. At the end of each minimization, a  *
 *  consistency check is performed (a check that the language has been preserved).  *
 * 																					*
 ************************************************************************************/

#include "minimizationTechniques_tester.hh"

unsigned int pr_depth = -1;

// A static global variable has its access restricted to the scope of the file.
static vector<string> DIRS = {
                           // "test automata//small_timbuk//",
                            "test automata//moderate_artmc_timbuk//" ,
                           //  "test automata//forester_redblack_timbuk//32843200//",
                           //  "test automata//forester_redblack_timbuk//33465936//",
                           //  "test automata//forester_redblack_timbuk//33559760//",
                           //  "test automata//forester_redblack_timbuk//33578272//",
                           //  "test automata//forester_redblack_timbuk//33636192//",
                           // "test automata//artmc_timbuk//"
                           };


static void printSummary_procedure(unsigned int la_dw, unsigned int la_up, MetaData& metadata,
                                   TestData& testData_ru,
                                   TestData& testData_quot,   Timeout& timeout_quot,
                                   TestData& testData_qAndP,  Timeout& timeout_qAndP,
                                   TestData& testData_heavy,  TestData& testData_heavy_rels,
                                   Timeout& timeout_heavy,    string log_humanread_filename = "")
{

    outputText("\nSummary results of applying the following Procedure to a battery of test automata: \n  1) Removing the useless states (i.e., states which cannot be reached starting from a leaf-rule or which cannot reach a final state); \n",
               log_humanread_filename);

    outputText("  2) Removing the useless states + Quotienting with dw-" + std::to_string(la_dw) + " sim.; \n"
             + "  3) Removing the useless states + Quotienting with dw-" + std::to_string(la_dw)
             + " sim. + Pruning with P(id, dw-" + std::to_string(la_dw) + " sim.); \n"
             + "  4) Removing the useless states + Applying the Heavy(" + std::to_string(la_dw)
             + ", " + std::to_string(la_up) + ") method, defined as a double loop using the following sequence of steps: \n"
             + "    4.1) computing the dw-" + std::to_string(la_dw) + " sim. + quotienting with it; \n"
             + "    4.2) pruning with P(id, strict dw-" + std::to_string(la_dw) + " sim.); \n"
             + "    4.3) removing the useless states; \n"
             + "    4.4) computing the up-" + std::to_string(la_up) + "(id) sim. + quotienting with it; \n"
             + "    4.5) computing the strict up-1(id) sim.; \n"
             + "    4.6) computing the dw-" + std::to_string(la_dw) + " sim. again; \n"
             + "    4.7) pruning with P(strict up-1(id) sim., dw-" + std::to_string(la_dw) + " sim.); \n"
             + "    4.8) removing the useless states; \n"
             + "    4.9) repetition of 4.4): computing the up-" + std::to_string(la_up)
             + "(id) sim. + quotienting with it; \n"
             + "    4.10) computing the dw-1 sim.; \n"
             + "    4.11) computing the up-" + std::to_string(la_up) + "(dw-1) sim.; \n"
             + "    4.12) pruning with P(up-" + std::to_string(la_up) + "(dw-1) sim., strict dw-1 sim.); \n"
             + "    4.13) removing the useless states; \n"

              + "We performed tests on a total of " + std::to_string(metadata.total) + " automata, with an average of "
                 + std::to_string(metadata.initial_avg_q) + " states, " + std::to_string(metadata.initial_avg_delta) + " transitions, "
                 + std::to_string(metadata.initial_avg_sigma) + " symbols, average rank per symbol of "
                 + std::to_string(metadata.initial_avg_ranking) + " and transition density of " + std::to_string(metadata.initial_avg_dens) + ". "
                 + std::to_string(metadata.initial_greatest_q) + ", " + std::to_string(metadata.initial_greatest_delta) + ", "
                 + std::to_string(metadata.initial_greatest_sigma) + ", " + std::to_string(metadata.initial_greatest_ranking) + " and "
                 + std::to_string(metadata.initial_greatest_dens) + " were the greatest number of states, number of transitions, number of symbols, average rank per symbol and transition density observed;\n"

                 + " -  When Removing the useless states, on average: \n"
                  + "    - the number of states at the end of each reduction was "
                  + std::to_string(testData_ru.avg_q_reduction) + "%, the number of transitions was "
                    + std::to_string(testData_ru.avg_delta_reduction) + "% and the transition density was "
                    + std::to_string(testData_ru.avg_transDens_reduction) + "%; \n"
                    + "    - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
                    + std::to_string(testData_ru.greatest_q_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
                    + std::to_string(testData_ru.greatest_delta_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of transition density happened for an automaton whose transition density at the end of the minimization was "
                    + std::to_string(testData_ru.greatest_transDens_reduction) + "%;\n"

                 + " - Quotienting with dw-" + std::to_string(la_dw) + " timed out in "
                  + std::to_string(timeout_quot.total)
                  + " (" + std::to_string(( (float) timeout_quot.total / (float) metadata.total ) * (float) 100) + "%) "
                  + "of the automata. On average: \n"
                  + "    - the number of states at the end of each reduction was "
                  + std::to_string(testData_quot.avg_q_reduction) + "%, the number of transitions was "
                    + std::to_string(testData_quot.avg_delta_reduction) + "% and the transition density was "
                    + std::to_string(testData_quot.avg_transDens_reduction) + "%; \n"
                    + "    - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
                    + std::to_string(testData_quot.greatest_q_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
                    + std::to_string(testData_quot.greatest_delta_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of transition density happened for an automaton whose transition density at the end of the minimization was "
                    + std::to_string(testData_quot.greatest_transDens_reduction) + "%;\n", log_humanread_filename);
        testData_quot.printReductionBuckets(log_humanread_filename);

        outputText(" - Quotienting with dw-" + std::to_string(la_dw)
                + " + Pruning with P(id, dw-" + std::to_string(la_dw) + ") timed out in "
                  + std::to_string(timeout_qAndP.total)
                  + " (" + std::to_string(( (float) timeout_qAndP.total / (float) metadata.total ) * (float) 100) + "%) "
                  + "of the automata. On average: \n"
                    + "    - the number of states at the end of each reduction was "
                    + std::to_string(testData_qAndP.avg_q_reduction) + "%, the number of transitions was "
                    + std::to_string(testData_qAndP.avg_delta_reduction) + "% and the transition density was "
                    + std::to_string(testData_qAndP.avg_transDens_reduction) + "%; \n"
                    + "    - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
                    + std::to_string(testData_qAndP.greatest_q_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
                    + std::to_string(testData_qAndP.greatest_delta_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of transition density happened for an automaton whose transition density at the end of the minimization was "
                    + std::to_string(testData_qAndP.greatest_transDens_reduction) + "%;\n",
                   log_humanread_filename);
        testData_qAndP.printReductionBuckets(log_humanread_filename);

        outputText(" - Heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up) + ") timed out in "
                   + std::to_string(timeout_heavy.total)
                   + " (" + std::to_string(( (float) timeout_heavy.total / (float) metadata.total ) * (float) 100) + "%) "
                   + "of the automata. On average: \n"
                     + "    - the number of states at the end of each reduction was "
                     + std::to_string(testData_heavy.avg_q_reduction) + "%, the number of transitions was "
                     + std::to_string(testData_heavy.avg_delta_reduction) + "% and the transition density was "
                     + std::to_string(testData_heavy.avg_transDens_reduction) + "%; \n"
                     + "    - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
                     + std::to_string(testData_heavy.greatest_q_reduction) + "%;\n"
                     + "    - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
                     + std::to_string(testData_heavy.greatest_delta_reduction) + "%;\n"
                     + "    - the most significant reduction in terms of transition density happened for an automaton whose transition density at the end of the minimization was "
                     + std::to_string(testData_heavy.greatest_transDens_reduction) + "%;\n"
                     + "  The following statistics are relative to the computations using the dw and up lookaheads given as parameters: \n"
                     + "    - the dw-" + std::to_string(la_dw) + " sim. computed contained "
                     + std::to_string(testData_heavy_rels.avg_sizes_relations[DW_LA_1ST_TIME])
                     + " pairs of states (including pairs with the special initial state ψ)"
                     + " and took " + std::to_string(testData_heavy_rels.avg_numbs_refinements[DW_LA_1ST_TIME])
                     + " refinements and " + std::to_string(testData_heavy_rels.avg_times_relations[DW_LA_1ST_TIME])
                     + " seconds to compute. \n"
                     + "    - the quotienting with dw-" + std::to_string(la_dw) + " sim. took "
                     + std::to_string(testData_heavy_rels.avg_times_quotientings[QUOT_WITH_DW_LA]) + " sec. \n"
                     + "    - the pruning with P(id, dw-" + std::to_string(la_dw) + " sim.) took "
                     + std::to_string(testData_heavy_rels.avg_times_prunings[PRUN_WITH_P_ID_AND_DW_LA]) + " sec. \n"
                     + "    - the up-" + std::to_string(la_up) + "(id) sim computed the first time contained "
                     + std::to_string(testData_heavy_rels.avg_sizes_relations[UP_LA_WITH_ID_1ST_TIME])
                     + " pairs of states and took "
                     + std::to_string(testData_heavy_rels.avg_numbs_refinements[UP_LA_WITH_ID_1ST_TIME])
                     + " refinements and " + std::to_string(testData_heavy_rels.avg_times_relations[UP_LA_WITH_ID_1ST_TIME])
                     + " seconds to compute. \n"
                     + "    - the first quotienting with up-" + std::to_string(la_up) + "(id) took "
                     + std::to_string(testData_heavy_rels.avg_times_quotientings[QUOT_WITH_UP_LA_WITH_ID_1ST]) + " sec. \n"
                     + "    - the strict up-1(id) sim contained "
                     + std::to_string(testData_heavy_rels.avg_sizes_relations[STRICT_UP_1_WITH_ID]) + " pairs of states"
                     + " and took " + std::to_string(testData_heavy_rels.avg_numbs_refinements[STRICT_UP_1_WITH_ID])
                     + " refinements and " + std::to_string(testData_heavy_rels.avg_times_relations[STRICT_UP_1_WITH_ID])
                     + " seconds to compute. \n"
                     + "    - the pruning with P(up-1(id), dw-" + std::to_string(la_dw) + ") took "
                     + std::to_string(testData_heavy_rels.avg_times_prunings[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA])
                     + " sec. \n"
                     + "    - the dw-" + std::to_string(la_dw) + " sim computed the second time contained, on average, "
                     + std::to_string(testData_heavy_rels.avg_sizes_relations[DW_LA_2ND_TIME])
                     + " pairs of states and took "
                     + std::to_string(testData_heavy_rels.avg_numbs_refinements[DW_LA_2ND_TIME])
                     + " refinements and " + std::to_string(testData_heavy_rels.avg_times_relations[DW_LA_2ND_TIME])
                     + " seconds to compute. \n"
                     + "    - the second quotienting with up-" + std::to_string(la_up) + "(id) took "
                     + std::to_string(testData_heavy_rels.avg_times_quotientings[QUOT_WITH_UP_LA_WITH_ID_2ND]) + " sec. \n"
                     + "    - the dw-1 sim contained " + std::to_string(testData_heavy_rels.avg_sizes_relations[DW_1])
                     + " pairs of states and took " + std::to_string(testData_heavy_rels.avg_times_relations[DW_1])
                     + " seconds to compute. \n"
                     + "    - the computed up-" + std::to_string(la_up) + "(dw-1 sim) sim contained "
                     + std::to_string(testData_heavy_rels.avg_sizes_relations[UP_LA_WITH_DW_1]) + " pairs of states"
                     + " and took " + std::to_string(testData_heavy_rels.avg_numbs_refinements[UP_LA_WITH_DW_1])
                     + " refinements and " + std::to_string(testData_heavy_rels.avg_times_relations[UP_LA_WITH_DW_1])
                     + " seconds to compute.\n"
                     + "    - the pruning with P(up-" + std::to_string(la_up) + "(dw-1), strict dw-1) took "
                     + std::to_string(testData_heavy_rels.avg_times_prunings[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1])
                     + " sec. \n",
                     log_humanread_filename);
         testData_heavy.printReductionBuckets(log_humanread_filename);

}

static void printSummary_quotienting(unsigned int la_dw, unsigned int la_up,
                                     MetaData& metadata, TestData& testData,
                                     Timeout& timeout, string log_humanread_filename = "")
{
    outputText("\nSummary results of applying a quotienting method to a battery of test automata using dw lookahead = "
               + std::to_string(la_dw) + " sim. and up lookahead = " + std::to_string(la_up) + ".\n"
              + "We performed tests on a total of " + std::to_string(metadata.total) + " automata, with an average of "
                 + std::to_string(metadata.initial_avg_q) + " states, " + std::to_string(metadata.initial_avg_delta) + " transitions, "
                 + std::to_string(metadata.initial_avg_sigma) + " symbols, average rank per symbol of "
                 + std::to_string(metadata.initial_avg_ranking) + " and transition density of " + std::to_string(metadata.initial_avg_dens) + ". "
                 + std::to_string(metadata.initial_greatest_q) + ", " + std::to_string(metadata.initial_greatest_delta) + ", "
                 + std::to_string(metadata.initial_greatest_sigma) + ", " + std::to_string(metadata.initial_greatest_ranking) + " and "
                 + std::to_string(metadata.initial_greatest_dens) + " were the greatest number of states, number of transitions, number of symbols, average rank per symbol and transition density observed;\n"

                + " - Quotienting imed out in "
                  + std::to_string(timeout.total)
                  + " (" + std::to_string(( (float) timeout.total / (float) metadata.total ) * (float) 100) + "%) "
                  + "of the automata. On average: \n"
                  + "    - the number of states at the end of each reduction was "
                  + std::to_string(testData.avg_q_reduction) + "%, the number of transitions was "
                    + std::to_string(testData.avg_delta_reduction) + "% and the transition density was "
                    + std::to_string(testData.avg_transDens_reduction) + "%; \n"
                    + "    - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
                    + std::to_string(testData.greatest_q_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
                    + std::to_string(testData.greatest_delta_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of transition density happened for an automaton whose transition density at the end of the minimization was "
                    + std::to_string(testData.greatest_transDens_reduction) + "%;\n", log_humanread_filename);
        testData.printReductionBuckets(log_humanread_filename);

}

static void printSummary_simple_heavy_tester(unsigned int la_dw, unsigned int la_up,
                                             MetaData& metadata, TestData& testData_heavy,
                                             Timeout& timeout_heavy)
{

    outputText("\nSummary: We applied Heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up)
               + ") to a total of " + std::to_string(metadata.total) + " automata, with an average of "
               + std::to_string(metadata.initial_avg_q) + " states, "
               + std::to_string(metadata.initial_avg_delta) + " transitions, "
               + std::to_string(metadata.initial_avg_sigma) + " symbols, average rank per symbol of "
               + std::to_string(metadata.initial_avg_ranking) + " and transition density of "
               + std::to_string(metadata.initial_avg_dens) + ". "
               + std::to_string(metadata.initial_greatest_q) + ", "
               + std::to_string(metadata.initial_greatest_delta) + ", "
               + std::to_string(metadata.initial_greatest_sigma) + ", "
               + std::to_string(metadata.initial_greatest_ranking) + " and "
               + std::to_string(metadata.initial_greatest_dens)
               + " were the greatest number of states, number of transitions, number of symbols, average rank per symbol and transition density observed;\n"
               + "Heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up) + ") timed out in "
               + std::to_string(timeout_heavy.total)
               + " (" + std::to_string(( (float) timeout_heavy.total / (float) metadata.total ) * (float) 100) + "%) "
               + "of the automata. On average: \n"
               + "    - the number of states at the end of each reduction dropped to "
               + std::to_string(testData_heavy.avg_q_reduction) + "%, the number of transitions dropped to "
               + std::to_string(testData_heavy.avg_delta_reduction) + "% and the transition density was "
               + std::to_string(testData_heavy.avg_transDens_reduction) + "% of the initial one; \n"
               + "    - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
               + std::to_string(testData_heavy.greatest_q_reduction) + "%;\n"
               + "    - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
               + std::to_string(testData_heavy.greatest_delta_reduction) + "%;\n"
               + "    - the most significant reduction in terms of transition density happened for an automaton whose transition density at the end of the minimization was "
               + std::to_string(testData_heavy.greatest_transDens_reduction) + "%;\n");

}

void simple_Heavy_tester(unsigned int sequence_version, unsigned int la_dw, unsigned int la_up,
                         string input_dir, string output_dir)
{

    MetaData metaData        =  MetaData();
    TestData testData_heavy  =  TestData();
    Timeout timeout_heavy    =  Timeout();

    vector<filename> filenames = getAllFilenames(input_dir);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i<NUMB_TESTS; i++)
    {

        string local_filename = getLocalFilename(filenames.at(i));
        string header1  = "\tAutomaton " + std::to_string(i+1) + " - " + local_filename + ":\n";
        outputText(header1);

        string autStr = VATA::Util::ReadFile(filenames.at(i));

        /* Parsing and taking data from the automaton. */
        stateDict sDict;
        Automaton aut_i = parseFromString(autStr, sDict, false, false);

        if (getNumbTransitions(aut_i) == 0)
        {
            string output_msg = "This automaton has 0 transitions and therefore will be skipped. \n";
            outputText(output_msg);
            continue;
        }

        metaData.inc();
        metaData.updateInitialAvg(getNumbUsedStates(aut_i), getNumbTransitions(aut_i),
                                   getNumbSymbols(aut_i), getAvgRank(aut_i),
                                   getTransitionDensity(aut_i),
                                   measureTransOverlaps(aut_i));
        metaData.checkInitialGreatest(getNumbUsedStates(aut_i),
                                       getNumbTransitions(aut_i),
                                       getNumbSymbols(aut_i), getAvgRank(aut_i),
                                       getTransitionDensity(aut_i),
                                       measureTransOverlaps(aut_i));

        /* Applying Heavy(k,j). */
        Automaton aut_heavy;
        try
        {
            aut_heavy = heavy(sequence_version, aut_i, la_dw, la_up);

            float q_red          =  measureStatesReduction(aut_heavy, aut_i);
            float delta_red      =  measureTransitionsReduction(aut_heavy, aut_i);
            float transDens_red  =  measureTransDensReduction(aut_heavy, aut_i);
            testData_heavy.inc();
            testData_heavy.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_heavy.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_heavy.updateReductionBuckets(q_red, delta_red, transDens_red);

            string q_red_str          =  std::to_string(q_red);
            string delta_red_str      =  std::to_string(delta_red);
            string transDens_red_str  =  std::to_string(transDens_red);

            if (q_red>=100.0 && delta_red>=100.0)
                outputText("No minimization was achieved. \n");
            else
                outputText("After the minimization, the number of states, number of transitions and the transition density were, resp., " +
                       q_red_str + "%, " + delta_red_str
                       + "% and " + transDens_red_str + "% of the original ones. \n");

        }
        catch (timeout_& e)
        {
            timeout_heavy.inc();
            timeout_heavy.checkSmallest(getNumbUsedStates(aut_i), getNumbTransitions(aut_i), getTransitionDensity(aut_i));

            aut_heavy            =  e.getAut();
            float q_red          =  measureStatesReduction(aut_heavy, aut_i);
            float delta_red      =  measureTransitionsReduction(aut_heavy, aut_i);
            float transDens_red  =  measureTransDensReduction(aut_heavy, aut_i);
            testData_heavy.inc();
            testData_heavy.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_heavy.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_heavy.updateReductionBuckets(q_red, delta_red, transDens_red);

            string q_red_str          =  std::to_string(q_red);
            string delta_red_str      =  std::to_string(delta_red);
            string transDens_red_str  =  std::to_string(transDens_red);

            outputText("Timeout: Heavy took longer than "
                      + std::to_string( (float) TIMEOUT/60 )
                      + " minutes and therefore was interrupted. ");

            if (q_red>=100.0 && delta_red>=100.0)   /* >= instead of == to avoid unsafeness of floating point comparison. */
                outputText("No minimization was achieved. \n");
            else
                outputText("The automaton was still minimized: the number of states, number of transitions and the transition density at the end of the minimization were, resp., " +
                           q_red_str + "%, " + delta_red_str
                           + "% and " + transDens_red_str + "% of the original ones. \n");

        }

        outputText("\n");

        aut_heavy = removeInitialState(aut_heavy);
        // dump the automaton
        string new_filename = appendTimbukFilename(local_filename,
                             "_minimized_with_Heavy(" + std::to_string(la_dw) + ","
                                                   + std::to_string(la_up) + ")");

        writeToFile(output_dir + new_filename, autToStringTimbuk(aut_heavy), true);

    }

    printSummary_simple_heavy_tester(la_dw, la_up, metaData, testData_heavy, timeout_heavy);

}

void procedure1_tester(vector<string> dir = {},
                       string summarized_results_filename = "", string log_humanread_filename = "")
{

    MetaData metaData             =  MetaData();
    TestData testData_ru          =  TestData();
    TestData testData_quot        =  TestData();
    Timeout timeout_quot          =  Timeout();
    TestData testData_qAndP       =  TestData();
    Timeout timeout_qAndP         =  Timeout();
    TestData testData_heavy       =  TestData();
    TestData testData_heavy_rels  =  TestData(numbRelations, numbQuotientings, numbPrunings);
    Timeout timeout_heavy         =  Timeout();

    string log_machread_filename;   /* Name of the machine-readable log file. */
    log_machread_filename =
            "results/logs/log_machine_readable_of_procedure1_tester_" + localTime();
    string log_machread_heavy_1_1_times_filename =
            "results/logs/log_machine_readable_of_procedure1_tester_heavy_1_1_times " + localTime();

    writeToFile(log_machread_heavy_1_1_times_filename,
                "Name\t#Q_i\t#Dt_i\tQ reduc. (RemoveUselessSt) (%)\tDt reduc. (RemoveUselessSt) (%)\tTimes(s) (RemoveUselessSt)\tQ reduc. (RUQ) (%)\tDt reduc. (RUQ) (%)\tTimes(s) (RUQ)\tQ reduc. (RUQP) (%)\tDt reduc. (RUQP) (%)\tTimes(s) (RUQP)\tQ reduc. (Heavy(1,1)) (%)\tDt reduc. (Heavy(1,1)) (%)\tTimes(s) (Heavy(1,1))\n");

    vector<filename> filenames = getAllFilenames(dir.empty() ? DIRS : dir);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i<NUMB_TESTS; i++)
    {
        string autStr = VATA::Util::ReadFile(filenames.at(i));
        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        Automaton aut = parseFromString(autStr,stateDict,false,false);

        if (getNumbTransitions(aut) == 0)
        {
            //string output_msg = "This automaton has 0 transitions and so will be skipped. \n";
            //outputText(output_msg, log_humanread_filename);
            continue;
        }

        string header1 = "  Test " + std::to_string(i+1) + " - " + filenames.at(i) + ":\n";
        outputText(header1, log_humanread_filename);

        writeToFile(log_machread_heavy_1_1_times_filename,
                    std::to_string(i+1) + ": " + filenames.at(i) + "\t");

        writeToFile(log_machread_filename, "\n" + header1);
        string header2 = "Q reduction (%) \t Delta reduction (%) \t TransDens reduction (%) \n";
        writeToFile(log_machread_filename, header2);       

        applyProcedure1(aut, stateDict, 1, 1, metaData,
                       testData_ru, testData_quot, timeout_quot,
                       testData_qAndP, timeout_qAndP, testData_heavy, testData_heavy_rels, timeout_heavy,
                       log_humanread_filename, log_machread_filename, log_machread_heavy_1_1_times_filename);

        outputText("\n", log_humanread_filename);
    }

    printSummary_procedure(1, 1, metaData, testData_ru, testData_quot, timeout_quot,
                           testData_qAndP, timeout_qAndP, testData_heavy, testData_heavy_rels, timeout_heavy,
                           log_humanread_filename);

    writeToFile((summarized_results_filename=="") ? "summarized_results_procedure1.txt" : summarized_results_filename,
                dir.at(0) + "\t"
                + std::to_string(testData_ru.avg_q_reduction)        + "\t"
                + std::to_string(testData_ru.avg_delta_reduction)    + "\t"
                + std::to_string(testData_ru.avg_time)    + "\t"
                + std::to_string(testData_quot.avg_q_reduction)      + "\t"
                + std::to_string(testData_quot.avg_delta_reduction)  + "\t"
                + std::to_string(testData_quot.avg_time)             + "\t"
                + std::to_string(testData_qAndP.avg_q_reduction)     + "\t"
                + std::to_string(testData_qAndP.avg_delta_reduction) + "\t"
                + std::to_string(testData_qAndP.avg_time)            + "\t"
                + std::to_string(testData_heavy.avg_q_reduction)     + "\t"
                + std::to_string(testData_heavy.avg_delta_reduction) + "\t"
                + std::to_string(testData_heavy.avg_time)            + "\n");

}

void procedure2_tester(vector<string> dir = {}, string summarized_results_filename="", string log_humanread_filename = "")
{

    MetaData metaData               =  MetaData();
    TestData testData_ru            =  TestData();
    TestData testData_quot          =  TestData();
    Timeout timeout_quot            =  Timeout();
    TestData testData_qAndP         =  TestData();
    Timeout timeout_qAndP           =  Timeout();
    TestData testData_heavy11       =  TestData();
    TestData testData_heavy11_rels  =  TestData(numbRelations, numbQuotientings, numbPrunings);
    Timeout timeout_heavy11         =  Timeout();
    TestData testData_heavy24       =  TestData();
    TestData testData_heavy24_rels  =  TestData(numbRelations, numbQuotientings, numbPrunings);
    Timeout timeout_heavy24         =  Timeout();

    string log_machread_filename;   /* Name of the machine-readable log file. */
    log_machread_filename =
            "results/logs/log_machine_readable_of_procedure2_tester_" + localTime();
    string log_machread_heavy_times_filename =
            "results/logs/log_machine_readable_of_procedure2_tester_times_" + localTime();

    writeToFile(log_machread_heavy_times_filename,
                "Name \t\t\t\t\t\t #Q_i \t #Dt_i \t Q reduc. (Heavy(1,1)) (%) \t Dt reduc. (Heavy(1,1)) (%) \t Times(s) (Heavy(1,1)) \t Q reduc. (Heavy(2,4)) (%) \t Dt reduc. (Heavy(2,4)) (%) \t Times(s) (Heavy(2,4)) \n");

    vector<filename> filenames = getAllFilenames(dir.empty() ? DIRS : dir);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i<NUMB_TESTS ; i++)
    {
        string header1 = "  Test " + std::to_string(i+1) + " - " + filenames.at(i) + ":\n";
        outputText(header1, log_humanread_filename);

        writeToFile(log_machread_heavy_times_filename,
                    std::to_string(i+1) + ": " + filenames.at(i) + "\t");

        string autStr = VATA::Util::ReadFile(filenames.at(i));

        writeToFile(log_machread_filename, "\n" + header1);
        string header2 = "Q reduction (%) \t Delta reduction (%) \t TransDens reduction (%) \n";
        writeToFile(log_machread_filename, header2);

        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        Automaton aut = parseFromString(autStr,stateDict,false,false);

        if (getNumbTransitions(aut) == 0)
        {
            string output_msg = "This automaton has 0 transitions and so will be skipped. \n";
            outputText(output_msg, log_humanread_filename);
            continue;
        }

        applyProcedure2(aut, stateDict, metaData,
                       testData_ru, testData_quot, timeout_quot,
                       testData_qAndP, timeout_qAndP,
                       testData_heavy11, testData_heavy11_rels, timeout_heavy11,
                       testData_heavy24, testData_heavy24_rels, timeout_heavy24,
                       log_humanread_filename, log_machread_filename, log_machread_heavy_times_filename);

        outputText("\n", log_humanread_filename);
    }

    printSummary_procedure(1, 1, metaData, testData_ru, testData_quot, timeout_quot,
                           testData_qAndP, timeout_qAndP,
                           testData_heavy11, testData_heavy11_rels, timeout_heavy11,
                           log_humanread_filename);

    writeToFile((summarized_results_filename=="") ? "summarized_results_procedure2.txt" : summarized_results_filename,
                dir.at(0) + "\t"
                + std::to_string(testData_ru.avg_q_reduction)          + "\t"
                + std::to_string(testData_ru.avg_delta_reduction)      + "\t"
                + std::to_string(testData_quot.avg_q_reduction)        + "\t"
                + std::to_string(testData_quot.avg_delta_reduction)    + "\t"
                + std::to_string(testData_qAndP.avg_q_reduction)       + "\t"
                + std::to_string(testData_qAndP.avg_delta_reduction)   + "\t"
                + std::to_string(testData_heavy11.avg_q_reduction)     + "\t"
                + std::to_string(testData_heavy11.avg_delta_reduction) + "\t"
                + std::to_string(testData_heavy24.avg_q_reduction)     + "\t"
                + std::to_string(testData_heavy24.avg_delta_reduction) + "\n");

}

void procedure22_tester(vector<string> dir = {}, string summarized_results_filename="", string log_humanread_filename = "")
{

    MetaData metaData               =  MetaData();
    TestData testData_ru            =  TestData();
    //Timeout timeout_ru              =  Timeout();
    TestData testData_quot          =  TestData();
    Timeout timeout_quot            =  Timeout();
    TestData testData_qAndP         =  TestData();
    Timeout timeout_qAndP           =  Timeout();
    TestData testData_heavy11       =  TestData();
    TestData testData_heavy11_rels  =  TestData(numbRelations, numbQuotientings, numbPrunings);
    Timeout timeout_heavy11         =  Timeout();
    TestData testData_heavy24       =  TestData();
    TestData testData_heavy24_rels  =  TestData(numbRelations, numbQuotientings, numbPrunings);
    Timeout timeout_heavy24         =  Timeout();

    string log_machread_filename;   /* Name of the machine-readable log file. */
    log_machread_filename =
            "results/logs/log_machine_readable_of_procedure22_tester_" + localTime();
    string log_machread_heavy_times_filename =
            "results/logs/log_machine_readable_of_procedure22_tester_times_" + localTime();

    writeToFile(log_machread_heavy_times_filename,
                "Name \t\t\t\t\t\t #Q_i \t #Dt_i \t Q reduc. (Heavy(1,1)) (%) \t Dt reduc. (Heavy(1,1)) (%) \t Times(s) (Heavy(1,1)) \t Q reduc. (Heavy(2,4)) (%) \t Dt reduc. (Heavy(2,4)) (%) \t Times(s) (Heavy(2,4)) \n");

    vector<filename> filenames = getAllFilenames(dir.empty() ? DIRS : dir);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i<NUMB_TESTS ; i++)
    {
        string header1 = "  Test " + std::to_string(i+1) + " - " + filenames.at(i) + ":\n";
        outputText(header1, log_humanread_filename);

        writeToFile(log_machread_heavy_times_filename,
                    std::to_string(i+1) + ": " + filenames.at(i) + "\t");

        string autStr = VATA::Util::ReadFile(filenames.at(i));

        writeToFile(log_machread_filename, "\n" + header1);
        string header2 = "Q reduction (%) \t Delta reduction (%) \t TransDens reduction (%) \n";
        writeToFile(log_machread_filename, header2);

        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        Automaton aut = parseFromString(autStr,stateDict,false,false);

        if (getNumbTransitions(aut) == 0)
        {
            string output_msg = "This automaton has 0 transitions and so will be skipped. \n";
            outputText(output_msg, log_humanread_filename);
            continue;
        }

        applyProcedure22(aut, stateDict, metaData,
                       testData_heavy11, testData_heavy11_rels, timeout_heavy11,
                       testData_heavy24, testData_heavy24_rels, timeout_heavy24,
                       log_humanread_filename, log_machread_filename, log_machread_heavy_times_filename);

        outputText("\n", log_humanread_filename);
    }

    printSummary_procedure(1, 1, metaData, testData_ru, testData_quot, timeout_quot,
                           testData_qAndP, timeout_qAndP,
                           testData_heavy11, testData_heavy11_rels, timeout_heavy11,
                           log_humanread_filename);

    writeToFile((summarized_results_filename=="") ? "summarized_results_procedure22.txt" : summarized_results_filename,
                dir.at(0) + "\t"
                + std::to_string(testData_ru.avg_q_reduction)          + "\t"
                + std::to_string(testData_ru.avg_delta_reduction)      + "\t"
                + std::to_string(testData_quot.avg_q_reduction)        + "\t"
                + std::to_string(testData_quot.avg_delta_reduction)    + "\t"
                + std::to_string(testData_qAndP.avg_q_reduction)       + "\t"
                + std::to_string(testData_qAndP.avg_delta_reduction)   + "\t"
                + std::to_string(testData_heavy11.avg_q_reduction)     + "\t"
                + std::to_string(testData_heavy11.avg_delta_reduction) + "\t"
                + std::to_string(testData_heavy24.avg_q_reduction)     + "\t"
                + std::to_string(testData_heavy24.avg_delta_reduction) + "\n");

}

void procedure3_tester(vector<string> dir = {}, string summarized_results_filename="",
                       string log_humanread_filename = "")
{

    MetaData metaData               =  MetaData();
    TestData testData_ru            =  TestData();
    TestData testData_quot          =  TestData();
    Timeout timeout_quot            =  Timeout();
    TestData testData_qAndP         =  TestData();
    Timeout timeout_qAndP           =  Timeout();
    TestData testData_heavy11       =  TestData();
    TestData testData_heavy11_rels  =  TestData(numbRelations, numbQuotientings, numbPrunings);
    Timeout timeout_heavy11         =  Timeout();
    TestData testData_heavy24       =  TestData();
    TestData testData_heavy24_rels  =  TestData(numbRelations, numbQuotientings, numbPrunings);
    Timeout timeout_heavy24         =  Timeout();
    TestData testData_heavy36       =  TestData();
    TestData testData_heavy36_rels  =  TestData(numbRelations, numbQuotientings, numbPrunings);
    Timeout timeout_heavy36         =  Timeout();

    string log_machread_filename;   /* Name of the machine-readable log file. */
    log_machread_filename =
            "results/logs/log_machine_readable_of_procedure3_tester_" + localTime();
    string log_machread_heavy_times_filename =
            "results/logs/log_machine_readable_of_procedure3_tester_times_" + localTime();

    writeToFile(log_machread_heavy_times_filename,
                "Name\t#Q_i\t#Dt_i\tQ reduc. (RemoveUselessSt) (%)\tDt reduc. (RemoveUselessSt) (%)\tTimes(s) (RemoveUselessSt)\tQ reduc. (RUQ) (%)\tDt reduc. (RUQ) (%)\tTimes(s) (RUQ)\tQ reduc. (RUQP) (%)\tDt reduc. (RUQP) (%)\tTimes(s) (RUQP)\tQ reduc. (Heavy(1,1)) (%)\tDt reduc. (Heavy(1,1)) (%)\tTimes(s) (Heavy(1,1))\tQ reduc. (Heavy(2,4)) (%)\tDt reduc. (Heavy(2,4)) (%)\tTimes(s) (Heavy(2,4))\tQ reduc. (Heavy(3,6)) (%)\tDt reduc. (Heavy(3,6)) (%)\tTimes(s) (Heavy(3,6))\n");

    vector<filename> filenames = getAllFilenames(dir.empty() ? DIRS : dir);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i<NUMB_TESTS; i++)
    {      
        string autStr = VATA::Util::ReadFile(filenames.at(i));
        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        Automaton aut = parseFromString(autStr,stateDict,false,false);

        if (getNumbTransitions(aut) == 0)
        {
            //string output_msg = "This automaton has 0 transitions and so will be skipped. \n";
            //outputText(output_msg, log_humanread_filename);
            continue;
        }

        string header1 = "  Test " + std::to_string(i+1) + " - " + filenames.at(i) + ":\n";
        outputText(header1, log_humanread_filename);

        writeToFile(log_machread_heavy_times_filename,
                    std::to_string(i+1) + ": " + filenames.at(i) + "\t");

        writeToFile(log_machread_filename, "\n" + header1);
        string header2 = "Q reduction (%) \t Delta reduction (%) \t TransDens reduction (%) \n";
        writeToFile(log_machread_filename, header2);

        applyProcedure3(aut, stateDict, metaData,
                       testData_ru, testData_quot, timeout_quot,
                       testData_qAndP, timeout_qAndP,
                       testData_heavy11, testData_heavy11_rels, timeout_heavy11,
                       testData_heavy24, testData_heavy24_rels, timeout_heavy24,
                       testData_heavy36, testData_heavy36_rels, timeout_heavy36,
                       log_humanread_filename, log_machread_filename, log_machread_heavy_times_filename);

        outputText("\n", log_humanread_filename);
    }

    printSummary_procedure(1, 1, metaData, testData_ru, testData_quot, timeout_quot,
                           testData_qAndP, timeout_qAndP,
                           testData_heavy11, testData_heavy11_rels, timeout_heavy11,
                           log_humanread_filename);

    writeToFile((summarized_results_filename=="") ? "summarized_results_procedure3.txt" : summarized_results_filename,
                dir.at(0) + "\t"
                + std::to_string(testData_ru.avg_q_reduction)          + "\t"
                + std::to_string(testData_ru.avg_delta_reduction)      + "\t"
                + std::to_string(testData_ru.avg_time)                 + "\t"
                + std::to_string(testData_quot.avg_q_reduction)        + "\t"
                + std::to_string(testData_quot.avg_delta_reduction)    + "\t"
                + std::to_string(testData_quot.avg_time)               + "\t"
                + std::to_string(testData_qAndP.avg_q_reduction)       + "\t"
                + std::to_string(testData_qAndP.avg_delta_reduction)   + "\t"
                + std::to_string(testData_qAndP.avg_time)              + "\t"
                + std::to_string(testData_heavy11.avg_q_reduction)     + "\t"
                + std::to_string(testData_heavy11.avg_delta_reduction) + "\t"
                + std::to_string(testData_heavy11.avg_time)            + "\t"
                + std::to_string(testData_heavy24.avg_q_reduction)     + "\t"
                + std::to_string(testData_heavy24.avg_delta_reduction) + "\t"
                + std::to_string(testData_heavy24.avg_time)            + "\t"
                + std::to_string(testData_heavy36.avg_q_reduction)     + "\t"
                + std::to_string(testData_heavy36.avg_delta_reduction) + "\t"
                + std::to_string(testData_heavy36.avg_time)            + "\n");

}

void heavy_tester(unsigned int la_dw, unsigned int la_up,
                  unsigned int sequence_version,
                  vector<string> dir = {},
                  string summarized_results_filename = ""/*, string log_humanread_filename = ""*/)
{

    MetaData metaData             =  MetaData();
    TestData testData_heavy       =  TestData();
    //TestData testData_heavy_rels  =  TestData(numbRelations, numbQuotientings, numbPrunings);
    Timeout timeout_heavy         =  Timeout();

    string log_machread_filename;   /* Name of the machine-readable log file. */
    log_machread_filename =
            "results/logs/log_machine_readable_of_heavy(" + std::to_string(la_dw) + ","
            + std::to_string(la_up) + ")_tester_" + localTime();
    string log_machread_heavy_times_filename =
            "results/logs/log_machine_readable_of_heavy(" + std::to_string(la_dw) + ","
            + std::to_string(la_up) + ")_tester_times_" + localTime();

    writeToFile(log_machread_heavy_times_filename,
                "Name \t\t\t\t\t\t #Q_i \t #Dt_i \t Q reduc. (Heavy("
                + std::to_string(la_dw) + ","
                + std::to_string(la_up) + ")) (%) \t Dt reduc. (Heavy("
                + std::to_string(la_dw) + ","
                + std::to_string(la_up) + ")) (%) \t Times(s) (Heavy("
                + std::to_string(la_dw) + ","
                + std::to_string(la_up) + "))  \n");

    vector<filename> filenames = getAllFilenames(dir.empty() ? DIRS : dir);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i<NUMB_TESTS  ; i++)
    {
        string autStr = VATA::Util::ReadFile(filenames.at(i));
        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        std::tuple<Automaton,Automaton> aut_tup = parseFromString2(autStr,stateDict,false,false);

        Automaton aut = std::get<1>(aut_tup);
       
        if (getNumbTransitions(aut) == 0)
        {
            /*string output_msg = "This automaton has 0 transitions and so will be skipped. \n";
            outputText(output_msg, log_humanread_filename);*/
            continue;
        }

        string header1 = "  Test " + std::to_string(i+1) + " - " + filenames.at(i) + ":\n";
        //outputText(header1, log_humanread_filename);

        writeToFile(log_machread_heavy_times_filename,
                    std::to_string(i+1) + ": " + filenames.at(i) + "\t");

        writeToFile(log_machread_filename, "\n" + header1);
        string header2 = "Q reduction (%) \t Delta reduction (%) \t TransDens reduction (%) \n";
        writeToFile(log_machread_filename, header2);

        applyHeavy(sequence_version, aut, stateDict, metaData, la_dw, la_up,
                   testData_heavy, timeout_heavy,
                   /*log_humanread_filename,*/ log_machread_filename, log_machread_heavy_times_filename);

    }

    writeToFile((summarized_results_filename=="") ?
                    ("summarized_results_heavy(" + std::to_string(la_dw) + ","
                     + std::to_string(la_up) +").txt")
                  : summarized_results_filename,
                dir.at(0) + "\t"
               + std::to_string(testData_heavy.avg_q_reduction) + "\t"
               + std::to_string(testData_heavy.avg_delta_reduction) + "\t"
               + std::to_string(testData_heavy.avg_time) + "\n");

}

void applyDwSim(const Automaton& aut, unsigned int la_dw_i,
                unsigned int la_dw_f,
                vector<TestData> & testData, string log_machread_times_filename)
{
    unsigned int greatest_state  = getGreatestUsedState(aut);
    unsigned int greatest_symbol = getGreatestUsedSymbol(aut);
    vector<bool> column(greatest_state+1);
    vector<typerank> ranks = getRanks(aut, greatest_symbol+1);

    for (unsigned int la_dw = la_dw_i; la_dw <= la_dw_f; la_dw++)
    {
        vector<vector<bool> > dw_la(greatest_state+1, column);
        auto start = std::chrono::high_resolution_clock::now();
        timespec startTime;
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &startTime);
        try
        {
            dw_simulation(aut, la_dw, dw_la, greatest_state+1, ranks, &startTime);

            outputText(std::to_string(getSizeOfRel(dw_la, greatest_state+1)) + " ", "dw_sim_size_lin_pr_depth=" + std::to_string(pr_depth) + ".txt");

            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
            testData[la_dw].inc();
            testData[la_dw].updateAvgTime(seconds);
            outputText(std::to_string(seconds) + "\t", log_machread_times_filename);
        }
        catch(timeout_& e)
        {
            testData[la_dw].inc();
            testData[la_dw].updateAvgTime(TIMEOUT);
            outputText("TO \t", log_machread_times_filename);
        }
    }

    outputText("\n", "dw_sim_size_lin_pr_depth=" + std::to_string(pr_depth) + ".txt");
    outputText("\n", log_machread_times_filename);

}

void dw_tester(unsigned int la_dw_i, unsigned int la_dw_f,
               vector<string> dir = {}, string summarized_results_filename = "")
{
    TestData td   = TestData();
    vector<TestData> testData (la_dw_f, td);
    string log_machread_times_filename =
            "results/logs/log_machine_readable_times_of_dw_from_" + std::to_string(la_dw_i) + "_to_"
            + std::to_string(la_dw_f) + "_tester_times" + "_pr_depth=" + std::to_string(pr_depth)
            + "_" + localTime();

    writeToFile(log_machread_times_filename,
                "Name \t\t\t\t\t\t \t Times(s) (Dw-i sim("
                + std::to_string(la_dw_i) + "-"
                + std::to_string(la_dw_f) + "))  \n");

    vector<filename> filenames = getAllFilenames(dir.empty() ? DIRS : dir);
    const unsigned int NUMB_TESTS = filenames.size();

    outputText(std::to_string(la_dw_i) + " - " + std::to_string(la_dw_f) + " \n", "dw_sim_size_lin_pr_depth=" + std::to_string(pr_depth) + ".txt");

    for (unsigned int i=0; i<NUMB_TESTS ; i++)
    {
        //string header1 = "  Test " + std::to_string(i+1) + " - " + filenames.at(i) + ":\n";
        //outputText(header1, log_humanread_filename);

        writeToFile(log_machread_times_filename,
                    std::to_string(i+1) + ": " + filenames.at(i) + "\t");

        string autStr = VATA::Util::ReadFile(filenames.at(i));

        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        std::tuple<Automaton,Automaton> aut_tup = parseFromString2(autStr,stateDict,false,false);

        Automaton aut = std::get<1>(aut_tup);

        if (getNumbTransitions(aut) == 0)
        {
            /*string output_msg = "This automaton has 0 transitions and so will be skipped. \n";
            outputText(output_msg, log_humanread_filename);*/
            continue;
        }

        outputText(std::to_string(i+1) + ": " + filenames.at(i) + "\t", "dw_sim_size_lin_pr_depth=" + std::to_string(pr_depth) + ".txt");

        applyDwSim(aut, la_dw_i, la_dw_f, testData, log_machread_times_filename);

        //outputText("\n", log_humanread_filename);
    }

//    printSummary_procedure(la_dw, la_up, metaData, testData_ru, timeout_ru, testData_quot, timeout_quot,
//                           testData_qAndP, timeout_qAndP,
//                           testData_heavy11, testData_heavy11_rels, timeout_heavy11,
//                           log_humanread_filename);

    writeToFile((summarized_results_filename=="") ?
                    ("summarized_results_dw_sim_from_" + std::to_string(la_dw_i) + "_to_"
                     + std::to_string(la_dw_f) +".txt")
                  : summarized_results_filename,
                    dir.at(0));
    for (unsigned int la_dw = la_dw_i; la_dw <= la_dw_f; la_dw++)
    {
        writeToFile((summarized_results_filename=="") ?
                        ("summarized_results_dw_sim_from_" + std::to_string(la_dw_i) + "_to_"
                         + std::to_string(la_dw_f) +".txt")
                      : summarized_results_filename,
                      "\t" + std::to_string(testData[la_dw].avg_time));
    }
    writeToFile((summarized_results_filename=="") ?
                    ("summarized_results_dw_sim_from_" + std::to_string(la_dw_i) + "_to_"
                     + std::to_string(la_dw_f) +".txt")
                  : summarized_results_filename,
                  "\n");

}

void combined_preorder_tester(unsigned int la_dw, unsigned int la_up,
                              vector<string> dir = {},
                              string summarized_results_filename = "",
                              string log_humanread_filename = "")
{

    MetaData metaData     = MetaData();
    TestData testData     = TestData();
    Timeout timeout       = Timeout();

    string log_machread_filename =
            "results/logs/log_machine_readable_of_combined_preorder(" + std::to_string(la_dw) + ","
            + std::to_string(la_up) + ")_tester_" + localTime();
    string log_machread_times_filename =
            "results/logs/log_machine_readable_of_combined_preorder(" + std::to_string(la_dw) + ","
            + std::to_string(la_up) + ")_tester_times_" + localTime();

    writeToFile(log_machread_times_filename,
                "Name \t #Q_i \t #Dt_i \t Q reduc. (Combined_preorder("
                + std::to_string(la_dw) + ","
                + std::to_string(la_up) + ")) (%) \t Dt reduc. (Combined_preorder("
                + std::to_string(la_dw) + ","
                + std::to_string(la_up) + ")) (%) \t Times(s) (Combined_preorder("
                + std::to_string(la_dw) + ","
                + std::to_string(la_up) + "))  \n");

    vector<filename> filenames    = getAllFilenames(dir.empty() ? DIRS : dir);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i<NUMB_TESTS  ; i++)
    {
        string autStr = VATA::Util::ReadFile(filenames.at(i));
        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        std::tuple<Automaton,Automaton> aut_tup = parseFromString2(autStr,stateDict,false,false);

        Automaton aut = std::get<1>(aut_tup);

        if (getNumbTransitions(aut) == 0)
            continue;

        string header1 = "  Test " + std::to_string(i+1) + " - " + filenames.at(i) + ":\n";
        outputText(header1, log_humanread_filename);

        writeToFile(log_machread_times_filename,
                    std::to_string(i+1) + ": " + filenames.at(i) + "\t");

        writeToFile(log_machread_filename, "\n" + header1);
        string header2 = "Q reduction (%) \t Delta reduction (%) \t TransDens reduction (%) \n";
        writeToFile(log_machread_filename, header2);

        applyQuotCombinedPreorder(aut, stateDict, metaData, la_dw, la_up, testData, timeout,
                                  log_humanread_filename, log_machread_filename, log_machread_times_filename);

    }

    printSummary_quotienting(la_dw, la_up, metaData, testData, timeout, log_humanread_filename);

    writeToFile((summarized_results_filename=="") ?
                    ("summarized_results_combined_preorder(" + std::to_string(la_dw) + ","
                     + std::to_string(la_up) +").txt") : summarized_results_filename,
                dir.at(0) + "\t"
               + std::to_string(testData.avg_q_reduction) + "\t"
               + std::to_string(testData.avg_delta_reduction) + "\t"
               + std::to_string(testData.avg_time) + "\n");

}

void wrong_procedure_tester(vector<string> dir)
{

    vector<filename> filenames = getAllFilenames(dir.empty() ? DIRS : dir);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i<NUMB_TESTS ; i++)
    {
        std::cout << i << endl;

        string autStr = VATA::Util::ReadFile(filenames.at(i));

        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        Automaton aut_i = parseFromString(autStr,stateDict,false,false);

        if (getNumbTransitions(aut_i) == 0)
        {
            continue;
        }

        Automaton aut = copyAut(aut_i);

        unsigned int numb_states = getNumbUsedStates(aut);
        unsigned int numb_symbols = getNumbSymbols(aut);
        vector<typerank> ranks = getRanks(aut,numb_symbols);
        quotient_with_up_la_dw_la(aut, 1, 2);
        prune_with_up_la_dw_la_and_strict_dw_la(aut, stateDict, 1, 2, numb_states,
                                                numb_symbols, ranks, true);

        if (!equiv(aut,aut_i))
            exit_with_error("lang not preserved");
    }

}

void removeDeadStates_tester()
{
    vector<filename> filenames = getAllFilenames(DIRS);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i<NUMB_TESTS; i++)
    {
        std::cout << "  Test " << i+1 << " - " << filenames.at(i) << ":\n";
        string autStr = VATA::Util::ReadFile(filenames.at(i));

        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        Automaton aut = parseFromString(autStr,stateDict,false,false);
        if (getNumbTransitions(aut) == 0)
        {
            std::cout << "This automaton has 0 transitions and so will be skipped." << std::endl;
            continue;
        }

        std::cout << "Before removing the dead states: \n";
        printAut(aut);

        aut = removeDeadStates(aut);

        std::cout << "After removing the dead states: \n";
        printAut(aut);
    }
}

void call_procedure_tester(unsigned int version, string dir)
{
    vector<string> subfolders = getAllSubfolders(dir);

    string summarized_results_filename =
            "very_summarized_results_procedure" + std::to_string(version) + "_"
            + /*"pr_depth=" + std::to_string(pr_depth) + "_" +*/ localTime() + ".txt";
    for (const string& subfolder : subfolders)
    {        
        string log_humanread_filename =
                "results//Procedure" + std::to_string(version) + "//" + subfolder + "_" + localTime() + ".txt";

        string subdir = dir + "//" + subfolder + "//";

        if (version == 1)
            procedure1_tester({subdir}, summarized_results_filename, log_humanread_filename);
        if (version == 2)
            procedure2_tester({subdir}, summarized_results_filename, log_humanread_filename);
        if (version == 22)
            procedure22_tester({subdir}, summarized_results_filename, log_humanread_filename);
        if (version == 3)
            procedure3_tester({subdir}, summarized_results_filename, log_humanread_filename);
    }

}

void call_heavy_tester(unsigned int la_dw, unsigned int la_up, unsigned int sequence_version, string dir)
{
    vector<string> subfolders = getAllSubfolders(dir);

    string summarized_results_filename =
            "very_summarized_results_heavy(" + std::to_string(la_dw) + ","
            + std::to_string(la_up) + ")_" + localTime() + ".txt";
    for (const string& subfolder : subfolders)
    {
        /*string log_humanread_filename =
                "results//Heavy(" + std::to_string(la_dw) + ","
                + std::to_string(la_up) + ")//" + subfolder + "_" + localTime() + ".txt";*/

        string subdir = dir + "//" + subfolder + "//";

        heavy_tester(la_dw, la_up, sequence_version, {subdir},
                     summarized_results_filename/*, log_humanread_filename*/);

    }

}

void call_combined_preorder_tester(unsigned int la_dw, unsigned int la_up, string dir)
{
    vector<string> subfolders = getAllSubfolders(dir);

    string summarized_results_filename =
            "very_summarized_results_combined_preorder_la-dw=" + std::to_string(la_dw) + "_la-up="
            + std::to_string(la_up) + "_" + localTime() + ".txt";

    for (const string& subfolder : subfolders)
    {
        string log_humanread_filename =
                "results//human_friendly_logs//" + subfolder + "_" + localTime() + ".txt";

        string subdir = dir + "//" + subfolder + "//";

        combined_preorder_tester(la_dw, la_up, {subdir}, summarized_results_filename, log_humanread_filename);
    }

}

void call_dw_sim_tester(unsigned int la_dw_i, unsigned int la_dw_f, string dir)
{
    vector<string> subfolders = getAllSubfolders(dir);

    string summarized_results_filename =
            "very_summarized_results_dw_from_" + std::to_string(la_dw_i) + "_to_"
            + std::to_string(la_dw_f) + "_pr_depth=" + std::to_string(pr_depth) + "_" + localTime() + ".txt";

    for (const string& subfolder : subfolders)
    {
        /*string log_humanread_filename =
                "results//Heavy(" + std::to_string(la_dw) + ","
                + std::to_string(la_up) + ")//" + subfolder + "_" + localTime() + ".txt";*/

        string subdir = dir + "//" + subfolder + "//";

        dw_tester(la_dw_i, la_dw_f, {subdir}, summarized_results_filename);

    }

}

void compare_pr_methods(string dir)
{
    vector<string> subfolders = getAllSubfolders(dir);

    for (const string& subfolder : subfolders)
    {
        string subdir = dir + "//" + subfolder + "//";

        vector<filename> filenames = getAllFilenames(subdir);
        const unsigned int NUMB_TESTS = filenames.size();

        for (unsigned int i=0; i<NUMB_TESTS ; i++)
        {
            string autStr = VATA::Util::ReadFile(filenames.at(i));

            /* Parsing and taking data from the automaton. */
            stateDict stateDict;
            std::tuple<Automaton,Automaton> aut_tup = parseFromString2(autStr,stateDict,false,false);

            Automaton aut = std::get<1>(aut_tup);

            if (getNumbTransitions(aut) == 0)
                continue;

            outputText(std::to_string(i+1) + ": " + filenames.at(i) + "\t", "dw_sim_size_lin_pr_depth'=" + std::to_string(pr_depth) + ".txt");

            unsigned int greatest_state  = getGreatestUsedState(aut);
            unsigned int greatest_symbol = getGreatestUsedSymbol(aut);
            vector<bool> column(greatest_state+1);
            vector<typerank> ranks = getRanks(aut, greatest_symbol+1);

            vector<vector<bool> > W1(greatest_state+1, column);
            initializeW(aut, W1, greatest_state+1);
            dw_simulation(aut, pr_depth, W1, greatest_state+1, ranks);

            vector<vector<bool> > W2(greatest_state+1, column);
            initializeW(aut, W2, greatest_state+1);
            pre_refine_lin(W2, aut, pr_depth, greatest_state+1, greatest_symbol+1, ranks, getInitialState(aut));

            outputText("For the automaton " + std::to_string(i+1) + ": " + filenames.at(i)
                       + "the given pairs are in W1 but not in W2 \n");
            //printAut(aut);

            for (unsigned int i=0; i<W1.size() && i != greatest_state; i++)
                for (unsigned int j=0; j<W1.size() && j != greatest_state; j++)
                    if (W1.at(i).at(j) && !W2.at(i).at(j))
                        outputText("(" + /*std::to_string(*/translateState(aut,stateDict,i)/*)*/ + ","
                                   + /*std::to_string(*/translateState(aut,stateDict,j)/*)*/ + ") ");

            outputText("\n\n");

        }

    }

}

void call_wrong_procedure_tester()
{
    string dir = "test automata//random_tabakov_vardi_timbuk//samples of 400 TA each with 1<=td<=6";

    vector<string> subfolders = getAllSubfolders(dir);

    for (const string& subfolder : subfolders)
    {
        string subdir = dir + "//" + subfolder + "//";
        wrong_procedure_tester({subdir});
    }

}


void minimizationTechniques_tester()
{

    pr_depth = 1;
    call_heavy_tester(2, 4, 1, "test automata//random_tabakov_vardi_timbuk//samples of 400 TA each with 1<=td<=6//all");

}
