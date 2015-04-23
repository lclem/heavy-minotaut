
//#include <vata/serialization/timbuk_serializer.hh>
#include "minimizationTechniques_tester.hh"

// A static global variable has its access restricted to the scope of the file.
static vector<string> DIRS = {
                           // "test automata//small_timbuk//",
                            "test automata//moderate_artmc_timbuk//" ,
                           //  "test automata//forester_redblack_timbuk//32843200//",
                           //  "test automata//forester_redblack_timbuk//33465936//",
                           //  "test automata//forester_redblack_timbuk//33559760//",
                           //  "test automata//forester_redblack_timbuk//33578272//",
                           //  "test automata//forester_redblack_timbuk//33636192//",
                           // "test automata//artmc_timbuk//",
                           // "test automata//yun_yun_mata//"
                           };

//static vector<const char*>DIRS = {"test automata//debugging_tests//"};
//static vector<const char*>DIRS = {"test automata//small automata//dead states//"};

static void printSummary_minimizationSequence(unsigned int numb_failures, unsigned int NUMB_TESTS,
                                              unsigned int la_dw, unsigned int la_up,
                                              Success* success, Reduction* reduction, Timeout* timeout) {
    std::cout << "\nSummary: I tried to apply the following Procedure to a total of " << NUMB_TESTS << " automata: \n"
              << " 1 - computation of dw-" << la_dw << " sim + quotienting with it; \n"
              << " 2 - pruning with P(id, strict dw-la sim); \n"
              << " 3 - computation of up-" << la_up << "(id) sim + quotienting with it; \n"
              << " 4 - computation of strict up-1(id) sim + pruning with P(strict up-1(id) sim, dw-" << la_dw << " sim); \n"
              //<< " 4.5 - computing the (maximal) dw-" << la << " sim again; \n"
              << " 5 - repetition of step 1; \n"
              << " 6 - computation of dw-1 sim and of up-" << la_up << "(dw-1 sim) sim + pruning with P(up-"
              << la_up << "(dw-1 sim) sim), strict dw-1 sim); \n"

              << "For " << timeout->total << " (" << ((float) timeout->total / (float) NUMB_TESTS)*100
              << "%) of those automata, the Procedure was aborted due to a 'timeout' (" << TIMEOUT/60 << "min) "
              << "in the computation of one of the relations. ";

    if (numb_failures == 0)
        std::cout << "In all of the other " << success->total << " (" << ((float) success->total / (float) NUMB_TESTS)*100
                  << "%) tests, the Procedure was successful, i.e., the Reduction (effective or not) preserved the language.\n";
    else
        std::cout << "In " << numb_failures << "(" << ((float) numb_failures / (float) NUMB_TESTS)*100
                  << "%) of the tests, the Reduction did NOT preserve the language.\n";
    std::cout << "The following results are relative to the automata for which the Reduction methods successfully preserved the language:\n"

              << " - prior to any Reduction, the automata had, on average, " << success->initial_avg_q << " states, "
              << success->initial_avg_delta << " transitions, " << success->initial_avg_sigma << " symbols, average rank per symbol of "
              << success->initial_avg_ranking << " and transition density of " << success->initial_avg_dens << ". "
              << success->initial_greatest_q << ", " << success->initial_greatest_delta << ", " << success->initial_greatest_sigma << ", "
              << success->initial_greatest_ranking << " and " << success->initial_greatest_dens
              << " were the greatest number of states, number of transitions, number of symbols, average rank per symbol and transition density observed;\n"

              << " - the dw-" << la_dw << " sim computed contained, on average, "
              << success->avg_sizes_relations[DW_LA_1ST_TIME]
              << " pairs of states (including pairs with the special initial state ψ)"
              << " and took " << success->avg_numbs_refinements[DW_LA_1ST_TIME]
              << " refinements and " << success->avg_times_relations[DW_LA_1ST_TIME]
              << " seconds to compute. \n"

              << " - the quotienting with dw-la sim took, on average, "
              << success->avg_times_quotientings[QUOT_WITH_DW_LA] << " sec. \n"

              << " - the pruning with P(id, dw-la sim.) took "
              << success->avg_times_prunings[PRUN_WITH_P_ID_AND_DW_LA] << " sec. \n"

              << " - the up-" << la_up << "(id) sim computed the first time contained, on average, "
              << success->avg_sizes_relations[UP_LA_WITH_ID_1ST_TIME]
              << " pairs of states and took "
              << success->avg_numbs_refinements[UP_LA_WITH_ID_1ST_TIME]
              << " refinements and " << success->avg_times_relations[UP_LA_WITH_ID_1ST_TIME]
              << " seconds to compute. \n"

              << " - the first quotienting with up-la(id) took "
              << success->avg_times_quotientings[QUOT_WITH_UP_LA_WITH_ID_1ST] << " sec. \n"

              << " - the strict up-1(id) sim contained, on average, "
              << success->avg_sizes_relations[STRICT_UP_1_WITH_ID] << " pairs of states"
              << " and took " << success->avg_numbs_refinements[STRICT_UP_1_WITH_ID]
              << " refinements and " << success->avg_times_relations[STRICT_UP_1_WITH_ID]
              << " seconds to compute. \n"

              << " - the pruning with P(up-1(id), dw-la) took "
              << success->avg_times_prunings[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA] << " sec. \n"

//              << " - the dw-" << la << " sim computed the second time contained, on average, "
//              << success->avg_sizes_relations[DW_LA_2ND_TIME]
//              << " pairs of states (including pairs with the special initial state ψ)"
//              << " and took " << success->avg_numbs_refinements[DW_LA_2ND_TIME]
//              << " refinements and " << success->avg_times_relations[DW_LA_2ND_TIME]
//              << " seconds to compute. \n"

              << " - the second quotienting with up-la(id) took "
              << success->avg_times_quotientings[QUOT_WITH_UP_LA_WITH_ID_2ND] << " sec. \n"

              << " - the dw-1 sim contained, on average, " << success->avg_sizes_relations[DW_1]
              << " pairs of states and took " << success->avg_numbs_refinements[DW_1]
              << " refinements and " << success->avg_times_relations[DW_1]
              << " seconds to compute. \n"

              << " - the computed up-" << la_up << "(dw-1 sim) sim contained, on average, "
              << success->avg_sizes_relations[UP_LA_WITH_DW_1] << " pairs of states"
              << " and took " << success->avg_numbs_refinements[UP_LA_WITH_DW_1]
              << " refinements and " << success->avg_times_relations[UP_LA_WITH_DW_1]
              << " seconds to compute.\n"

              << " - the pruning with P(up-la(dw-1), strict dw-1) took "
              << success->avg_times_prunings[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1] << " sec. \n"

                 /*
              << " The up-" << la << "(id) sim computed the 2nd time contained, on average, " << success->avg_sizes_relations[UP_LA_WITH_ID_2ND_TIME] << " pairs of states"
              << " and took " << success->avg_numbs_refinements[UP_LA_WITH_ID_2ND_TIME] << " refinements and " << success->avg_times_relations[UP_LA_WITH_ID_2ND_TIME] << " seconds to compute."*/

              << " - the consistency check took " << success->avg_time_consCheck << " sec.\n"

              << " - " << ((float)reduction->total / (float)success->total) * (float)100 << "% "
              << "of the automata effectively became smaller (smaller number of states or smaller number of transitions) after applying the Reduction methods;\n"

              << " - on those automata that effectively became smaller, on average, the number of states at the end of the reduction was "
              << reduction->avg_q_reduction << "% and the number of transitions was "
              << reduction->avg_delta_reduction << "%; \n"
              << " - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
              << reduction->greatest_q_reduction << "%;\n"
              << " - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
              << reduction->greatest_delta_reduction << "%;\n";

    if (timeout->total > 0)
              std::cout << "Additional data: The smallest number of states in an automaton that generated a 'timeout' was of "
              << timeout->smallest_q << ", the smallest number of transitions was "
              << timeout->smallest_delta
              << " and the smallest transition density was " << timeout->smallest_dens << "\n";

    delete success;
    delete reduction;
    delete timeout;
}

