
#include "minimizationAndComplement_testers.hh"


#include <iostream>     // provides std::cout
#include <utility>      // provides std::pair


static void printSummary_complement_after_heavy_tester(unsigned int la_dw, unsigned int la_up,
                                                       MetaData& metadata, TestData& testData,
                                                       Timeout& timeout)
{

    outputText("\nSummary: We minimized (using Heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up)
               + ")) and complemented a total of " + std::to_string(metadata.total) + " automata, with an average of "
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
               + std::to_string(timeout.total)
               + " (" + std::to_string(( (float) timeout.total / (float) metadata.total ) * (float) 100) + "%) "
               + "of the automata. On average: \n"
               + "    - the number of states at the end of each reduction dropped to "
               + std::to_string(testData.avg_q_reduction) + "%, the number of transitions dropped to "
               + std::to_string(testData.avg_delta_reduction) + "% and the transition density was "
               + std::to_string(testData.avg_transDens_reduction) + "% of the initial one. "
               + "On average, the minimization and the complement together took "
               + std::to_string(testData.avg_time) + " seconds; \n"
               + "    - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
               + std::to_string(testData.greatest_q_reduction) + "%;\n"
               + "    - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
               + std::to_string(testData.greatest_delta_reduction) + "%;\n"
               + "    - the most significant reduction in terms of transition density happened for an automaton whose transition density at the end of the minimization was "
               + std::to_string(testData.greatest_transDens_reduction) + "%;\n"
               );

}

AutData complement(const AutData& autData1, TestData& testData, string log)
{
    auto start = startTimer();
    AutData autData2 = complement(autData1);
    float seconds = elapsed_sec(start);

    testData.inc();
    testData.updateAvgSizes(autData2);
    testData.updateAvgTime(seconds);

    log_autSizes(log, autData2, seconds);

    return autData2;
}

AutData complement_after_heavy(const AutData& autData_i, unsigned int la_dw, unsigned int la_up,
                               TestData& testData_h, TestData& testData_hAndc, string log)
{
    /* Run Heavy(la_dw,la_up) on the aut and time it. */
    auto start_h      = startTimer();
    AutData autData_h;
    try
    {
        autData_h = heavy(autData_i, la_dw, la_up);
    }
    catch (timeout_& e)
    {
        autData_h = wrapAutData(e.getAut(), getRanks(autData_i));
    }
    float time_h = elapsed_sec(start_h);

    /* Record the statistical measures of the minimized aut. */
    testData_h.inc();
    testData_h.updateAvgReductions(autData_h, autData_i);
    log_autReduction(log, autData_h, autData_i);

    /* Complement workaround */
    Automaton aut1 = getAut(autData_h);
    saveToFile(aut1, "aut_.timbuk", true);
    stateDict sDict;
    autData_h = parseFromFile("aut_.timbuk", sDict);

    /* Run Complement on the minimized aut and time it. */
    auto start_c = startTimer();
    AutData autData_c = complement(autData_h);
    float time_c = elapsed_sec(start_c);

    /* Record the statistical measures of the complemented aut. */
    testData_hAndc.inc();
    testData_hAndc.updateAvgSizes(autData_c);
    log_autSizes(log, autData_c);

    /* Record the accumulated time of the 2 operations. */
    testData_hAndc.updateAvgTime(time_h + time_c);
    log_time(log, time_h + time_c);

    return autData_c;
}

AutData call_complement_after_heavy(const AutData& autData_i, unsigned int la_dw, unsigned int la_up,
         MetaData& metaData, TestData& testData_h, TestData& testData_hAndc, TestData& testData_c,
         string log)
{
    metaData.inc();
    metaData.updateInitialAvg(autData_i);
    log_autSizes(log, autData_i);

    AutData aut_hAndc = complement_after_heavy(autData_i, la_dw, la_up, testData_h, testData_hAndc, log);

    complement(autData_i, testData_c, log);

    writeToFile(log, "\n");

    return aut_hAndc;
}

