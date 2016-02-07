
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file defining a collection of functions that perform tests   *
 *  to minimization techniques based on the computation of both lookahead downward  *
 *  simulation and lookahead upward simulation.                                     *
 * 																					*
 ************************************************************************************/

#include "minimizationTechniques_testers.hh"

unsigned int pr_depth = -1;


void printSummary_procedure(unsigned int la_dw, unsigned int la_up, MetaData& metadata,
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

void heavy_tester(unsigned int la_dw, unsigned int la_up,
                  string input_dir, string output_dir, unsigned int max_numb_tests,
                  string log_avgs)
{

    MetaData metaData         =  MetaData();
    TestData testData_heavy   =  TestData();
    Timeout timeout_heavy     =  Timeout();

    string log = "log_of_heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up) +
            ")_input_folder:" + getLocalFilename(input_dir) + "_date:" + localTime() + ".txt";
    writeToFile(log,
                "Each row of the following table contains results of an experiment on a particular tree automaton. The columns should be read as follows: \n"
                "\t Filename - the name of the file containing the input tree automaton (TA) \n"
                "\t Q_i - the number of states of TA \n"
                "\t Dt_i - the number of transitions of TA \n"
                "\t TD_i - the transition density of TA \n"
               // "\t (TOL_i_1,TOL_i_2,TOL_I_3) - the transitions-overlapping tuple in TA (consult the references in README.txt for more details) \n"
                "\t Q_h(%) - the percentage of the number of states that remained after the application of the Heavy algorithm on TA \n"
                "\t Dt_h(%) - the percentage of the number of transitions that remained after the application of the Heavy algorithm on TA \n"
                "\t TD_h(%) - the t.d. of TA vs the t.d. of Heavy(TA) \n"
                "\t T_h - the time (in seconds) taken to minimize TA with Heavy \n \n");
/*    writeToFile(log, "Filename\tQ_i\tDt_i\tTD_i\t(TOL_i_1,TOL_i_2,TOL_I_3)\t"
                "Q_h(%)\tDt_h(%)\tTD_h(%)\tT_h \n"); */

    vector<filename> filenames = getAllFilenames(input_dir);
    const unsigned int numb_tests = max_numb_tests==0 ? filenames.size() : min(max_numb_tests,filenames.size());

    for (unsigned int i=0; i<numb_tests; i++)
    {
        string filename = filenames.at(i);
        string local_filename = getLocalFilename(filename);
        writeToFile(log, local_filename + "\t");

        string header  = "\tAutomaton " + std::to_string(i+1) + " - " + filename + ": ";
        outputText(header);   

        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        AutData autData = parseFromFile(filename, stateDict);

        if (getNumbTransitions(getAut(autData)) == 0)
        {
            string output_msg = "This automaton has 0 transitions and therefore will be skipped. \n";
            outputText(output_msg);
            continue;
        }

        AutData autData_heavy = applyHeavy(autData, metaData, la_dw, la_up,
                                           testData_heavy, timeout_heavy, log);

        /* Output resulting automaton (optional). */
        if (output_dir != "")
        {
            string aut_heavy_filename = appendTimbukFilename(sanitizeDirStr(output_dir) + local_filename,
                                    "_minimized_with_Heavy(" + std::to_string(la_dw) + "," +
                                    std::to_string(la_up) + ")");
            saveToFile(autData_heavy, aut_heavy_filename);
        }

        outputText("Done. \n");
    }

    outputText("\n"
               "  Please consult the file ./" + log +
               " for individual experimental results concerning each automaton in the folder " +
               getLocalFilename(input_dir) + ".\n \n");

    if (!log_avgs.empty())
        writeToFile(log_avgs,
                    getLocalFilename(input_dir) + "\t" +
                    metaData.initial_avg_q_str2Dec() + "\t" +
                    metaData.initial_avg_delta_str2Dec() + "\t" +
                    metaData.initial_avg_transDens_str2Dec() + "\t" +
                    //metaData.initial_avg_transOverlap_str() + "\t" +
                    testData_heavy.avg_q_red_str2Dec() + "\t" +
                    testData_heavy.avg_delta_red_str2Dec() + "\t" +
                    testData_heavy.avg_transDens_red_str2Dec() + "\t" +
                    testData_heavy.avg_time_str2Dec() + "\n");

}

void heavy_tester(unsigned int la_dw, unsigned int la_up,
                  vector<string> input_dirs, string output_dir, unsigned int numb_tests, string log_avgs_)
{
    string log_avgs = log_avgs_=="" ? "log_of_avg_values_of_applying_heavy(" + std::to_string(la_dw) + "," +
                       std::to_string(la_up) + ")_date:" + localTime() + ".txt" : log_avgs_;
    writeToFile(log_avgs, "Each row of the following table contains avg. result values "
                          "respecting experiments on the automata in a given directory. "
                          "The columns should be read as follows: \n"
                          "\t Folder - the name of the folder that this avg. values correspond to \n"
                          "\t Q_i - the avg. number of states of the TA \n"
                          "\t Dt_i - the avg. number of transitions of the TA \n"
                          "\t TD_i - the avg. transition density of the TA \n"
/*                          "\t (TOL_i_1,TOL_i_2,TOL_I_3) - the avg. transitions-overlapping tuple in TA (consult the references in README.txt for more details) \n"    */
                          "\t Q_h(%) - the avg. percentage of the number of states that remained after the application of the Heavy algorithm on the TA \n"
                          "\t Dt_h(%) - the avg. percentage of the number of transitions that remained after the application of the Heavy algorithm on the TA \n"
                          "\t TD_h(%) - the avg. relation: t.d. of the TA vs the t.d. of the Heavy(TA) \n"
                          "\t T_h - the avg. time (in seconds) taken to compute Heavy(TA) \n \n");
/*    writeToFile(log_avgs, "Folder\t\tQ_i\tDt_i\tTD_i\t(TOL_i_1,TOL_i_2,TOL_I_3)"
                "Q_h(%)\tDt_h(%)\tTD_h(%)\tT_h \n");    */

    for (const string input_dir : input_dirs)
        heavy_tester(la_dw, la_up, input_dir, output_dir, numb_tests, log_avgs);

    writeToFile(log_avgs, "\nThe End\n");

    outputText("The End. Please consult file " + log_avgs + " for average result values for each input directory. \n");
}

void combined_relation_tester(unsigned int la_dw, unsigned int la_up,
                              vector<string> dir,
                              string summarized_results_filename = "",
                              string log_humanread_filename = "")
{    
    MetaData metaData = MetaData();
    TestData testData = TestData();
    Timeout timeout   = Timeout();

    string log_machread_filename =
            "results/logs/log_machine_readable_of_combined_relation(" + std::to_string(la_dw) + ","
            + std::to_string(la_up) + ")_tester_" + localTime();
    string log_machread_times_filename =
            "results/logs/log_machine_readable_of_combined_relation(" + std::to_string(la_dw) + ","
            + std::to_string(la_up) + ")_tester_times_" + localTime();

    writeToFile(log_machread_times_filename,
                "Name \t #Q_i \t #Dt_i \t Q reduc. (Combined_preorder("
                + std::to_string(la_dw) + ","
                + std::to_string(la_up) + ")) (%) \t Dt reduc. (Combined_preorder("
                + std::to_string(la_dw) + ","
                + std::to_string(la_up) + ")) (%) \t Times(s) (Combined_preorder("
                + std::to_string(la_dw) + ","
                + std::to_string(la_up) + "))  \n");

    vector<filename> filenames    = getAllFilenames(dir);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i<NUMB_TESTS  ; i++)
    {
        string autStr = VATA::Util::ReadFile(filenames.at(i));
        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        AutData autData = parseFromString(autStr,stateDict);

        if (getNumbTransitions(autData) == 0)
            continue;

        string header1 = "  Test " + std::to_string(i+1) + " - " + filenames.at(i) + ":\n";
        outputText(header1, log_humanread_filename);

        writeToFile(log_machread_times_filename,
                    std::to_string(i+1) + ": " + filenames.at(i) + "\t");

        writeToFile(log_machread_filename, "\n" + header1);
        string header2 = "Q reduction (%) \t Delta reduction (%) \t TransDens reduction (%) \n";
        writeToFile(log_machread_filename, header2);

        applyQuotCombinedPreorder(autData, stateDict, metaData, la_dw, la_up, /*ranks,*/ testData, timeout,
                                  log_humanread_filename, log_machread_filename, log_machread_times_filename);

    }

    printSummary_quotienting(la_dw, la_up, metaData, testData, timeout, log_humanread_filename);

    writeToFile((summarized_results_filename=="") ?
                    ("summarized_results_combined_relation(" + std::to_string(la_dw) + ","
                     + std::to_string(la_up) +").txt") : summarized_results_filename,
                dir.at(0) + "\t"
               + std::to_string(testData.avg_q_reduction) + "\t"
               + std::to_string(testData.avg_delta_reduction) + "\t"
               + std::to_string(testData.avg_time) + "\n");

}