static void printSummary_testData(/*unsigned int NUMB_TESTS,*/ unsigned int la_dw,
                                  unsigned int la_up, TestData* testData, Timeout* timeout) {

    std::cout << "\nThe following results are relative to the minimization sequence consisting of: \n"
              << " 1 - computation of dw-" << la_dw << " sim + quotienting with it; \n"
              << " 2 - pruning with P(id, strict dw-la sim); \n"
              << " 3 - computation of up-" << la_up << "(id) sim + quotienting with it; \n"
              << " 4 - computation of strict up-1(id) sim + pruning with P(strict up-1(id) sim, dw-" << la_dw << " sim); \n"
              //<< " 4.5 - computing the (maximal) dw-" << la << " sim again; \n"
              << " 5 - repetition of step 1; \n"
              << " 6 - computation of dw-1 sim and of up-" << la_up << "(dw-1 sim) sim + pruning with P(up-"
              << la_up << "(dw-1 sim) sim), strict dw-1 sim); \n"

              << " - the dw-" << la_dw << " sim computed contained, on average, "
              << testData->avg_sizes_relations[DW_LA_1ST_TIME]
              << " pairs of states (including pairs with the special initial state ψ)"
              << " and took " << testData->avg_numbs_refinements[DW_LA_1ST_TIME]
              << " refinements and " << testData->avg_times_relations[DW_LA_1ST_TIME]
              << " seconds to compute. \n"

              << " - the quotienting with dw-la sim took, on average, "
              << testData->avg_times_quotientings[QUOT_WITH_DW_LA] << " sec. \n"

              << " - the pruning with P(id, dw-la sim.) took "
              << testData->avg_times_prunings[PRUN_WITH_P_ID_AND_DW_LA] << " sec. \n"

              << " - the up-" << la_up << "(id) sim computed the first time contained, on average, "
              << testData->avg_sizes_relations[UP_LA_WITH_ID_1ST_TIME]
              << " pairs of states and took "
              << testData->avg_numbs_refinements[UP_LA_WITH_ID_1ST_TIME]
              << " refinements and " << testData->avg_times_relations[UP_LA_WITH_ID_1ST_TIME]
              << " seconds to compute. \n"

              << " - the first quotienting with up-la(id) took "
              << testData->avg_times_quotientings[QUOT_WITH_UP_LA_WITH_ID_1ST] << " sec. \n"

              << " - the strict up-1(id) sim contained, on average, "
              << testData->avg_sizes_relations[STRICT_UP_1_WITH_ID] << " pairs of states"
              << " and took " << testData->avg_numbs_refinements[STRICT_UP_1_WITH_ID]
              << " refinements and " << testData->avg_times_relations[STRICT_UP_1_WITH_ID]
              << " seconds to compute. \n"

              << " - the pruning with P(up-1(id), dw-la) took "
              << testData->avg_times_prunings[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA] << " sec. \n"

//              << " - the dw-" << la << " sim computed the second time contained, on average, "
//              << success->avg_sizes_relations[DW_LA_2ND_TIME]
//              << " pairs of states (including pairs with the special initial state ψ)"
//              << " and took " << success->avg_numbs_refinements[DW_LA_2ND_TIME]
//              << " refinements and " << success->avg_times_relations[DW_LA_2ND_TIME]
//              << " seconds to compute. \n"

              << " - the second quotienting with up-la(id) took "
              << testData->avg_times_quotientings[QUOT_WITH_UP_LA_WITH_ID_2ND] << " sec. \n"

              << " - the dw-1 sim contained, on average, " << testData->avg_sizes_relations[DW_1]
              << " pairs of states and took " << testData->avg_numbs_refinements[DW_1]
              << " refinements and " << testData->avg_times_relations[DW_1]
              << " seconds to compute. \n"

              << " - the computed up-" << la_up << "(dw-1 sim) sim contained, on average, "
              << testData->avg_sizes_relations[UP_LA_WITH_DW_1] << " pairs of states"
              << " and took " << testData->avg_numbs_refinements[UP_LA_WITH_DW_1]
              << " refinements and " << testData->avg_times_relations[UP_LA_WITH_DW_1]
              << " seconds to compute.\n"

              << " - the pruning with P(up-la(dw-1), strict dw-1) took "
              << testData->avg_times_prunings[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1] << " sec. \n"

                 /*
              << " The up-" << la << "(id) sim computed the 2nd time contained, on average, " << success->avg_sizes_relations[UP_LA_WITH_ID_2ND_TIME] << " pairs of states"
              << " and took " << success->avg_numbs_refinements[UP_LA_WITH_ID_2ND_TIME] << " refinements and " << success->avg_times_relations[UP_LA_WITH_ID_2ND_TIME] << " seconds to compute."*/

              << " - the consistency check took " << testData->avg_time_consCheck << " sec.\n"
              << "There were a total of " << timeout->total << " timeouts.\n";

    if (timeout->total > 0)
              std::cout << "Additional data: The smallest number of states in an automaton that generated a 'timeout' was of "
              << timeout->smallest_q << ", the smallest number of transitions was "
              << timeout->smallest_delta
              << " and the smallest transition density was " << timeout->smallest_dens << "\n";

    delete testData;
    delete timeout;
}