void complement_after_heavy_tester(unsigned int la_dw, unsigned int la_up,
                                   string input_dir, string output_dir, unsigned int numb_tests, string log_avgs)
{

    MetaData metaData       = MetaData();
    TestData testData_h     = TestData();
    TestData testData_hAndc = TestData();
    TestData testData_c     = TestData();

    string log = "log_of_comp_after_heavy(" + std::to_string(la_dw) + ","
            + std::to_string(la_up) + ")_input_folder:" + getLocalFilename(input_dir) + "_date:" + localTime() + ".txt";
    writeToFile(log, "Each row of the following table contains results of an experiment on a particular tree automaton. The columns should be read as follows: \n"
                "\t Filename - the name of the file containing the input tree automaton (TA) \n"
                "\t Q_i - the number of states of TA \n"
                "\t Dt_i - the number of transitions of TA \n"
                "\t TD_i - the transition density of TA \n"
               // "\t OL_i - the transitions overlapping in TA \n"
                "\t Q_h(%) - the percentage of the number of states that remained after the application of the Heavy algorithm on TA \n"
                "\t Dt_h(%) - the percentage of the number of transitions that remained after the application of the Heavy algorithm on TA \n"
                "\t TD_h(%) - the t.d. of TA vs the t.d. of Heavy(TA) \n"
                "\t Q_h+c - the number of states in the complement of Heavy(TA) \n"
                "\t Dt_h+c - the number of transitions in the complement of Heavy(TA) \n"
                "\t TD_h+c - the transition density of the complement of Heavy(TA) \n"
                "\t T_h+c - the time (in seconds) taken to compute Heavy(TA) and then its complement, altogether \n"
                "\t Q_c - the number of states in the complement of TA \n"
                "\t Dt_c - the number of transitions in the complement of TA \n"
                "\t TD_c - the transition density of the complement of TA \n"
                "\t T_c - the time (in seconds) taken to compute the complement of TA, alone \n \n");
    writeToFile(log, "Filename\tQ_i\tDt_i\tTD_i\t"
                "Q_h(%)\tDt_h(%)\tTD_h(%)\t"
                "Q_h+c\tDt_h+c\tTD_h+c\tT_h+c\t"
                "Q_c\tDt_c\tTD_c\tT_c \n");

    // Change the name of getAllSubFilenames to getAllFilenames ?
    vector<filename> filenames = getAllSubFilenames(input_dir);
    unsigned int numb_aut = !numb_tests ? filenames.size() : min(numb_tests, filenames.size());

    for (unsigned int i=0; i<numb_aut; i++)
    {        
        /* Parsing and taking data from the automaton. */
        stateDict sDict;
        AutData autData_i = parseFromFile(filenames.at(i), sDict);

        if (getNumbTransitions(autData_i) == 0)
        {
            string output_msg = "This automaton has 0 transitions and therefore will be skipped. \n";
            outputText(output_msg);
            continue;
        }

        string local_filename = getLocalFilename(filenames.at(i));
        writeToFile(log, local_filename + "\t");

        string header1  = "\tAutomaton " + std::to_string(i+1) + " - " + filenames.at(i) + ": ";
        outputText(header1);

        AutData aut_hAndc = call_complement_after_heavy(autData_i, la_dw, la_up, metaData, testData_h, testData_hAndc, testData_c, log);

        /* Output resulting automaton (optional). */
        if (output_dir != "")
        {
            string aut_hAndc_filename = appendTimbukFilename(sanitizeDirStr(output_dir) + local_filename,
                                    "_minimized_with_Heavy(" + std::to_string(la_dw) + "," +
                                    std::to_string(la_up) + ")_and_complemented");
            saveToFile(aut_hAndc, aut_hAndc_filename);
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
                    metaData.initial_avg_q_str2Dec() + "\t" + metaData.initial_avg_delta_str2Dec() + "\t" +
                    metaData.initial_avg_transDens_str2Dec() + "\t" +
                    testData_h.avg_q_red_str2Dec() + "\t" + testData_h.avg_delta_red_str2Dec() + "\t" +
                    testData_h.avg_transDens_red_str2Dec() + "\t" +
                    testData_hAndc.avg_q_str2Dec() + "\t" + testData_hAndc.avg_delta_str2Dec() + "\t" +
                    testData_hAndc.avg_transDens_str2Dec() + "\t" + testData_hAndc.avg_time_str2Dec() + "\t" +
                    testData_c.avg_q_str2Dec() + "\t" + testData_c.avg_delta_str2Dec() + "\t"
                    + testData_c.avg_transDens_str2Dec() + "\t" + testData_c.avg_time_str2Dec() + "\t" +
                    "\n");

}

void complement_after_heavy_tester(unsigned int la_dw, unsigned int la_up,
                                   vector<string> input_dirs, string output_dir, unsigned int numb_tests)
{
    string log_avgs = "log_of_avg_values_of_comp_after_heavy(" + std::to_string(la_dw) + "," +
                       std::to_string(la_up) + ")_date:" + localTime() + ".txt";
    writeToFile(log_avgs, "Each row of the following table contains avg. result values "
                          "respecting experiments on the automata in a given directory. "
                          "The columns should be read as follows: \n"
                          "\t Folder - the name of the folder that this avg. values correspond to \n"
                          "\t Q_i - the avg. number of states of the TA \n"
                          "\t Dt_i - the avg. number of transitions of the TA \n"
                          "\t TD_i - the avg. transition density of the TA \n"
                          // "\t OL_i - the transitions overlapping in TA \n"
                          "\t Q_h(%) - the avg. percentage of the number of states that remained after the application of the Heavy algorithm on the TA \n"
                          "\t Dt_h(%) - the avg. percentage of the number of transitions that remained after the application of the Heavy algorithm on the TA \n"
                          "\t TD_h(%) - the avg. relation: t.d. of the TA vs the t.d. of the Heavy(TA) \n"
                          "\t Q_h+c - the avg. number of states in the complement of Heavy(TA) \n"
                          "\t Dt_h+c - the avg. number of transitions in the complement of Heavy(TA) \n"
                          "\t TD_h+c - the avg. transition density of the complement of Heavy(TA) \n"
                          "\t T_h+c - the avg. time (in seconds) taken to compute Heavy(TA) and then its complement, altogether \n"
                          "\t Q_c - the avg. number of states in the complement of TA \n"
                          "\t Dt_c - the avg. number of transitions in the complement of TA \n"
                          "\t TD_c - the avg. transition density of the complement of TA \n"
                          "\t T_c - the avg. time (in seconds) taken to compute the complement of TA, alone \n \n");
    writeToFile(log_avgs, "Folder\tQ_i\tDt_i\tTD_i\t"
                "Q_h(%)\tDt_h(%)\tTD_h(%)\t"
                "Q_h+c\tDt_h+c\tTD_h+c\tT_h+c\t"
                "Q_c\tDt_c\tTD_c\tT_c \n");

    for (const string input_dir : input_dirs)
        complement_after_heavy_tester(la_dw, la_up, input_dir, output_dir, numb_tests, log_avgs);

    writeToFile(log_avgs, "\nThe End\n");

    outputText("\nThe End. Please consult file " + log_avgs + " for average result values for each input directory. \n");
}
