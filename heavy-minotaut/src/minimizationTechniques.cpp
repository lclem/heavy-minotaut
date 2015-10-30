
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file for the automata minimization procedures. It includes   *
 *  the procedures introduced in the literature (see CONCUR'15 paper in             *
 *  README file).                                                                   *
 * 																					*
 ************************************************************************************/

#include "minimizationTechniques.hh"

/* Encodings */
const unsigned int DW_LA_1ST_TIME         = 0;
const unsigned int UP_LA_WITH_ID_1ST_TIME = 1;
const unsigned int STRICT_UP_1_WITH_ID    = 2;
const unsigned int DW_LA_2ND_TIME         = 3;
const unsigned int UP_LA_WITH_ID_2ND_TIME = 4;
const unsigned int DW_1                   = 5;
const unsigned int UP_LA_WITH_DW_1        = 6;
const unsigned int numbRelations          = 7;

const unsigned int QUOT_WITH_DW_LA             = 0;
const unsigned int QUOT_WITH_UP_LA_WITH_ID_1ST = 1;
const unsigned int QUOT_WITH_UP_LA_WITH_ID_2ND = 2;
const unsigned int numbQuotientings            = 3;

const unsigned int PRUN_WITH_P_ID_AND_DW_LA             = 0;
const unsigned int PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA   = 1;
const unsigned int PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1 = 2;
const unsigned int numbPrunings                         = 3;


AutData removeDeadStates(const AutData& autData)
{
    const Automaton& aut_i = getAut(autData);

    Automaton aut_f = removeInitialState(aut_i);

    aut_f = aut_f.RemoveUselessStates();

    aut_f = addInitialState(aut_f);

    return wrapAutData(aut_f,getRanks(autData));
}

AutData applyQuotDwLa(const AutData& autData,
                      unsigned int la_dw,
                      unsigned int greatest_state,
                      unsigned int greatest_symbol,
                      string log_humanread_filename)
{

    const Automaton aut_i = getAut(autData);
    greatest_state  = !greatest_state  ? getGreatestUsedState(aut_i)     : greatest_state;
    greatest_symbol = !greatest_symbol ? getGreatestUsedSymbol(aut_i)    : greatest_symbol;

    auto start   = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type.

    /* Common declarations. */
    AutData aut = removeUselessStates(autData);

    /* Computing the dw-la sim. */
    outputText("Computing the dw-" + std::to_string(la_dw) + " simulation... \n", log_humanread_filename);
    vector<vector<bool> > dw_la;
    start = std::chrono::high_resolution_clock::now();
    dw_la = dw_simulation(aut, la_dw);
    elapsed = std::chrono::high_resolution_clock::now() - start;

    /* Quotient with dw-la sim. */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_quot = quotientAutomaton(aut,transClosure(dw_la));
    elapsed = std::chrono::high_resolution_clock::now() - start;

    return aut_quot;
}

AutData applyQuotAndPrunDwLa(const AutData& autData_i,
                           unsigned int la_dw,
                           unsigned int greatest_state  = 0,
                           unsigned int greatest_symbol = 0,
                           string log_humanread_filename = "")
{

    const Automaton aut_i = getAut(autData_i);
    greatest_state  = !greatest_state  ? getGreatestUsedState(aut_i)     : greatest_state;
    greatest_symbol = !greatest_symbol ? getGreatestUsedSymbol(aut_i)    : greatest_symbol;

    auto start   = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    // Just setting the auto type.

    /* Common declarations. */
    AutData aut = removeUselessStates(autData_i);

    /* Computing the dw-la sim. */
    outputText("Computing the dw-" + std::to_string(la_dw) + " simulation... \n", log_humanread_filename);
    vector<vector<bool> > dw_la;
    start = std::chrono::high_resolution_clock::now();
    dw_la = dw_simulation(aut, la_dw);
    elapsed = std::chrono::high_resolution_clock::now() - start;

    /* Quotient with dw-la sim. */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_quot = quotientAutomaton(aut,transClosure(dw_la/*, greatest_state+1*/)/*, greatest_state+1*/);
    elapsed = std::chrono::high_resolution_clock::now() - start;

    /* Prune with P(id,dw-la sim). */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_prun = prune(aut_quot, generateIdRelation(greatest_state+1), dw_la, true);
    /* We must generate the id relation based on the number of states which existed before the quotienting,
     * since the quotienting does not force the reindexing of the states.
     * Alternatively, we could try to force this reindexation.
     * libvata provides a method for the reindexation of the states, but it doesn't appear to be preserving the lang.' */
    elapsed = std::chrono::high_resolution_clock::now() - start;

    return aut_prun;

}