//static void printSummary_minimizationProcedure(unsigned int numb_failures, unsigned int NUMB_TESTS, unsigned int la_dw, unsigned int la_up,
//                                              MetaData* metadata, Reduction* reduction) {

//    if (numb_failures == 0)
//        std::cout << "In all of the " << metadata->total << " (" << ((float) metadata->total / (float) NUMB_TESTS)*100
//                  << "%) tests, the Procedure was successful, i.e., the Reduction (effective or not) preserved the language.\n";
//    else
//        std::cout << "In " << numb_failures << "(" << ((float) numb_failures / (float) NUMB_TESTS)*100
//                  << "%) of the tests, the Reduction did NOT preserve the language.\n";
//    std::cout << "The following results are relative to the automata for which the Reduction methods successfully preserved the language:\n"

//              << " - prior to any Reduction, the automata had, on average, " << metadata->initial_avg_q << " states, "
//              << metadata->initial_avg_delta << " transitions, " << metadata->initial_avg_sigma << " symbols, average rank per symbol of "
//              << metadata->initial_avg_ranking << " and transition density of " << metadata->initial_avg_dens << ". "
//              << metadata->initial_greatest_q << ", " << metadata->initial_greatest_delta << ", " << metadata->initial_greatest_sigma << ", "
//              << metadata->initial_greatest_ranking << " and " << metadata->initial_greatest_dens
//              << " were the greatest number of states, number of transitions, number of symbols, average rank per symbol and transition density observed;\n"

//              << " - " << ((float)reduction->total / (float)metadata->total) * (float)100 << "% "
//              << "of the automata effectively became smaller (smaller number of states or smaller number of transitions) after applying the Reduction methods;\n"

//              << " - on those automata that effectively became smaller, on average, the number of states at the end of the reduction was "
//              << reduction->avg_q_reduction << "% and the number of transitions was "
//              << reduction->avg_delta_reduction << "% and the transition density was "
//              << reduction->avg_transDens_reduction << "; \n"
//              << " - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
//              << reduction->greatest_q_reduction << "%;\n"
//              << " - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
//              << reduction->greatest_delta_reduction << "%;\n"
//              << " - the most significant reduction in terms of transition density happened for an automaton whose number of transitions was reduced to "
//              << reduction->greatest_transDens_reduction << "%;\n"
//                 ;

//    delete metadata;
//    delete reduction;
//}

static void printSummary_procedure(unsigned int la_dw, unsigned int la_up, MetaData& metadata,
                                   TestData& testData_ru,     Timeout& timeout_ru,
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

                 + " - Removing the useless states timed out in "
                  + std::to_string(timeout_ru.total)
                  + " (" + std::to_string (((float) timeout_ru.total / (float) metadata.total) * (float) 100) + "%) "
                  + "of the automata. On average: \n"
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
                     + std::to_string(testData_heavy_rels.avg_times_prunings[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA]) + " sec. \n"
                     + "    - the dw-" + std::to_string(la_dw) + " sim computed the second time contained, on average, "
                     + std::to_string(testData_heavy_rels.avg_sizes_relations[DW_LA_2ND_TIME])
                     + " pairs of states and took " + std::to_string(testData_heavy_rels.avg_numbs_refinements[DW_LA_2ND_TIME])
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

static void printSummaryConsistency(unsigned int numb_failures, unsigned int NUMB_TESTS, unsigned int la, Success* success, Reduction* reduction, Timeout* timeout) {
    std::cout << "\nSummary: I tried to apply the following Procedure to a total of " << NUMB_TESTS << " automata: \n"
              << " 1 - computation of dw-" << la << " sim + quotienting with it; \n"
              << " 2 - pruning with P(id, strict dw-la sim); \n"
              << " 3 - computation of up-" << la << "(id) sim + quotienting with it; \n"
              << " 4 - computation of strict up-1(id) sim + pruning with P(strict up-1(id) sim, dw-" << la << " sim); \n"
              //<< " 4.5 - computing the (maximal) dw-" << la << " sim again; \n"
              << " 5 - repetition of step 1; \n"
              << " 6 - computation of dw-1 sim and of up-" << la << "(dw-1 sim) sim + pruning with P(up-" << la << "(dw-1 sim) sim), strict dw-1 sim); \n"

              << "For " << timeout->total << " (" << ((float) timeout->total / (float) NUMB_TESTS)*100
              << "%) of those automata, the Procedure was aborted due to a 'timeout' (" << TIMEOUT/60 << "min) "
              << "in the computation of one of the relations. ";

    if (numb_failures == 0)
        std::cout << "In all of the other " << success->total << " (" << ((float) success->total / (float) NUMB_TESTS)*100
                  << "%) tests, the Procedure was successful, i.e., the Reduction (effective or not) preserved the language.\n";
    else
        std::cout << "In " << numb_failures << "(" << ((float) numb_failures / (float) NUMB_TESTS)*100
                  << "%) of the tests, the Reduction did NOT preserve the language.\n";
    std::cout << "The following results are relative to the automata for which the Reduction methods successfully preserved the language:\n"

              << " - prior to any Reduction, the automata had, on average, " << success->initial_avg_q << " states, "
              << success->initial_avg_delta << " transitions, " << success->initial_avg_sigma << " symbols, average rank per symbol of "
              << success->initial_avg_ranking << " and transition density of " << success->initial_avg_dens << ". "
              << success->initial_greatest_q << ", " << success->initial_greatest_delta << ", " << success->initial_greatest_sigma << ", "
              << success->initial_greatest_ranking << " and " << success->initial_greatest_dens
              << " were the greatest number of states, number of transitions, number of symbols, average rank per symbol and transition density observed;\n"

              << " - the dw-" << la << " sim computed contained, on average, "
              << success->avg_sizes_relations[DW_LA_1ST_TIME]
              << " pairs of states (including pairs with the special initial state ψ)"
              << " and took " << success->avg_numbs_refinements[DW_LA_1ST_TIME]
              << " refinements and " << success->avg_times_relations[DW_LA_1ST_TIME]
              << " seconds to compute. \n"

              << " - the quotienting with dw-la sim took, on average, "
              << success->avg_times_quotientings[QUOT_WITH_DW_LA] << " sec. \n"

              << " - the pruning with P(id, dw-la sim.) took "
              << success->avg_times_prunings[PRUN_WITH_P_ID_AND_DW_LA] << " sec. \n"

              << " - the up-" << la << "(id) sim computed the first time contained, on average, "
              << success->avg_sizes_relations[UP_LA_WITH_ID_1ST_TIME]
              << " pairs of states and took "
              << success->avg_numbs_refinements[UP_LA_WITH_ID_1ST_TIME]
              << " refinements and " << success->avg_times_relations[UP_LA_WITH_ID_1ST_TIME]
              << " seconds to compute. \n"

              << " - the first quotienting with up-la(id) took "
              << success->avg_times_quotientings[QUOT_WITH_UP_LA_WITH_ID_1ST] << " sec. \n"

              << " - the strict up-1(id) sim contained, on average, "
              << success->avg_sizes_relations[STRICT_UP_1_WITH_ID] << " pairs of states"
              << " and took " << success->avg_numbs_refinements[STRICT_UP_1_WITH_ID]
              << " refinements and " << success->avg_times_relations[STRICT_UP_1_WITH_ID]
              << " seconds to compute. \n"

              << " - the pruning with P(up-1(id), dw-la) took "
              << success->avg_times_prunings[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA] << " sec. \n"

//              << " - the dw-" << la << " sim computed the second time contained, on average, "
//              << success->avg_sizes_relations[DW_LA_2ND_TIME]
//              << " pairs of states (including pairs with the special initial state ψ)"
//              << " and took " << success->avg_numbs_refinements[DW_LA_2ND_TIME]
//              << " refinements and " << success->avg_times_relations[DW_LA_2ND_TIME]
//              << " seconds to compute. \n"

              << " - the second quotienting with up-la(id) took "
              << success->avg_times_quotientings[QUOT_WITH_UP_LA_WITH_ID_2ND] << " sec. \n"

              << " - the dw-1 sim contained, on average, " << success->avg_sizes_relations[DW_1]
              << " pairs of states and took " << success->avg_numbs_refinements[DW_1]
              << " refinements and " << success->avg_times_relations[DW_1]
              << " seconds to compute. \n"

              << " - the computed up-" << la << "(dw-1 sim) sim contained, on average, "
              << success->avg_sizes_relations[UP_LA_WITH_DW_1] << " pairs of states"
              << " and took " << success->avg_numbs_refinements[UP_LA_WITH_DW_1]
              << " refinements and " << success->avg_times_relations[UP_LA_WITH_DW_1]
              << " seconds to compute.\n"

              << " - the pruning with P(up-la(dw-1), strict dw-1) took "
              << success->avg_times_prunings[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1] << " sec. \n"

                 /*
              << " The up-" << la << "(id) sim computed the 2nd time contained, on average, " << success->avg_sizes_relations[UP_LA_WITH_ID_2ND_TIME] << " pairs of states"
              << " and took " << success->avg_numbs_refinements[UP_LA_WITH_ID_2ND_TIME] << " refinements and " << success->avg_times_relations[UP_LA_WITH_ID_2ND_TIME] << " seconds to compute."*/

              << " - the consistency check took " << success->avg_time_consCheck << " sec.\n"

              << " - " << ((float)reduction->total / (float)success->total) * (float)100 << "% "
              << "of the automata effectively became smaller (smaller number of states or smaller number of transitions) after applying the Reduction methods;\n"

              << " - on those automata that effectively became smaller, on average, the number of states at the end of the reduction was "
              << reduction->avg_q_reduction << "% and the number of transitions was "
              << reduction->avg_delta_reduction << "%; \n"
              << " - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
              << reduction->greatest_q_reduction << "%;\n"
              << " - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
              << reduction->greatest_delta_reduction << "%;\n";

    if (timeout->total > 0)
              std::cout << "Additional data: The smallest number of states in an automaton that generated a 'timeout' was of "
              << timeout->smallest_q << ", the smallest number of transitions was "
              << timeout->smallest_delta
              << " and the smallest transition density was " << timeout->smallest_dens << "\n";

    delete success;
    delete reduction;
    delete timeout;
}

void simple_Heavy_tester(unsigned int la_dw, unsigned int la_up, string dir/*, int timeout*/)
{
    MetaData metaData             =  MetaData();
    TestData testData_heavy       =  TestData();
    Timeout timeout_heavy         =  Timeout();

    vector<filename> filenames = getAllFilenames(dir);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i<NUMB_TESTS; i++)
    {
        string header1 = "  Automaton " + std::to_string(i+1) + " - " + filenames.at(i) + ":\n";
        outputText(header1);

        string autStr = VATA::Util::ReadFile(filenames.at(i));

        /* Parsing and taking data from the automaton. */
        stateDict sDict;
        Automaton aut_i = parseFromString(autStr, sDict, false, false);
        //std::cout << autToStringTimbuk(aut_i, &sDict) << std::endl;

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
        /*string headline = "Applying Heavy(" + std::to_string(la_dw) + ", "
                + std::to_string(la_up) + ")...";
        outputText("\n" + headline + "\n");*/

        Automaton aut_heavy = copyAut(aut_i, true);
        try
        {
            aut_heavy = applyHeavy(aut_heavy, sDict, la_dw, la_up/*, timeout*/);

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

            /*outputText("  The resulting automaton has "
                       + q_red_str + "% of its states, "
                       + delta_red_str + "% of its transitions and "
                       + transDens_red_str + "% of its transition density. \n");*/

        }
        catch (timeout_& e)
        {
            outputText("Timeout: Heavy took longer than "
                      + std::to_string( (float) TIMEOUT/60 )
                       + " minutes and therefore was aborted. In terms of averages, for this case we will consider that no reduction has been achieved on this particular automaton. \n");
            timeout_heavy.inc();
            timeout_heavy.checkSmallest(getNumbUsedStates(aut_i), getNumbTransitions(aut_i), getTransitionDensity(aut_i));

            testData_heavy.inc();
            testData_heavy.updateAvgReductions(100, 100, 100);
            testData_heavy.checkGreatestReductions(100, 100, 100);
            testData_heavy.updateReductionBuckets(100, 100, 100);
        }

        outputText("\n");

        // create the serializer for the Timbuk format
        VATA::Serialization::AbstrSerializer* serializer =
                new VATA::Serialization::TimbukSerializer();
        aut_heavy = removeInitialState(aut_heavy);
        // dump the automaton
        string new_filename = appendTimbukFilename(filenames.at(i),
                             "_minimized_with_Heavy(" + std::to_string(la_dw) + ","
                                                   + std::to_string(la_up) + ")");

        if (!(equiv(aut_i,aut_heavy)) || !(equiv(aut_heavy,aut_i)))
            exit_with_error("error");

        //std::cout << autToStringTimbuk(aut_heavy, &sDict) << std::endl;
        writeToFile(new_filename, autToStringTimbuk(aut_heavy), true);

    }

    printSummary_simple_heavy_tester(la_dw, la_up, metaData, testData_heavy, timeout_heavy);

}