AutData applyMinimizationSequence(const AutData& autData_i,
                                  unsigned int la_dw, unsigned int la_up,
                                  unsigned int greatest_state  = 0,
                                  unsigned int greatest_symbol = 0,
                                  timespec* timeout_start = NULL,
                                  TestData& testData = emptyTestData,
                                  string log_humanread_filename = "")
{

    const Automaton aut_i = getAut(autData_i);
    greatest_state  = !greatest_state  ? getGreatestUsedState(aut_i)   : greatest_state;
    greatest_symbol = !greatest_symbol ? getGreatestUsedSymbol(aut_i)    : greatest_symbol;

    auto start   = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type.

    vector<unsigned int> sizesRel(numbRelations);
    vector<float>        numbRefs(numbRelations);
    vector<float>        timesRel(numbRelations);
    vector<float>        timesQuot(numbQuotientings);
    vector<float>        timesPrun(numbPrunings);

    /* Common declarations. */
    float refin = 0;
    AutData aut = removeUselessStates(autData_i);

    /* Computing the dw-la sim. */
    if (log_humanread_filename != "")
        outputText("Computing the dw-" + std::to_string(la_dw) + " simulation... \n", log_humanread_filename);
    vector<vector<bool> > dw_la;
    start = std::chrono::high_resolution_clock::now();

    dw_la = dw_simulation(aut, la_dw, &refin, timeout_start);

    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_LA_1ST_TIME]
            = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_LA_1ST_TIME] = getSizeOfRel(dw_la);

    numbRefs[DW_LA_1ST_TIME] = refin;
    if (log_humanread_filename != "") outputText("The dw-" + std::to_string(la_dw) + " simulation has "
               + std::to_string(sizesRel[DW_LA_1ST_TIME]) + " pairs ", log_humanread_filename);
    if (log_humanread_filename != "")
            outputText(" and took " + std::to_string(numbRefs[DW_LA_1ST_TIME]) + " matrix-refinements and "
                       + std::to_string(timesRel[DW_LA_1ST_TIME]) + " seconds to compute. ", log_humanread_filename);

    /* Quotient with dw-la sim. */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_quot = quotientAutomaton(aut,transClosure(dw_la));
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesQuot[QUOT_WITH_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("The quotienting took " + std::to_string(timesQuot[QUOT_WITH_DW_LA])
               + " seconds ", log_humanread_filename);

    /* Prune with P(id,dw-la sim). */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_prun = prune(aut_quot, generateIdRelation(getGreatestUsedState(aut)+1), dw_la, true);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesPrun[PRUN_WITH_P_ID_AND_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("and the pruning took " + std::to_string(timesPrun[PRUN_WITH_P_ID_AND_DW_LA]) + " seconds. ",
                log_humanread_filename);

    AutData aut_ru = removeUselessStates(aut_prun);

    /* Computing the up-la(id) sim. */
    if (log_humanread_filename != "") outputText("\nComputing the up-" + std::to_string(la_up) + "(id) simulation... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_id_1st_time =
            up_simulation(aut_ru, la_up, generateIdRelation(greatest_state+1), false,
                          greatest_state+1, greatest_symbol+1, &refin, timeout_start);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_ID_1ST_TIME]
            = getSizeOfRel(up_la_with_id_1st_time);
    numbRefs[UP_LA_WITH_ID_1ST_TIME] = refin;
    timesRel[UP_LA_WITH_ID_1ST_TIME] =
            std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("The up-" + std::to_string(la_up) + "(id) simulation has "
              + std::to_string(sizesRel[UP_LA_WITH_ID_1ST_TIME]) + " pairs and it took "
              + std::to_string(numbRefs[UP_LA_WITH_ID_1ST_TIME]) + " matrix-refinements and "
              + std::to_string(timesRel[UP_LA_WITH_ID_1ST_TIME]) + " seconds to compute and ",
               log_humanread_filename);

    /* Quotienting with the up-la(id) sim the 1st time. */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_quot2 = quotientAutomaton(aut_ru, transClosure(up_la_with_id_1st_time));
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesQuot[QUOT_WITH_UP_LA_WITH_ID_1ST] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("the quotienting took " + std::to_string(timesQuot[QUOT_WITH_UP_LA_WITH_ID_1ST]) + " seconds. ",
                   log_humanread_filename);

    /* Prune with P(strict up-la(id) sim, id) */
    extractStrictRelation(up_la_with_id_1st_time/*, greatest_state+1*/);
    start = std::chrono::high_resolution_clock::now();
    AutData aut_prun2 = prune(aut_quot2, up_la_with_id_1st_time, generateIdRelation(greatest_state+1), false);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("The pruning with P(strict up-la(id) sim, id) took " + std::to_string(seconds) + " seconds. ",
               log_humanread_filename);

    AutData aut_ru2 = removeUselessStates(aut_prun2);

    /* Computing the strict up-1(id) sim. */
    if (log_humanread_filename != "") outputText("\nComputing the strict up-1(id) simulation... \n", log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > strict_up_1_with_id
            = up_simulation_strict(aut_ru2, 1, generateIdRelation(greatest_state+1), false,
                                   greatest_state+1, greatest_symbol+1, &refin, timeout_start);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[STRICT_UP_1_WITH_ID] = getSizeOfRel(strict_up_1_with_id);
    numbRefs[STRICT_UP_1_WITH_ID] = refin;
    timesRel[STRICT_UP_1_WITH_ID] =
            std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("The strict up-1(id) simulation has " + std::to_string(sizesRel[STRICT_UP_1_WITH_ID])
              + " pairs and the computation of the relation took "
              + std::to_string(numbRefs[STRICT_UP_1_WITH_ID]) + " matrix-refinements and "
              + std::to_string(timesRel[STRICT_UP_1_WITH_ID]) + " seconds. ", log_humanread_filename);

    /* Computing the dw-la sim again. */
    if (log_humanread_filename != "") outputText("\nComputing the dw-" + std::to_string(la_dw) + " simulation again... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();

    dw_la = dw_simulation(aut_ru2, la_dw, &refin, timeout_start);

    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_LA_2ND_TIME] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_LA_2ND_TIME] = getSizeOfRel(dw_la);
    numbRefs[DW_LA_2ND_TIME] = refin;
    if (log_humanread_filename != "") outputText("The dw-" + std::to_string(la_dw) + " simulation has " + std::to_string(sizesRel[DW_LA_2ND_TIME])
              + " pairs and took " + std::to_string(numbRefs[DW_LA_2ND_TIME])
              + " matrix-refinements and " + std::to_string(timesRel[DW_LA_2ND_TIME])
              + " seconds to compute. ",
              log_humanread_filename);

    /* Pruning with P(strict up-1(id) sim, dw-la sim) */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_prun3 = prune(aut_ru2, strict_up_1_with_id, dw_la, false);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesPrun[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA]
            = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("The pruning took " + std::to_string(timesPrun[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA]) + " seconds. ",
               log_humanread_filename);

    AutData aut_ru3 = removeUselessStates(aut_prun3);

    /* Computing the up-la(id) sim. again */
    if (log_humanread_filename != "")
        outputText("\nComputing the up-" + std::to_string(la_up) + "(id) simulation again... \n", log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_id_2nd_time
            = up_simulation(aut_ru3, la_up, generateIdRelation(greatest_state+1), false,
                            greatest_state+1, greatest_symbol+1, &refin, timeout_start);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_ID_2ND_TIME]
            = getSizeOfRel(up_la_with_id_2nd_time);
    numbRefs[UP_LA_WITH_ID_2ND_TIME] = refin;
    timesRel[UP_LA_WITH_ID_2ND_TIME] =
            std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("The up-" + std::to_string(la_up) + "(id) simulation has "
              + std::to_string(sizesRel[UP_LA_WITH_ID_2ND_TIME]) + " pairs and it took "
              + std::to_string(numbRefs[UP_LA_WITH_ID_2ND_TIME]) + " matrix-refinements and "
              + std::to_string(timesRel[UP_LA_WITH_ID_2ND_TIME]) + " seconds to compute and ",
               log_humanread_filename);

    /* Quotienting again with the up-la(id) sim. */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_quot3 = quotientAutomaton(aut_ru3,
                            transClosure(up_la_with_id_2nd_time));
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesQuot[QUOT_WITH_UP_LA_WITH_ID_2ND] =
            std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("the quotienting took " + std::to_string(timesQuot[QUOT_WITH_UP_LA_WITH_ID_2ND]) + " seconds. ",
               log_humanread_filename);

    /* Computing the dw-1 sim. */
    if (log_humanread_filename != "") outputText("\nComputing the dw-1 simulation... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > dw_1;
    dw_1 = dw_simulation(aut_quot3, 1);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_1] = getSizeOfRel(dw_1);
    numbRefs[DW_1] = refin;
    if (log_humanread_filename != "") outputText("The dw-1 simulation has " + std::to_string(sizesRel[DW_1])
               + " and took " + std::to_string(timesRel[DW_1]) + " seconds to compute. ",
               log_humanread_filename);

    /* Computing the up-la(dw-1) sim */
    if (log_humanread_filename != "") outputText("\nComputing the up-" + std::to_string(la_up) + "(dw-1) simulation... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_dw_1
            = up_simulation(aut_quot3, la_up, dw_1, false, greatest_state+1, greatest_symbol+1, &refin, timeout_start);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_DW_1] = getSizeOfRel(up_la_with_dw_1);
    numbRefs[UP_LA_WITH_DW_1] = refin;
    timesRel[UP_LA_WITH_DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("The up-" + std::to_string(la_up) + "(dw-1) simulation has "
              + std::to_string(sizesRel[UP_LA_WITH_DW_1])
              + " pairs and the computation of the relation took "
              + std::to_string(numbRefs[UP_LA_WITH_DW_1]) + " matrix-refinements and "
              + std::to_string(timesRel[UP_LA_WITH_DW_1]) + " seconds to compute, ",
               log_humanread_filename);

    /* Pruning with P(up-la(dw-1) sim, strict dw-1 sim) */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_prun4 = prune(aut_quot3, up_la_with_dw_1, dw_1, true);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesPrun[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("the pruning took " + std::to_string(timesPrun[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1]) + " seconds. \n",
               log_humanread_filename);

    AutData aut_ru4 = removeUselessStates(aut_prun4);

    if (testData.numb_relations > 0)
    {
        testData.inc();
        testData.updateAvgSizesRels(sizesRel);
        testData.updateAvgNumbsRefs(numbRefs);
        testData.updateAvgTimes(timesRel, timesQuot, timesPrun);
    }

    std::cout << std::flush;

    return aut_ru4;
}

AutData quotient_with_combined_relation(const AutData& autData, unsigned int la_dw, unsigned int la_up,
                                          unsigned int numb_states, unsigned int greatest_symbol,
                                          timespec* timeout_start, unsigned int timeout)
{

    const Automaton aut = getAut(autData);

    numb_states  = !numb_states  ? getGreatestUsedState(aut)+1      : numb_states;
    greatest_symbol = !greatest_symbol ? getGreatestUsedSymbol(aut) : greatest_symbol;

    vector<vector<bool> > W_combined =
            combined_relation(autData, la_dw, la_up, numb_states, greatest_symbol, timeout_start, timeout);

    return quotientAutomaton(autData, W_combined);

}

AutData heavy(const AutData& autData, unsigned int la_dw, unsigned int la_up,
              TestData& testData, string log_humanread_filename)
{

    Automaton aut_i = copyAut(getAut(autData)), aut_f;
    const vector<typerank> ranks = getRanks(autData);
    symbol greatest_symbol  =  getGreatestUsedSymbol(aut_i);

    timespec timeout_start;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timeout_start);

    while(true)
    {
        if (la_dw>1 || la_up>1)
        {
            while(true)
            {

                aut_f = getAut(applyMinimizationSequence(wrapAutData(aut_i,ranks), 1, 1,
                                                         getGreatestUsedState(aut_i),
                                                         greatest_symbol,
                                                         &timeout_start, emptyTestData,
                                                         log_humanread_filename));
                if (equalSize(aut_i, aut_f)) break;
                else aut_i = aut_f;

            }
        }
        aut_f = getAut(applyMinimizationSequence(wrapAutData(aut_i,ranks), la_dw, la_up,
                                                 getGreatestUsedState(aut_i),
                                                 greatest_symbol,
                                                 &timeout_start, testData,
                                                 log_humanread_filename));
        if (equalSize(aut_i, aut_f)) return wrapAutData(aut_f,ranks);
        else aut_i = aut_f;
    }

    return wrapAutData(aut_f,ranks);

}

AutData heavyGradual(const AutData& autData_i,
                     unsigned int la_dw, unsigned int la_dw_max,
                     unsigned int greatest_symbol,
                     TestData& testData, timespec* timeout_start, string log_humanread_filename)
{

    if (la_dw > la_dw_max) return autData_i;

    AutData autData_f = applyMinimizationSequence(autData_i,
                                                  la_dw, ((unsigned int) pow(2.0, la_dw)) - 1,
                                                  getGreatestUsedState(getAut(autData_i)),
                                                  greatest_symbol,
                                                  timeout_start, testData, log_humanread_filename);

    if (!equalSize(autData_i,autData_f))
        return heavyGradual(autData_f, 1, la_dw_max, greatest_symbol, testData, timeout_start, log_humanread_filename);
    else
        return heavyGradual(autData_f, ++la_dw, la_dw_max, greatest_symbol, testData, timeout_start, log_humanread_filename);

}

void applyProcedure(const AutData& autData_i,   stateDict sDict,
                     MetaData& metaData,        TestData& testData_ru,
                    TestData& testData_quot,    Timeout& timeout_quot,
                    TestData& testData_qAndP,   Timeout& timeout_qAndP,
                    TestData& testData_heavy11, TestData& testData_heavy11_rels, Timeout& timeout_heavy11,
                    TestData& testData_heavy24, TestData& testData_heavy24_rels, Timeout& timeout_heavy24,
                    TestData& testData_heavy37, TestData& testData_heavy37_rels, Timeout& timeout_heavy37,
                    string log_humanread_filename, string log_machread_filename,
                    string log_machread_heavy_times_filename)
{
    Automaton aut_i = getAut(autData_i);

    /* Declarations and initializations independent of each reducing method. */
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

    if (log_machread_heavy_times_filename != "")
        writeToFile(log_machread_heavy_times_filename,
                    std::to_string(getNumbUsedStates(aut_i)) + "\t"
                    + std::to_string(getNumbTransitions(aut_i)) + "\t");

    state greatest_state    =  getGreatestUsedState(aut_i);
    symbol greatest_symbol  =  getGreatestUsedSymbol(aut_i);

    /* Applying Remove Useless States */
    string headline = "\n1) Removing Useless States... ";
    outputText(headline, log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    auto start = std::chrono::high_resolution_clock::now();
    AutData autData_ru = removeUselessStates(autData_i);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

    Automaton aut_ru     =  getAut(autData_ru);
    float q_red          =  measureStatesReduction(aut_ru, aut_i);
    float delta_red      =  measureTransitionsReduction(aut_ru, aut_i);
    float transDens_red  =  measureTransDensReduction(aut_ru, aut_i);
    testData_ru.inc();
    testData_ru.updateAvgReductions(q_red, delta_red, transDens_red);
    testData_ru.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData_ru.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData_ru.updateAvgTime(seconds);

    string q_red_str = std::to_string(q_red);
    string delta_red_str = std::to_string(delta_red);
    string transDens_red_str = std::to_string(transDens_red);

    outputText("  The resulting automaton has "
               + q_red_str + "% of its states, "
               + delta_red_str + "% of its transitions and "
               + transDens_red_str + "% of its transition density. \n",
               log_humanread_filename);

    if (log_machread_filename != "")
        writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");

    if (log_machread_heavy_times_filename != "")
        writeToFile(log_machread_heavy_times_filename,
                    q_red_str + "\t"
                    + delta_red_str + "\t"
                    + std::to_string(seconds) + "\t");


    /* Applying Quotienting with dw-1 sim. */
    headline = "2) Quotienting with dw-1 sim... ";
    outputText("\n" + headline, log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    try
    {
        auto start = std::chrono::high_resolution_clock::now();
        AutData autData_quot = applyQuotDwLa(autData_i, 1,
                                 greatest_state, greatest_symbol,
                                  log_humanread_filename);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

        Automaton aut_quot = getAut(autData_quot);
        if ( /*equiv(aut_quot, aut_i)*/ true )
        {
            float q_red          =  measureStatesReduction(aut_quot, aut_i);
            float delta_red      =  measureTransitionsReduction(aut_quot, aut_i);
            float transDens_red  =  measureTransDensReduction(aut_quot, aut_i);
            testData_quot.inc();
            testData_quot.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_quot.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_quot.updateReductionBuckets(q_red, delta_red, transDens_red);
            testData_quot.updateAvgTime(seconds);

            string q_red_str = std::to_string(q_red);
            string delta_red_str = std::to_string(delta_red);
            string transDens_red_str = std::to_string(transDens_red);
            outputText("  The resulting automaton has "
                      + q_red_str         + "% of its states, "
                      + delta_red_str     + "% of its transitions and "
                      + transDens_red_str + "% of its transition density. \n",
                       log_humanread_filename);

            if (log_machread_filename != "")
                writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");

            if (log_machread_heavy_times_filename != "")
                writeToFile(log_machread_heavy_times_filename,
                            q_red_str + "\t"
                            + delta_red_str + "\t"
                            + std::to_string(seconds) + "\t");

        }
        else
        {
            printAut(aut_i, &sDict);
            printAut(aut_quot, &sDict);
            exit_with_error("The Quotienting method did NOT preserve the language!\n");
        }
    }
    catch (timeout_& e)
    {
        outputText("TIMEOUT: The Quotienting method took longer than "
                  + std::to_string( (float) TIMEOUT/60 ) + " minutes and therefore was aborted.\n",
                   log_humanread_filename);
        timeout_quot.inc();
        timeout_quot.checkSmallest(getNumbUsedStates(aut_i),
                                    getNumbTransitions(aut_i),
                                    getTransitionDensity(aut_i));

        testData_quot.inc();
        testData_quot.updateAvgReductions(100, 100, 100);
        testData_quot.checkGreatestReductions(100, 100, 100);
        testData_quot.updateReductionBuckets(100, 100, 100);
        testData_quot.updateAvgTime(TIMEOUT);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, "TO\tTO\tTO\t");

        if (log_machread_heavy_times_filename != "")
            writeToFile(log_machread_heavy_times_filename,
                        "100\t100\tTO\t");
    }

    /* Applying Quotienting & Pruning with dw-1 sim. */
    headline = "3) Quot. with dw-1 sim. + Prun. with P(id,dw-1 sim.)... ";
    outputText("\n" + headline + "\n", log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);


    try
    {
        auto start = std::chrono::high_resolution_clock::now();
        AutData autData_qAndP = applyQuotAndPrunDwLa(autData_i, 1,
                                         greatest_state,
                                         greatest_symbol,
                                         log_humanread_filename);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        float seconds =
                std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

        Automaton aut_qAndP  =  getAut(autData_qAndP);
        if ( /*equiv(aut_qAndP, aut_i)*/ true )
        {
            float q_red          =  measureStatesReduction(aut_qAndP, aut_i);
            float delta_red      =  measureTransitionsReduction(aut_qAndP, aut_i);
            float transDens_red  =  measureTransDensReduction(aut_qAndP, aut_i);
            testData_qAndP.inc();
            testData_qAndP.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_qAndP.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_qAndP.updateReductionBuckets(q_red, delta_red, transDens_red);
            testData_qAndP.updateAvgTime(seconds);

            string q_red_str         = std::to_string(q_red);
            string delta_red_str     = std::to_string(delta_red);
            string transDens_red_str = std::to_string(transDens_red);

            outputText("  The resulting automaton has "
                       + q_red_str + "% of its states, "
                       + delta_red_str + "% of its transitions and "
                       + transDens_red_str + "% of its transition density. \n",
                       log_humanread_filename);

            if (log_machread_filename != "")
                writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str + "\t"
                            + transDens_red_str + "\n");

            if (log_machread_heavy_times_filename != "")
                writeToFile(log_machread_heavy_times_filename,
                            q_red_str + "\t"
                            + delta_red_str + "\t"
                            + std::to_string(seconds) + "\t");
        }
        else
        {
            printAut(aut_i, &sDict);
            printAut(aut_qAndP, &sDict);
            exit_with_error("The Quotienting&Pruning method did NOT preserve the language!\n");
        }
    }
    catch (timeout_& e)
    {
        std::cout << "TIMEOUT: The Quotienting&Pruning method took longer than "
                  << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
        timeout_qAndP.inc();
        timeout_qAndP.checkSmallest(getNumbUsedStates(aut_i),
                                    getNumbTransitions(aut_i),
                                    getTransitionDensity(aut_i));

        testData_qAndP.inc();
        testData_qAndP.updateAvgReductions(100, 100, 100);
        testData_qAndP.checkGreatestReductions(100, 100, 100);
        testData_qAndP.updateReductionBuckets(100, 100, 100);
        testData_qAndP.updateAvgTime(TIMEOUT);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, "TO\tTO\tTO\t");

        if (log_machread_heavy_times_filename != "")
            writeToFile(log_machread_heavy_times_filename,
                        "TO\tTO\tTO\t");
    }

    /* Applying Heavy(1,1). */
    unsigned int la_dw = 1, la_up = 1;
    headline = "4) Applying Heavy(" + std::to_string(la_dw) + ", " + std::to_string(la_up) + ")... ";
    outputText("\n" + headline + "\n", log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    try
    {
        auto start = std::chrono::high_resolution_clock::now();
        AutData autData_heavy11 = heavy(autData_i, la_dw, la_up,
                               testData_heavy11_rels, log_humanread_filename);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

        Automaton aut_heavy11 = getAut(autData_heavy11);
        if ( /*equiv(aut_heavy11, aut_i)*/ true )
        {
            float q_red           =  measureStatesReduction(aut_heavy11, aut_i);
            float delta_red       =  measureTransitionsReduction(aut_heavy11, aut_i);
            float transDens_red   =  measureTransDensReduction(aut_heavy11, aut_i);
            testData_heavy11.inc();
            testData_heavy11.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_heavy11.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_heavy11.updateReductionBuckets(q_red, delta_red, transDens_red);
            testData_heavy11.updateAvgTime(seconds);

            string q_red_str          =  std::to_string(q_red);
            string delta_red_str      =  std::to_string(delta_red);
            string transDens_red_str  =  std::to_string(transDens_red);

            outputText("  The resulting automaton has "
                      + q_red_str + "% of its states, "
                      + delta_red_str + "% of its transitions and "
                      + transDens_red_str + "% of its transition density. \n",
                       log_humanread_filename);

            if (log_machread_filename != "")
                writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");

            if (log_machread_heavy_times_filename != "")
                writeToFile(log_machread_heavy_times_filename,
                            q_red_str + "\t"
                            + delta_red_str + "\t"
                            + std::to_string(seconds) + "\t");

        }
        else
        {
            printAut(aut_i, &sDict);
            printAut(aut_heavy11, &sDict);
            exit_with_error("The Heavy(1,1) method did NOT preserve the language!\n");
        }
    }
    catch (timeout_& e)
    {
        outputText("TIMEOUT: The Heavy(1,1) took longer than "
                  + std::to_string( (float) TIMEOUT/60 ) + " minutes and therefore was aborted.\n",
                   log_humanread_filename);
        timeout_heavy11.inc();
        timeout_heavy11.checkSmallest(getNumbUsedStates(aut_i), getNumbTransitions(aut_i), getTransitionDensity(aut_i));

        Automaton last_aut   =  e.getAut();
        float q_red          =  measureStatesReduction(last_aut, aut_i);
        float delta_red      =  measureTransitionsReduction(last_aut, aut_i);
        float transDens_red  =  measureTransDensReduction(last_aut, aut_i);
        testData_heavy11.inc();
        testData_heavy11.updateAvgReductions(q_red, delta_red, transDens_red);
        testData_heavy11.checkGreatestReductions(q_red, delta_red, transDens_red);
        testData_heavy11.updateReductionBuckets(q_red, delta_red, transDens_red);
        testData_heavy11.updateAvgTime(TIMEOUT);

        string q_red_str          =  std::to_string(q_red);
        string delta_red_str      =  std::to_string(delta_red);
        string transDens_red_str  =  std::to_string(transDens_red);

        outputText("  The resulting automaton has "
                  + q_red_str + "% of its states, "
                  + delta_red_str + "% of its transitions and "
                  + transDens_red_str + "% of its transition density. \n",
                   log_humanread_filename);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");

        if (log_machread_heavy_times_filename != "")
            writeToFile(log_machread_heavy_times_filename,
                        q_red_str + "\t"
                        + delta_red_str + "\t"
                        + "TO" + "\t");
    }

    /* Applying Heavy(2,4). */
    la_dw = 2;
    la_up = 4;
    headline = "5) Applying Heavy(" + std::to_string(la_dw) + ", " + std::to_string(la_up) + ")... ";
    outputText("\n" + headline + "\n", log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    try
    {
        auto start = std::chrono::high_resolution_clock::now();
        AutData autData_heavy24 = heavy(autData_i, la_dw, la_up,
                            testData_heavy24_rels, log_humanread_filename);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        float seconds =
                std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

        Automaton aut_heavy24 = getAut(autData_heavy24);
        if ( /*equiv(aut_heavy24, aut_i)*/ true )
        {
            float q_red           =  measureStatesReduction(aut_heavy24, aut_i);
            float delta_red       =  measureTransitionsReduction(aut_heavy24, aut_i);
            float transDens_red   =  measureTransDensReduction(aut_heavy24, aut_i);
            testData_heavy24.inc();
            testData_heavy24.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_heavy24.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_heavy24.updateReductionBuckets(q_red, delta_red, transDens_red);
            testData_heavy24.updateAvgTime(seconds);

            string q_red_str          =  std::to_string(q_red);
            string delta_red_str      =  std::to_string(delta_red);
            string transDens_red_str  =  std::to_string(transDens_red);

            outputText("  The resulting automaton has "
                      + q_red_str + "% of its states, "
                      + delta_red_str + "% of its transitions and "
                      + transDens_red_str + "% of its transition density. \n",
                       log_humanread_filename);

            if (log_machread_filename != "")
                writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str
                            + "\t" + transDens_red_str + "\n");

            if (log_machread_heavy_times_filename != "")
                writeToFile(log_machread_heavy_times_filename,
                            q_red_str + "\t"
                            + delta_red_str + "\t"
                            + std::to_string(seconds) + "\t");

        }
        else
        {
            printAut(aut_i, &sDict);
            printAut(aut_heavy24, &sDict);
            exit_with_error("The Heavy(2,4) method did NOT preserve the language!\n");
        }
    }
    catch (timeout_& e)
    {
        outputText("TIMEOUT: The Heavy(2,4) took longer than "
                  + std::to_string( (float) TIMEOUT/60 ) + " minutes and therefore was aborted.\n",
                   log_humanread_filename);
        timeout_heavy24.inc();
        timeout_heavy24.checkSmallest(getNumbUsedStates(aut_i), getNumbTransitions(aut_i), getTransitionDensity(aut_i));

        Automaton last_aut   =  e.getAut();
        float q_red          =  measureStatesReduction(last_aut, aut_i);
        float delta_red      =  measureTransitionsReduction(last_aut, aut_i);
        float transDens_red  =  measureTransDensReduction(last_aut, aut_i);
        testData_heavy24.inc();
        testData_heavy24.updateAvgReductions(q_red, delta_red, transDens_red);
        testData_heavy24.checkGreatestReductions(q_red, delta_red, transDens_red);
        testData_heavy24.updateReductionBuckets(q_red, delta_red, transDens_red);
        testData_heavy24.updateAvgTime(TIMEOUT);

        string q_red_str          =  std::to_string(q_red);
        string delta_red_str      =  std::to_string(delta_red);
        string transDens_red_str  =  std::to_string(transDens_red);

        outputText("  The resulting automaton has "
                  + q_red_str + "% of its states, "
                  + delta_red_str + "% of its transitions and "
                  + transDens_red_str + "% of its transition density. \n",
                   log_humanread_filename);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str
                        + "\t" + transDens_red_str + "\t");

        if (log_machread_heavy_times_filename != "")
            writeToFile(log_machread_heavy_times_filename,
                        q_red_str + "\t"
                        + delta_red_str + "\t"
                        + "TO" + "\t");

    }

    /* Applying Heavy(3,7). */
    la_dw = 3;
    la_up = 7;
    headline = "6) Applying Heavy(" + std::to_string(la_dw) + ", " + std::to_string(la_up) + ")... ";
    outputText("\n" + headline + "\n", log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    try
    {
        auto start = std::chrono::high_resolution_clock::now();
        AutData autData_heavy37 = heavy(autData_i, la_dw, la_up,
                               testData_heavy37_rels, log_humanread_filename);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

        Automaton aut_heavy37 = getAut(autData_heavy37);
        if ( /*equiv(aut_heavy37, aut_i)*/ true )
        {
            float q_red          =  measureStatesReduction(aut_heavy37, aut_i);
            float delta_red      =  measureTransitionsReduction(aut_heavy37, aut_i);
            float transDens_red  =  measureTransDensReduction(aut_heavy37, aut_i);
            testData_heavy37.inc();
            testData_heavy37.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_heavy37.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_heavy37.updateReductionBuckets(q_red, delta_red, transDens_red);
            testData_heavy37.updateAvgTime(seconds);

            string q_red_str          =  std::to_string(q_red);
            string delta_red_str      =  std::to_string(delta_red);
            string transDens_red_str  =  std::to_string(transDens_red);

            outputText("  The resulting automaton has "
                      + q_red_str + "% of its states, "
                      + delta_red_str + "% of its transitions and "
                      + transDens_red_str + "% of its transition density. \n",
                       log_humanread_filename);

            if (log_machread_filename != "")
                writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str
                            + "\t" + transDens_red_str + "\n");

            if (log_machread_heavy_times_filename != "")
                writeToFile(log_machread_heavy_times_filename,
                            q_red_str + "\t"
                            + delta_red_str + "\t"
                            + std::to_string(seconds) + "\n");

        }
        else
        {
            printAut(aut_i, &sDict);
            printAut(aut_heavy37, &sDict);
            exit_with_error("The Heavy(3,7) method did NOT preserve the language!\n");
        }
    }
    catch (timeout_& e)
    {
        outputText("TIMEOUT: The Heavy(3,7) took longer than "
                  + std::to_string( (float) TIMEOUT/60 ) + " minutes and therefore was aborted.\n",
                   log_humanread_filename);
        timeout_heavy37.inc();
        timeout_heavy37.checkSmallest(getNumbUsedStates(aut_i), getNumbTransitions(aut_i), getTransitionDensity(aut_i));

        Automaton last_aut   =  e.getAut();
        float q_red          =  measureStatesReduction(last_aut, aut_i);
        float delta_red      =  measureTransitionsReduction(last_aut, aut_i);
        float transDens_red  =  measureTransDensReduction(last_aut, aut_i);
        testData_heavy37.inc();
        testData_heavy37.updateAvgReductions(q_red, delta_red, transDens_red);
        testData_heavy37.checkGreatestReductions(q_red, delta_red, transDens_red);
        testData_heavy37.updateReductionBuckets(q_red, delta_red, transDens_red);
        testData_heavy37.updateAvgTime(TIMEOUT);

        string q_red_str          =  std::to_string(q_red);
        string delta_red_str      =  std::to_string(delta_red);
        string transDens_red_str  =  std::to_string(transDens_red);

        outputText("  The resulting automaton has "
                  + q_red_str + "% of its states, "
                  + delta_red_str + "% of its transitions and "
                  + transDens_red_str + "% of its transition density. \n",
                   log_humanread_filename);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str
                        + "\t" + transDens_red_str + "\n");

        if (log_machread_heavy_times_filename != "")
            writeToFile(log_machread_heavy_times_filename,
                        q_red_str + "\t"
                        + delta_red_str + "\t"
                        + "TO" + "\n");

    }

}

void applyHeavy(const AutData& autData_i, stateDict sDict, MetaData& metaData,
                unsigned int la_dw, unsigned int la_up,
                TestData& testData_heavy, Timeout& timeout_heavy,
                string log_machread_filename,
                string log_machread_heavy_times_filename)
{

    Automaton aut_i = getAut(autData_i);
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

    /* Applying Heavy(la_dw, la_up). */
    string headline = "- Applying Heavy(" + std::to_string(la_dw) + ", "
            + std::to_string(la_up) + ")... ";

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    auto start = std::chrono::high_resolution_clock::now();
    try
    {
        timespec timeout_start;
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timeout_start);
        AutData autData_heavy = heavy(autData_i, la_dw, la_up, testData_heavy);

        auto elapsed  = std::chrono::high_resolution_clock::now() - start;
        float seconds =
                std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

        Automaton aut_heavy = getAut(autData_heavy);
        if ( /*equiv(aut_heavy, aut_i)*/ true )
        {
            float q_red          =  measureStatesReduction(aut_heavy, aut_i);
            float delta_red      =  measureTransitionsReduction(aut_heavy, aut_i);
            float transDens_red  =  measureTransDensReduction(aut_heavy, aut_i);
            testData_heavy.inc();
            testData_heavy.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_heavy.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_heavy.updateReductionBuckets(q_red, delta_red, transDens_red);
            testData_heavy.updateAvgTime(seconds);

            string q_red_str          =  std::to_string(q_red);
            string delta_red_str      =  std::to_string(delta_red);
            string transDens_red_str  =  std::to_string(transDens_red);

            if (log_machread_filename != "")
                writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str
                            + "\t" + transDens_red_str + "\n");

            if (log_machread_heavy_times_filename != "")
                writeToFile(log_machread_heavy_times_filename,
                            q_red_str + "\t"
                            + delta_red_str + "\t"
                            + std::to_string(seconds) + "\n");

        }
        else
        {
            printAut(aut_i, &sDict);
            printAut(aut_heavy, &sDict);
            exit_with_error("The Heavy method did NOT preserve the language!\n");
        }
    }
    catch (timeout_& e)
    {
        timeout_heavy.inc();
        timeout_heavy.checkSmallest(getNumbUsedStates(aut_i), getNumbTransitions(aut_i), getTransitionDensity(aut_i));

        Automaton last_aut   =  e.getAut();
        float q_red          =  measureStatesReduction(last_aut, aut_i);
        float delta_red      =  measureTransitionsReduction(last_aut, aut_i);
        float transDens_red  =  measureTransDensReduction(last_aut, aut_i);
        testData_heavy.inc();
        testData_heavy.updateAvgReductions(q_red, delta_red, transDens_red);
        testData_heavy.checkGreatestReductions(q_red, delta_red, transDens_red);
        testData_heavy.updateReductionBuckets(q_red, delta_red, transDens_red);
        testData_heavy.updateAvgTime(TIMEOUT);

        string q_red_str          =  std::to_string(q_red);
        string delta_red_str      =  std::to_string(delta_red);
        string transDens_red_str  =  std::to_string(transDens_red);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str
                        + "\t" + transDens_red_str + "\n");

        if (log_machread_heavy_times_filename != "")
            writeToFile(log_machread_heavy_times_filename,
                        q_red_str + "\t"
                        + delta_red_str + "\t"
                        + "TO" + "\n");

    }

}