void procedure1_tester(vector<string> dir = {},
                       string summarized_results_filename = "", string log_humanread_filename = "")
{

    MetaData metaData             =  MetaData();
    TestData testData_ru          =  TestData();
    Timeout timeout_ru            =  Timeout();
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

    /*writeToFile(log_machread_filename,
                "Using la-dw=" + std::to_string(la_dw) + " la-up=" + std::to_string(la_up) + "\n");
    writeToFile(log_machread_heavy_1_1_times_filename,
                "Using la-dw=" + std::to_string(la_dw) + " la-up=" + std::to_string(la_up) + "\n");*/

    writeToFile(log_machread_heavy_1_1_times_filename,
                "Name \t\t\t\t\t\t #Q_i \t #Dt_i \t #Q_f \t #Dt_f \t Q reduc. (%) \t Dt reduc. (%) \t Times(s) \n");

    vector<filename> filenames = getAllFilenames(dir.empty() ? DIRS : dir);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i<NUMB_TESTS; i++)
    {
        string header1 = "  Test " + std::to_string(i+1) + " - " + filenames.at(i) + ":\n";
        outputText(header1, log_humanread_filename);

        writeToFile(log_machread_heavy_1_1_times_filename,
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

        applyProcedure1(aut, stateDict, 1, 1, metaData,
                       testData_ru, timeout_ru, testData_quot, timeout_quot,
                       testData_qAndP, timeout_qAndP, testData_heavy, testData_heavy_rels, timeout_heavy,
                       log_humanread_filename, log_machread_filename, log_machread_heavy_1_1_times_filename);

        outputText("\n", log_humanread_filename);
    }

    printSummary_procedure(1, 1, metaData, testData_ru, timeout_ru, testData_quot, timeout_quot,
                           testData_qAndP, timeout_qAndP, testData_heavy, testData_heavy_rels, timeout_heavy,
                           log_humanread_filename);

    writeToFile((summarized_results_filename=="") ? "summarized_results_procedure1.txt" : summarized_results_filename,
                dir.at(0) + "\t"
                + std::to_string(testData_ru.avg_q_reduction)        + "\t"
                + std::to_string(testData_ru.avg_delta_reduction)    + "\t"
                + std::to_string(testData_quot.avg_q_reduction)      + "\t"
                + std::to_string(testData_quot.avg_delta_reduction)  + "\t"
                + std::to_string(testData_qAndP.avg_q_reduction)     + "\t"
                + std::to_string(testData_qAndP.avg_delta_reduction) + "\t"
                + std::to_string(testData_heavy.avg_q_reduction)     + "\t"
                + std::to_string(testData_heavy.avg_delta_reduction) + "\n");

}