void applyQuotCombinedPreorder(const AutData& autData, stateDict sDict, MetaData& metaData,
                               unsigned int la_dw, unsigned int la_up,
                               TestData& testData, Timeout& timeout,
                               string log_humanread_filename,
                               string log_machread_filename,
                               string log_machread_times_filename)
{

    Automaton aut = getAut(autData);
    metaData.inc();
    metaData.updateInitialAvg(getNumbUsedStates(aut), getNumbTransitions(aut),
                               getNumbSymbols(aut),   getAvgRank(aut),
                               getTransitionDensity(aut),
                               measureTransOverlaps(aut));
    metaData.checkInitialGreatest(getNumbUsedStates(aut), getNumbTransitions(aut),
                                  getNumbSymbols(aut),    getAvgRank(aut),
                                  getTransitionDensity(aut),
                                  measureTransOverlaps(aut));

    string headline = "- Applying Quotienting with Combined Preorder(" + std::to_string(la_dw) + ", "
            + std::to_string(la_up) + ")... ";
    outputText(headline, log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    auto start = std::chrono::high_resolution_clock::now();
    timespec timeout_start;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timeout_start);

    AutData autData_ru = removeUselessStates(autData);

    try
    {
        AutData autData_quot =
                quotient_with_combined_relation(autData_ru, la_dw, la_up, 0, 0, &timeout_start);

        auto elapsed  = std::chrono::high_resolution_clock::now() - start;
        float seconds =
                std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

        Automaton aut_quot = getAut(autData_quot);
        if (/*equiv(aut,aut_quot)*/ true)
        {
            float q_red          =  measureStatesReduction(aut_quot, aut);
            float delta_red      =  measureTransitionsReduction(aut_quot, aut);
            float transDens_red  =  measureTransDensReduction(aut_quot, aut);
            testData.inc();
            testData.updateAvgReductions(q_red, delta_red, transDens_red);
            testData.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData.updateReductionBuckets(q_red, delta_red, transDens_red);
            testData.updateAvgTime(seconds);

            string q_red_str          =  std::to_string(q_red);
            string delta_red_str      =  std::to_string(delta_red);
            string transDens_red_str  =  std::to_string(transDens_red);

            outputText("  The resulting automaton has "
                       + q_red_str + "% of its states, "
                       + delta_red_str + "% of its transitions and "
                       + transDens_red_str + "% of its transition density. \n",
                       log_humanread_filename);

            if (log_machread_filename != "")
                writeToFile(log_machread_filename,
                            q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");

            if (log_machread_times_filename != "")
                writeToFile(log_machread_times_filename,
                            q_red_str + "\t" + delta_red_str + "\t" + std::to_string(seconds) + "\n");

        }
        else
        {
           printAut(aut, &sDict);
           printAut(aut_quot, &sDict);
           outputText("\n RU + Quotienting with Combined Preorder did NOT preserve the language!\n");
        }
    }
    catch (timeout_& e)
    {
        outputText("TIMEOUT: The Quotienting method took longer than "
                  + std::to_string( (float) TIMEOUT/60 )
                   + " minutes and therefore was aborted.\n",
                   log_humanread_filename);
        timeout.inc();
        timeout.checkSmallest(getNumbUsedStates(aut), getNumbTransitions(aut), getTransitionDensity(aut));

        float q_red          =  100.0;
        float delta_red      =  100.0;
        float transDens_red  =  100.0;
        testData.inc();
        testData.updateAvgReductions(q_red, delta_red, transDens_red);
        testData.checkGreatestReductions(q_red, delta_red, transDens_red);
        testData.updateReductionBuckets(q_red, delta_red, transDens_red);
        testData.updateAvgTime(TIMEOUT);

        string q_red_str          =  std::to_string(q_red);
        string delta_red_str      =  std::to_string(delta_red);
        string transDens_red_str  =  std::to_string(transDens_red);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename,
                        q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");

        if (log_machread_times_filename != "")
            writeToFile(log_machread_times_filename,
                        q_red_str + "\t" + delta_red_str + "\t" + "TO" + "\n");

    }

}