void procedure2_tester(vector<string> dir = {}, string summarized_results_filename="", string log_humanread_filename = "")
{

    MetaData metaData             =  MetaData();
    TestData testData_ru          =  TestData();
    Timeout timeout_ru            =  Timeout();
    TestData testData_quot        =  TestData();
    Timeout timeout_quot          =  Timeout();
    TestData testData_qAndP       =  TestData();
    Timeout timeout_qAndP         =  Timeout();
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
                       testData_ru, timeout_ru, testData_quot, timeout_quot,
                       testData_qAndP, timeout_qAndP,
                       testData_heavy11, testData_heavy11_rels, timeout_heavy11,
                       testData_heavy24, testData_heavy24_rels, timeout_heavy24,
                       log_humanread_filename, log_machread_filename, log_machread_heavy_times_filename);

        outputText("\n", log_humanread_filename);
    }

    printSummary_procedure(1, 1, metaData, testData_ru, timeout_ru, testData_quot, timeout_quot,
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

void procedure3_tester(vector<string> dir = {}, string summarized_results_filename="", string log_humanread_filename = "")
{

    MetaData metaData             =  MetaData();
    TestData testData_ru          =  TestData();
    Timeout timeout_ru            =  Timeout();
    TestData testData_quot        =  TestData();
    Timeout timeout_quot          =  Timeout();
    TestData testData_qAndP       =  TestData();
    Timeout timeout_qAndP         =  Timeout();
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

    /*writeToFile(log_machread_filename,
                "Using la-dw=" + std::to_string(la_dw) + " la-up=" + std::to_string(la_up) + "\n");
    writeToFile(log_machread_heavy_1_1_times_filename,
                "Using la-dw=" + std::to_string(la_dw) + " la-up=" + std::to_string(la_up) + "\n");*/

    writeToFile(log_machread_heavy_times_filename,
                "Name \t\t\t\t\t\t #Q_i \t #Dt_i \t Q reduc. (Heavy(1,1)) (%) \t Dt reduc. (Heavy(1,1)) (%) \t Times(s) (Heavy(1,1)) \t Q reduc. (Heavy(2,4)) (%) \t Dt reduc. (Heavy(2,4)) (%) \t Times(s) (Heavy(2,4)) \t Q reduc. (Heavy(3,6)) (%) \t Dt reduc. (Heavy(3,6)) (%) \t Times(s) (Heavy(3,6)) \n");

    vector<filename> filenames = getAllFilenames(dir.empty() ? DIRS : dir);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i</*NUMB_TESTS*/ 200 ; i++)
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

        applyProcedure3(aut, stateDict, metaData,
                       testData_ru, timeout_ru, testData_quot, timeout_quot,
                       testData_qAndP, timeout_qAndP,
                       testData_heavy11, testData_heavy11_rels, timeout_heavy11,
                       testData_heavy24, testData_heavy24_rels, timeout_heavy24,
                       testData_heavy36, testData_heavy36_rels, timeout_heavy36,
                       log_humanread_filename, log_machread_filename, log_machread_heavy_times_filename);

        outputText("\n", log_humanread_filename);
    }

    printSummary_procedure(1, 1, metaData, testData_ru, timeout_ru, testData_quot, timeout_quot,
                           testData_qAndP, timeout_qAndP,
                           testData_heavy11, testData_heavy11_rels, timeout_heavy11,
                           log_humanread_filename);

    writeToFile((summarized_results_filename=="") ? "summarized_results_procedure3.txt" : summarized_results_filename,
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
                + std::to_string(testData_heavy24.avg_delta_reduction) + "\t"
                + std::to_string(testData_heavy36.avg_q_reduction)     + "\t"
                + std::to_string(testData_heavy36.avg_delta_reduction) + "\n");

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
        /*tuple<*/Automaton/*,unsigned int, unsigned int> parsing_tuple*/ aut = parseFromString(autStr,stateDict,false,false);
        //Automaton aut = std::get<0>(parsing_tuple);
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

void foo(unsigned int version, string dir)
{
    vector<string> subfolders = getAllSubfolders(dir);

    string summarized_results_filename =
            "very_summarized_results_procedure" + std::to_string(version) + "_" + localTime() + ".txt";
    for (const string& subfolder : subfolders)
    {        
        string log_humanread_filename =
                "results//Procedure" + std::to_string(version) + "//" + subfolder + "_" + localTime() + ".txt";

        string subdir = dir + "//" + subfolder + "//";

        if (version == 1)
            procedure1_tester({subdir}, summarized_results_filename, log_humanread_filename);
        if (version == 2)
            procedure2_tester({subdir}, summarized_results_filename, log_humanread_filename);
        if (version == 3)
            procedure3_tester({subdir}, summarized_results_filename, log_humanread_filename);
    }

}


void minimizationTechniques_tester() {
    //CheckConsistency(3);

    //procedure_tester(1, 1, {}, "");

    foo(3, "test automata//random_tabakov_vardi_timbuk//samples of 200 TA each");

}
