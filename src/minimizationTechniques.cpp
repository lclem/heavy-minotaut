
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


Automaton removeDeadStates(const Automaton& aut_i)
{
    Automaton aut_f = removeInitialState(aut_i);

    aut_f = aut_f.RemoveUselessStates();

    return addInitialState(aut_f);
}

Automaton applyQuotDwLa(const Automaton& aut_i,
                    unsigned int la_dw, unsigned int greatest_state  = 0,
                    unsigned int greatest_symbol = 0,
                    vector<typerank> ranks = {},
                    bool use_lvata = false,
                    string log_humanread_filename = "")
/* Optional parameters: either use all of them or none. */
{
    greatest_state  = !greatest_state  ? getGreatestUsedState(aut_i)     : greatest_state;
    greatest_symbol = !greatest_symbol ? getGreatestUsedSymbol(aut_i)    : greatest_symbol;
    ranks           = ranks.empty()    ? getRanks(aut_i,greatest_symbol) : ranks;

    auto start   = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type.

    /* Common declarations. */
    float refin = 0;
    vector<bool> column(greatest_state+1);
    Automaton aut;

    aut = removeUselessStates(aut_i);

    //printAut(aut, &stateDict);

    /* Computing the dw-la sim. */
    outputText("Computing the dw-" + std::to_string(la_dw) + " simulation... \n", log_humanread_filename);
    vector<vector<bool> > dw_la(greatest_state+1, column);
    start = std::chrono::high_resolution_clock::now();
    if (la_dw==1 && use_lvata)
    {
        dw_ord_simulation_lvata(aut, dw_la, greatest_state+1);
        refin = 0;
    }
    else
    {
        refin = dw_simulation(aut, la_dw, dw_la, greatest_state+1, ranks);
    }
    elapsed = std::chrono::high_resolution_clock::now() - start;
    /*timesRel[DW_LA_1ST_TIME]
            = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_LA_1ST_TIME] = getSizeOfRel(dw_la, greatest_state+1);
    numbRefs[DW_LA_1ST_TIME] = refin;
    std::cout << "The dw-" << la_dw << " simulation has "
              << sizesRel[DW_LA_1ST_TIME]
              << " pairs and took " << numbRefs[DW_LA_1ST_TIME]
              << " matrix-refinements and "
              << timesRel[DW_LA_1ST_TIME] << " seconds to compute. ";*/

    //printW(aut, dw_la, greatest_state+1);

    /* Quotient with dw-la sim. */
    start = std::chrono::high_resolution_clock::now();
    aut = quotientAutomaton(aut,transClosure(dw_la, greatest_state+1), greatest_state+1);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    /*timesQuot[QUOT_WITH_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    std::cout << "The quotienting took " << timesQuot[QUOT_WITH_DW_LA] << " seconds ";*/

    return aut;
}

Automaton applyQuotAndPrunDwLa(const Automaton& aut_i,
                           unsigned int la_dw, unsigned int greatest_state  = 0,
                           unsigned int greatest_symbol = 0,
                           vector<typerank> ranks = {},
                           bool use_lvata = false,
                           string log_humanread_filename = "")
/* Optional parameters: either use all of them or none. */
{

    greatest_state  = !greatest_state  ? getGreatestUsedState(aut_i)     : greatest_state;
    greatest_symbol = !greatest_symbol ? getGreatestUsedSymbol(aut_i)    : greatest_symbol;
    ranks           = ranks.empty()    ? getRanks(aut_i,greatest_symbol) : ranks;

    auto start   = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    // Just setting the auto type.

    /* Common declarations. */
    float refin = 0;
    vector<bool> column(greatest_state+1);
    Automaton aut;

    aut = removeUselessStates(aut_i);

   // printAut(aut, &stateDict);

    /* Computing the dw-la sim. */
    outputText("Computing the dw-" + std::to_string(la_dw) + " simulation... \n", log_humanread_filename);
    vector<vector<bool> > dw_la(greatest_state+1, column);
    start = std::chrono::high_resolution_clock::now();
    if (la_dw==1 && use_lvata)
    {
        dw_ord_simulation_lvata(aut, dw_la, greatest_state+1);
        refin = 0;
    }
    else
    {
        refin = dw_simulation(aut, la_dw, dw_la, greatest_state+1, ranks);
    }
    elapsed = std::chrono::high_resolution_clock::now() - start;
/*    timesRel[DW_LA_1ST_TIME]
            = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_LA_1ST_TIME] = getSizeOfRel(dw_la, greatest_state+1);
    numbRefs[DW_LA_1ST_TIME] = refin;
    std::cout << "The dw-" << la_dw << " simulation has "
              << sizesRel[DW_LA_1ST_TIME]
              << " pairs and took " << numbRefs[DW_LA_1ST_TIME]
              << " matrix-refinements and "
              << timesRel[DW_LA_1ST_TIME] << " seconds to compute. ";*/

    //printW(aut, dw_la, greatest_state+1);

    /* Quotient with dw-la sim. */
    start = std::chrono::high_resolution_clock::now();
    aut = quotientAutomaton(aut,transClosure(dw_la, greatest_state+1), greatest_state+1);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    /*timesQuot[QUOT_WITH_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    std::cout << "The quotienting took " << timesQuot[QUOT_WITH_DW_LA] << " seconds ";*/

    //printAut(aut, &stateDict);

    /* Prune with P(id,dw-la sim). */
    start = std::chrono::high_resolution_clock::now();
    aut = prune(aut, generateIdRelation(greatest_state+1), dw_la, true);
    // We must generate the id relation based on the number of states which existed before the quotienting, since the quotienting does not force the reindexing of the states. Alternatively, we could try to force this reindexation.
    elapsed = std::chrono::high_resolution_clock::now() - start;
    /*timesPrun[PRUN_WITH_P_ID_AND_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    std::cout << "and the pruning took " << timesPrun[PRUN_WITH_P_ID_AND_DW_LA] << " seconds. ";*/

    //printAut(aut, &stateDict);

   // aut = removeUselessStates(aut);

    return aut;
}

/* A test function that focuses on checking if the lookahead dw and up simulation algorithms presented
   in this project are consistent (ie, using them on the appropriate minimization techniques preserves
   the language). */
Automaton applyMinimizationSequenceOld(const Automaton& aut_i,
                                      unsigned int la_dw, unsigned int la_up,
                                      unsigned int greatest_state  = 0,
                                      unsigned int greatest_symbol = 0,
                                      vector<typerank> ranks = {},
                                      bool use_lvata = false, timespec* timeout_start = NULL,
                                      TestData& testData = emptyTestData,
                                      string log_humanread_filename = "")
/* Optional parameters: either use all of them or none. */
{

    greatest_state  = !greatest_state  ? getGreatestUsedState(aut_i)     : greatest_state;
    greatest_symbol = !greatest_symbol ? getGreatestUsedSymbol(aut_i)    : greatest_symbol;
    ranks           = ranks.empty()    ? getRanks(aut_i,greatest_symbol) : ranks;

    auto start   = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type.

    vector<unsigned int> sizesRel(numbRelations);
    vector<float>        numbRefs(numbRelations);
    vector<float>        timesRel(numbRelations);
    vector<float>        timesQuot(numbQuotientings);
    vector<float>        timesPrun(numbPrunings);

    /* Common declarations. */
    float refin;
    vector<bool> column(greatest_state+1);
    Automaton aut;
    //printAut(aut_i, &stateDict);

    aut = removeUselessStates(aut_i);

    //printAut(aut, &stateDict);

    /* Computing the dw-la sim. */
    outputText("Computing the dw-" + std::to_string(la_dw) + " simulation... \n", log_humanread_filename);
    vector<vector<bool> > dw_la(greatest_state+1, column);
    start = std::chrono::high_resolution_clock::now();
    if (la_dw==1 && use_lvata)
    {
        dw_ord_simulation_lvata(aut, dw_la, greatest_state+1);
        refin = 0;
    }
    else
    {
        refin = dw_simulation(aut, la_dw, dw_la, greatest_state+1, ranks, timeout_start);
    }
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_LA_1ST_TIME]
            = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_LA_1ST_TIME] = getSizeOfRel(dw_la, greatest_state+1);
    numbRefs[DW_LA_1ST_TIME] = refin;
    outputText("The dw-" + std::to_string(la_dw) + " simulation has "
               + std::to_string(sizesRel[DW_LA_1ST_TIME]) + " pairs ", log_humanread_filename);
    if (!(la_dw==1 && use_lvata))
            outputText(" and took " + std::to_string(numbRefs[DW_LA_1ST_TIME]) + " matrix-refinements ",
                       log_humanread_filename);
    outputText("and " + std::to_string(timesRel[DW_LA_1ST_TIME]) + " seconds to compute. ",
               log_humanread_filename);

    //printW(aut, dw_la, greatest_state+1);

    /* Quotient with dw-la sim. */
    start = std::chrono::high_resolution_clock::now();
    aut = quotientAutomaton(aut,transClosure(dw_la, greatest_state+1), greatest_state+1);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesQuot[QUOT_WITH_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    outputText("The quotienting took " + std::to_string(timesQuot[QUOT_WITH_DW_LA])
               + " seconds ", log_humanread_filename);

    //printAut(aut, &stateDict);

    /* Prune with P(id,dw-la sim). */
    start = std::chrono::high_resolution_clock::now();
    aut = prune(aut, generateIdRelation(greatest_state+1), dw_la, true);
    // We must generate the id relation based on the number of states which existed before the quotienting, since the quotienting does not force the reindexing of the states. Alternatively, we could try to force this reindexation.
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesPrun[PRUN_WITH_P_ID_AND_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    outputText("and the pruning took " + std::to_string(timesPrun[PRUN_WITH_P_ID_AND_DW_LA]) + " seconds. ",
               log_humanread_filename);

    //printAut(aut, &stateDict);

    aut = removeUselessStates(aut);

    //printAut(aut, &stateDict);

    /* Computing the up-la(id) sim. */
    outputText("\nComputing the up-" + std::to_string(la_up) + "(id) simulation... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_id_1st_time(greatest_state+1, column);
    refin = up_simulation(aut, la_up, generateIdRelation(greatest_state+1), false,
                          up_la_with_id_1st_time, greatest_state+1, greatest_symbol+1, ranks);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_ID_1ST_TIME]
            = getSizeOfRel(up_la_with_id_1st_time, greatest_state+1);
    numbRefs[UP_LA_WITH_ID_1ST_TIME] = refin;
    timesRel[UP_LA_WITH_ID_1ST_TIME] =
            std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    outputText("The up-" + std::to_string(la_up) + "(id) simulation has "
              + std::to_string(sizesRel[UP_LA_WITH_ID_1ST_TIME]) + " pairs and it took "
              + std::to_string(numbRefs[UP_LA_WITH_ID_1ST_TIME]) + " matrix-refinements and "
              + std::to_string(timesRel[UP_LA_WITH_ID_1ST_TIME]) + " seconds to compute and ",
               log_humanread_filename);

    /* Quotienting with the up-la(id) sim the 1st time. */
    start = std::chrono::high_resolution_clock::now();
    aut = quotientAutomaton(aut, transClosure(up_la_with_id_1st_time,greatest_state+1), greatest_state+1);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesQuot[QUOT_WITH_UP_LA_WITH_ID_1ST] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    outputText("the quotienting took " + std::to_string(timesQuot[QUOT_WITH_UP_LA_WITH_ID_1ST])
              + " seconds. ", log_humanread_filename);

    //printAut(aut, &stateDict);




    /* Computing the strict up-1(id) sim. */
    outputText("\nComputing the strict up-1(id) simulation... \n", log_humanread_filename);
    vector<vector<bool> > strict_up_1_with_id (greatest_state+1, column);
    start = std::chrono::high_resolution_clock::now();
    refin = up_simulation_strict(aut, 1, generateIdRelation(greatest_state+1), false, strict_up_1_with_id, greatest_state+1, greatest_symbol+1, ranks);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[STRICT_UP_1_WITH_ID] = getSizeOfRel(strict_up_1_with_id, greatest_state+1);
    numbRefs[STRICT_UP_1_WITH_ID] = refin;
    timesRel[STRICT_UP_1_WITH_ID] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    outputText("The strict up-1(id) simulation has " + std::to_string(sizesRel[STRICT_UP_1_WITH_ID])
              + " pairs and the computation of the relation took "
              + std::to_string(numbRefs[STRICT_UP_1_WITH_ID]) + " matrix-refinements and "
              + std::to_string(timesRel[STRICT_UP_1_WITH_ID]) + " seconds. ", log_humanread_filename);

    /* Computing the dw-la sim again. */
    outputText("\nComputing the dw-" + std::to_string(la_dw) + " simulation again... \n",
               log_humanread_filename);
    dw_la.assign(greatest_state, column);
    start = std::chrono::high_resolution_clock::now();

    //refin = dw_simulation(aut, la_dw, dw_la, greatest_state+1, ranks, timeout);

    if (la_dw==1 && use_lvata)
    {
        dw_ord_simulation_lvata(aut, dw_la, greatest_state+1);
        refin = 0;
    }
    else
    {
        refin = dw_simulation(aut, la_dw, dw_la, greatest_state+1, ranks, timeout_start);
    }

    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_LA_2ND_TIME] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_LA_2ND_TIME] = getSizeOfRel(dw_la, greatest_state);
    numbRefs[DW_LA_2ND_TIME] = refin;
    outputText("The dw-" + std::to_string(la_dw) + " simulation has " + std::to_string(sizesRel[DW_LA_2ND_TIME])
              + " pairs and took " + std::to_string(numbRefs[DW_LA_2ND_TIME])
              + " matrix-refinements and " + std::to_string(timesRel[DW_LA_2ND_TIME])
              + " seconds to compute. ",
              log_humanread_filename);

    /* Pruning with P(strict up-1(id) sim, dw-la sim) */
    start = std::chrono::high_resolution_clock::now();
    aut = prune(aut, strict_up_1_with_id, dw_la, false);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesPrun[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA]
            = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    outputText("The pruning took " + std::to_string(timesPrun[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA])
               + " seconds. ",
               log_humanread_filename);

    //printAut(aut, &stateDict);

    aut = removeUselessStates(aut);

    //printAut(aut, &stateDict);

    /* Computing the up-la(id) sim. again */
    outputText("\nComputing the up-" + std::to_string(la_up) + "(id) simulation again... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_id_2nd_time(greatest_state+1, column);
    refin = up_simulation(aut, la_up, generateIdRelation(greatest_state+1), false,
                          up_la_with_id_2nd_time, greatest_state+1, greatest_symbol+1, ranks);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_ID_2ND_TIME]
            = getSizeOfRel(up_la_with_id_2nd_time, greatest_state+1);
    numbRefs[UP_LA_WITH_ID_2ND_TIME] = refin;
    timesRel[UP_LA_WITH_ID_2ND_TIME] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    outputText("The up-" + std::to_string(la_up) + "(id) simulation has "
              + std::to_string(sizesRel[UP_LA_WITH_ID_2ND_TIME]) + " pairs and it took "
              + std::to_string(numbRefs[UP_LA_WITH_ID_2ND_TIME]) + " matrix-refinements and "
              + std::to_string(timesRel[UP_LA_WITH_ID_2ND_TIME]) + " seconds to compute and ",
               log_humanread_filename);

    /* Quotienting again with the up-la(id) sim. */
    start = std::chrono::high_resolution_clock::now();
    aut = quotientAutomaton(aut,
                            transClosure(up_la_with_id_2nd_time, greatest_state+1),
                            greatest_state+1);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesQuot[QUOT_WITH_UP_LA_WITH_ID_2ND] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    outputText("the quotienting took "
              + std::to_string(timesQuot[QUOT_WITH_UP_LA_WITH_ID_2ND]) + " seconds. ",
               log_humanread_filename);

    //printAut(aut, &stateDict);

    /* Computing the dw-1 sim. */
    outputText("\nComputing the dw-1 simulation... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    //aut = removeInitialState(aut);
    vector<vector<bool> > dw_1 (greatest_state+1, column);
    dw_ord_simulation_lvata(aut, dw_1, greatest_state+1);
    refin = 0;
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_1] = getSizeOfRel(dw_1, greatest_state+1);
    numbRefs[DW_1] = refin;
    outputText("The dw-1 simulation has " + std::to_string(sizesRel[DW_1])
               + " and took " + std::to_string(timesRel[DW_1]) + " seconds to compute. ",
               log_humanread_filename);


    /* Computing the up-la(dw-1) sim */
    outputText("\nComputing the up-" + std::to_string(la_up) + "(dw-1) simulation... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_dw_1 (greatest_state+1,column);
    refin = up_simulation(aut, la_up, dw_1, false, up_la_with_dw_1,
                          greatest_state+1, greatest_symbol+1, ranks);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_DW_1] = getSizeOfRel(up_la_with_dw_1, greatest_state+1);
    numbRefs[UP_LA_WITH_DW_1] = refin;
    timesRel[UP_LA_WITH_DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    outputText("The up-" + std::to_string(la_up) + "(dw-1) simulation has "
              + std::to_string(sizesRel[UP_LA_WITH_DW_1])
              + " pairs and the computation of the relation took "
              + std::to_string(numbRefs[UP_LA_WITH_DW_1]) + " matrix-refinements and "
              + std::to_string(timesRel[UP_LA_WITH_DW_1]) + " seconds to compute, ",
               log_humanread_filename);

    /* Pruning with P(up-la(dw-1) sim, strict dw-1 sim) */
    start = std::chrono::high_resolution_clock::now();
    aut = prune(aut, up_la_with_dw_1, dw_1, true);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesPrun[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    outputText("the pruning took " + std::to_string(timesPrun[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1])
               + " seconds. \n",
               log_humanread_filename);

    //printAut(aut, &stateDict);

    aut = removeUselessStates(aut);

    //printAut(aut, &stateDict);

    if (testData.numb_relations > 0)
    {
        testData.inc();
        testData.updateAvgSizesRels(sizesRel);
        testData.updateAvgNumbsRefs(numbRefs);
        testData.updateAvgTimes(timesRel, timesQuot, timesPrun);
    }

    std::cout << std::flush;

    return aut;
}

/* A test function that focuses on checking if the lookahead dw and up simulation algorithms presented
   in this project are consistent (ie, using them on the appropriate minimization techniques preserves
   the language). */
Automaton applyMinimizationSequence(const Automaton& aut_i,
                                    unsigned int la_dw, unsigned int la_up,
                                    unsigned int greatest_state  = 0,
                                    unsigned int greatest_symbol = 0,
                                    vector<typerank> ranks = {},
                                    bool use_lvata = false,
                                    timespec* timeout_start = NULL,
                                    TestData& testData = emptyTestData,
                                    string log_humanread_filename = "",
                                    unsigned int timeout = 0)
/* Optional parameters: either use all of them or none. */
{

    greatest_state  = !greatest_state  ? getGreatestUsedState(aut_i)     : greatest_state;
    greatest_symbol = !greatest_symbol ? getGreatestUsedSymbol(aut_i)    : greatest_symbol;
    ranks           = ranks.empty()    ? getRanks(aut_i,greatest_symbol) : ranks;

    auto start   = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type.

    vector<unsigned int> sizesRel(numbRelations);
    vector<float>        numbRefs(numbRelations);
    vector<float>        timesRel(numbRelations);
    vector<float>        timesQuot(numbQuotientings);
    vector<float>        timesPrun(numbPrunings);

    /* Common declarations. */
    float refin;
    vector<bool> column(greatest_state+1);
    Automaton aut;

    aut = removeUselessStates(aut_i);

    /* Computing the dw-la sim. */
    if (log_humanread_filename != "") outputText("Computing the dw-" + std::to_string(la_dw) + " simulation... \n", log_humanread_filename);
    vector<vector<bool> > dw_la(greatest_state+1, column);
    start = std::chrono::high_resolution_clock::now();
    if (la_dw==1 && use_lvata)
    {
        dw_ord_simulation_lvata(aut, dw_la, greatest_state+1);
        refin = 0;
    }
    else
    {
        refin = dw_simulation(aut, la_dw, dw_la, greatest_state+1,
                              ranks, timeout_start);
    }
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_LA_1ST_TIME]
            = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_LA_1ST_TIME] = getSizeOfRel(dw_la, greatest_state+1);
    numbRefs[DW_LA_1ST_TIME] = refin;
    if (log_humanread_filename != "") outputText("The dw-" + std::to_string(la_dw) + " simulation has "
               + std::to_string(sizesRel[DW_LA_1ST_TIME]) + " pairs ", log_humanread_filename);
    if (!(la_dw==1 && use_lvata))
            if (log_humanread_filename != "") outputText(" and took " + std::to_string(numbRefs[DW_LA_1ST_TIME]) + " matrix-refinements ",
                       log_humanread_filename);
    if (log_humanread_filename != "") outputText("and " + std::to_string(timesRel[DW_LA_1ST_TIME]) + " seconds to compute. ",
               log_humanread_filename);

    /* Quotient with dw-la sim. */
    start = std::chrono::high_resolution_clock::now();
    aut = quotientAutomaton(aut,transClosure(dw_la, greatest_state+1), greatest_state+1);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesQuot[QUOT_WITH_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("The quotienting took " + std::to_string(timesQuot[QUOT_WITH_DW_LA])
               + " seconds ", log_humanread_filename);

    /* Prune with P(id,dw-la sim). */
    start = std::chrono::high_resolution_clock::now();
    aut = prune(aut, generateIdRelation(greatest_state+1), dw_la, true);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesPrun[PRUN_WITH_P_ID_AND_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("and the pruning took " + std::to_string(timesPrun[PRUN_WITH_P_ID_AND_DW_LA]) + " seconds. ",
               log_humanread_filename);

    aut = removeUselessStates(aut);

    /* Computing the up-la(id) sim. */
    if (log_humanread_filename != "") outputText("\nComputing the up-" + std::to_string(la_up) + "(id) simulation... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_id_1st_time(greatest_state+1, column);
    refin = up_simulation(aut, la_up, generateIdRelation(greatest_state+1), false,
                          up_la_with_id_1st_time, greatest_state+1, greatest_symbol+1, ranks);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_ID_1ST_TIME]
            = getSizeOfRel(up_la_with_id_1st_time, greatest_state+1);
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
    aut = quotientAutomaton(aut, transClosure(up_la_with_id_1st_time,greatest_state+1), greatest_state+1);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesQuot[QUOT_WITH_UP_LA_WITH_ID_1ST] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("the quotienting took " + std::to_string(timesQuot[QUOT_WITH_UP_LA_WITH_ID_1ST])
              + " seconds. ", log_humanread_filename);

    //printAut(aut, &stateDict);

    /* Prune with P(strict up-la(id) sim, id) */
    extractStrictRelation(up_la_with_id_1st_time, greatest_state+1);
    start = std::chrono::high_resolution_clock::now();
    aut = prune(aut, up_la_with_id_1st_time, generateIdRelation(greatest_state+1), false);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("The pruning with P(strict up-la(id) sim, id) took " + std::to_string(seconds) + " seconds. ",
               log_humanread_filename);

    aut = removeUselessStates(aut);

    /* Computing the strict up-1(id) sim. */
    if (log_humanread_filename != "") outputText("\nComputing the strict up-1(id) simulation... \n", log_humanread_filename);
    vector<vector<bool> > strict_up_1_with_id (greatest_state+1, column);
    start = std::chrono::high_resolution_clock::now();
    refin = up_simulation_strict(aut, 1, generateIdRelation(greatest_state+1), false, strict_up_1_with_id, greatest_state+1, greatest_symbol+1, ranks);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[STRICT_UP_1_WITH_ID] = getSizeOfRel(strict_up_1_with_id, greatest_state+1);
    numbRefs[STRICT_UP_1_WITH_ID] = refin;
    timesRel[STRICT_UP_1_WITH_ID] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("The strict up-1(id) simulation has " + std::to_string(sizesRel[STRICT_UP_1_WITH_ID])
              + " pairs and the computation of the relation took "
              + std::to_string(numbRefs[STRICT_UP_1_WITH_ID]) + " matrix-refinements and "
              + std::to_string(timesRel[STRICT_UP_1_WITH_ID]) + " seconds. ", log_humanread_filename);

    /* Computing the dw-la sim again. */
    if (log_humanread_filename != "") outputText("\nComputing the dw-" + std::to_string(la_dw) + " simulation again... \n",
               log_humanread_filename);
    dw_la.assign(greatest_state, column);
    start = std::chrono::high_resolution_clock::now();

    if (la_dw==1 && use_lvata)
    {
        dw_ord_simulation_lvata(aut, dw_la, greatest_state+1);
        refin = 0;
    }
    else
    {
        refin = dw_simulation(aut, la_dw, dw_la, greatest_state+1, ranks, timeout_start);
    }

    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_LA_2ND_TIME] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_LA_2ND_TIME] = getSizeOfRel(dw_la, greatest_state);
    numbRefs[DW_LA_2ND_TIME] = refin;
    if (log_humanread_filename != "") outputText("The dw-" + std::to_string(la_dw) + " simulation has " + std::to_string(sizesRel[DW_LA_2ND_TIME])
              + " pairs and took " + std::to_string(numbRefs[DW_LA_2ND_TIME])
              + " matrix-refinements and " + std::to_string(timesRel[DW_LA_2ND_TIME])
              + " seconds to compute. ",
              log_humanread_filename);

    /* Pruning with P(strict up-1(id) sim, dw-la sim) */
    start = std::chrono::high_resolution_clock::now();
    aut = prune(aut, strict_up_1_with_id, dw_la, false);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesPrun[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA]
            = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("The pruning took " + std::to_string(timesPrun[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA])
               + " seconds. ",
               log_humanread_filename);

    aut = removeUselessStates(aut);

    /* Computing the up-la(id) sim. again */
    if (log_humanread_filename != "") outputText("\nComputing the up-" + std::to_string(la_up) + "(id) simulation again... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_id_2nd_time(greatest_state+1, column);
    refin = up_simulation(aut, la_up, generateIdRelation(greatest_state+1), false,
                          up_la_with_id_2nd_time, greatest_state+1, greatest_symbol+1, ranks);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_ID_2ND_TIME]
            = getSizeOfRel(up_la_with_id_2nd_time, greatest_state+1);
    numbRefs[UP_LA_WITH_ID_2ND_TIME] = refin;
    timesRel[UP_LA_WITH_ID_2ND_TIME] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("The up-" + std::to_string(la_up) + "(id) simulation has "
              + std::to_string(sizesRel[UP_LA_WITH_ID_2ND_TIME]) + " pairs and it took "
              + std::to_string(numbRefs[UP_LA_WITH_ID_2ND_TIME]) + " matrix-refinements and "
              + std::to_string(timesRel[UP_LA_WITH_ID_2ND_TIME]) + " seconds to compute and ",
               log_humanread_filename);

    /* Quotienting again with the up-la(id) sim. */
    start = std::chrono::high_resolution_clock::now();
    aut = quotientAutomaton(aut,
                            transClosure(up_la_with_id_2nd_time, greatest_state+1),
                            greatest_state+1);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesQuot[QUOT_WITH_UP_LA_WITH_ID_2ND] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("the quotienting took "
              + std::to_string(timesQuot[QUOT_WITH_UP_LA_WITH_ID_2ND]) + " seconds. ",
               log_humanread_filename);

    /* Computing the dw-1 sim. */
    if (log_humanread_filename != "") outputText("\nComputing the dw-1 simulation... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > dw_1 (greatest_state+1, column);
    dw_ord_simulation_lvata(aut, dw_1, greatest_state+1);
    refin = 0;
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_1] = getSizeOfRel(dw_1, greatest_state+1);
    numbRefs[DW_1] = refin;
    if (log_humanread_filename != "") outputText("The dw-1 simulation has " + std::to_string(sizesRel[DW_1])
               + " and took " + std::to_string(timesRel[DW_1]) + " seconds to compute. ",
               log_humanread_filename);

    /* Computing the up-la(dw-1) sim */
    if (log_humanread_filename != "") outputText("\nComputing the up-" + std::to_string(la_up) + "(dw-1) simulation... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_dw_1 (greatest_state+1,column);
    refin = up_simulation(aut, la_up, dw_1, false, up_la_with_dw_1,
                          greatest_state+1, greatest_symbol+1, ranks);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_DW_1] = getSizeOfRel(up_la_with_dw_1, greatest_state+1);
    numbRefs[UP_LA_WITH_DW_1] = refin;
    timesRel[UP_LA_WITH_DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("The up-" + std::to_string(la_up) + "(dw-1) simulation has "
              + std::to_string(sizesRel[UP_LA_WITH_DW_1])
              + " pairs and the computation of the relation took "
              + std::to_string(numbRefs[UP_LA_WITH_DW_1]) + " matrix-refinements and "
              + std::to_string(timesRel[UP_LA_WITH_DW_1]) + " seconds to compute, ",
               log_humanread_filename);

    /* Pruning with P(up-la(dw-1) sim, strict dw-1 sim) */
    start = std::chrono::high_resolution_clock::now();
    aut = prune(aut, up_la_with_dw_1, dw_1, true);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesPrun[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("the pruning took " + std::to_string(timesPrun[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1])
               + " seconds. \n",
               log_humanread_filename);

    aut = removeUselessStates(aut);

    if (testData.numb_relations > 0)
    {
        testData.inc();
        testData.updateAvgSizesRels(sizesRel);
        testData.updateAvgNumbsRefs(numbRefs);
        testData.updateAvgTimes(timesRel, timesQuot, timesPrun);
    }

    std::cout << std::flush;

    return aut;
}


/* Warning: the computed relation below in general does not preserve the language. */
static void quotient_with_up_la_dw_la(Automaton& aut_i,
                                      unsigned int la_dw, unsigned int la_up,
                                      unsigned int numb_states = 0,
                                      unsigned int numb_symbols = 0,
                                      vector<typerank> ranks = {},
                                      bool use_lvata = false)
/* Optional parameters: either use all of them or none. */
{

    numb_states  = !numb_states  ? getNumbUsedStates(aut_i)     : numb_states;
    numb_symbols = !numb_symbols ? getNumbSymbols(aut_i)        : numb_symbols;
    ranks        = ranks.empty() ? getRanks(aut_i,numb_symbols) : ranks;

    auto start   = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type.

    vector<unsigned int> sizesRel(numbRelations);
    vector<float>        numbRefs(numbRelations);
    vector<float>        timesRel(numbRelations);
    vector<float>        timesQuot(numbQuotientings);
    vector<float>        timesPrun(numbPrunings);


    /* Common declarations. */
    float refin;
    vector<bool> column(numb_states);
    Automaton aut;

    //printAut(aut_i, &stateDict);

    /* Computing the dw-la sim. */
    std::cout << "Computing the dw-" << la_dw << " simulation... \n";
    vector<vector<bool> > dw_la(numb_states,column);
    start = std::chrono::high_resolution_clock::now();
    if (la_dw==1 && use_lvata)
    {
        dw_ord_simulation_lvata(aut_i, dw_la, numb_states);
        refin = 0;
    }
    else
    {
        refin = dw_simulation(aut_i, la_dw, dw_la, numb_states, ranks);
    }
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_LA_1ST_TIME]
            = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_LA_1ST_TIME] = getSizeOfRel(dw_la, numb_states);
    numbRefs[DW_LA_1ST_TIME] = refin;
    std::cout << "The dw-" << la_dw << " simulation has " << sizesRel[DW_LA_1ST_TIME] << " pairs and ";
    std::cout << "took " << numbRefs[DW_LA_1ST_TIME] << " matrix-refinements and "
              << timesRel[DW_LA_1ST_TIME] << " seconds to compute. ";

    /* Computing the up-la(dw-la sim) sim. */
    std::cout << "\nComputing the up-" << la_up << "(dw-" << la_dw << ") simulation... " << std::endl;
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_dw_la_1st_time(numb_states, column);
    refin = up_simulation(aut_i, la_up, dw_la, false, up_la_with_dw_la_1st_time, numb_states, numb_symbols, ranks);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_ID_1ST_TIME] = getSizeOfRel(up_la_with_dw_la_1st_time, numb_states);
    numbRefs[UP_LA_WITH_ID_1ST_TIME] = refin;
    timesRel[UP_LA_WITH_ID_1ST_TIME] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    std::cout << "The up-" << la_up << "(" << la_dw << ") simulation has "
              << sizesRel[UP_LA_WITH_ID_1ST_TIME] << " pairs ";
    std::cout << "and it took "
              << numbRefs[UP_LA_WITH_ID_1ST_TIME] << " matrix-refinements and "
              << timesRel[UP_LA_WITH_ID_1ST_TIME] << " seconds to compute and ";
    std::cout << "The up-" << la_up << "(" << la_dw << ") simulation is "
              /*<< w2String(up_la_with_dw_la_1st_time, numb_states, stateDict)*/;


    /* Quotienting with the up-la(dw-la sim) sim. */
    start = std::chrono::high_resolution_clock::now();
    aut = quotientAutomaton(aut_i, transClosure(up_la_with_dw_la_1st_time, numb_states), numb_states);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesQuot[QUOT_WITH_UP_LA_WITH_ID_1ST] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    std::cout << "the quotienting took " << timesQuot[QUOT_WITH_UP_LA_WITH_ID_1ST]
              << " seconds. ";

    aut_i = aut;
}

/* Warning: the computed relation below in general does not preserve the language. */
static void prune_with_up_la_dw_la_and_strict_dw_la(Automaton& aut_i, stateDict stateDict, unsigned int la_dw, unsigned int la_up,
                                      unsigned int numb_states = 0, unsigned int numb_symbols = 0,
                                      vector<typerank> ranks = {},
                                      bool use_lvata = false)
/* Optional parameters: either use all of them or none. */
{

    numb_states  = !numb_states  ? getNumbUsedStates(aut_i)     : numb_states;
    numb_symbols = !numb_symbols ? getNumbSymbols(aut_i)        : numb_symbols;
    ranks        = ranks.empty() ? getRanks(aut_i,numb_symbols) : ranks;

    auto start   = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type.

    vector<unsigned int> sizesRel(numbRelations);
    vector<float>        numbRefs(numbRelations);
    vector<float>        timesRel(numbRelations);
    vector<float>        timesQuot(numbQuotientings);
    vector<float>        timesPrun(numbPrunings);


    /* Common declarations. */
    float refin;
    vector<bool> column(numb_states);
    Automaton aut;

    //printAut(aut_i, &stateDict);

    /* Computing the dw-la sim. */
    std::cout << "Computing the dw-" << la_dw << " simulation... \n";
    vector<vector<bool> > dw_la(numb_states,column);
    start = std::chrono::high_resolution_clock::now();
    if (la_dw==1 && use_lvata)
    {
        dw_ord_simulation_lvata(aut_i, dw_la, numb_states);
        refin = 0;
    }
    else
    {
        refin = dw_simulation(aut_i, la_dw, dw_la, numb_states, ranks);
    }
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_LA_1ST_TIME]
            = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_LA_1ST_TIME] = getSizeOfRel(dw_la, numb_states);
    numbRefs[DW_LA_1ST_TIME] = refin;
    std::cout << "The dw-" << la_dw << " simulation has " << sizesRel[DW_LA_1ST_TIME] << " pairs and ";
    std::cout << "took " << numbRefs[DW_LA_1ST_TIME] << " matrix-refinements and "
              << timesRel[DW_LA_1ST_TIME] << " seconds to compute. ";

    /* Computing the up-la(dw-la sim) sim. */
    std::cout << "\nComputing the up-" << la_up << "(dw-" << la_dw << ") simulation... " << std::endl;
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_dw_la(numb_states, column);
    refin = up_simulation(aut_i, la_up, dw_la, false, up_la_with_dw_la, numb_states, numb_symbols, ranks);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_ID_1ST_TIME] = getSizeOfRel(up_la_with_dw_la, numb_states);
    numbRefs[UP_LA_WITH_ID_1ST_TIME] = refin;
    timesRel[UP_LA_WITH_ID_1ST_TIME] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    std::cout << "The up-" << la_up << "(" << la_dw << ") simulation has "
              << sizesRel[UP_LA_WITH_ID_1ST_TIME] << " pairs ";
    std::cout << "and it took "
              << numbRefs[UP_LA_WITH_ID_1ST_TIME] << " matrix-refinements and "
              << timesRel[UP_LA_WITH_ID_1ST_TIME] << " seconds to compute and ";
    std::cout << "The up-" << la_up << "(" << la_dw << ") simulation is "
              << w2String(aut_i, up_la_with_dw_la, numb_states, &stateDict);

    /* Prune with P(up-la(dw-la sim) sim, strict dw-la sim). */
    start = std::chrono::high_resolution_clock::now();
    aut = prune(aut, up_la_with_dw_la, dw_la, true);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesPrun[PRUN_WITH_P_ID_AND_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    std::cout << "and the pruning took " << timesPrun[PRUN_WITH_P_ID_AND_DW_LA] << " seconds. ";

    aut_i = aut;
}

Automaton applyHeavy(Automaton& aut_i, stateDict stateDict, unsigned int la_dw,
                     unsigned int la_up, /*unsigned int timeout = 0,*/
                     TestData& testData,
                     string log_humanread_filename)
{
    symbol greatest_symbol  =  getGreatestUsedSymbol(aut_i);
    vector<typerank> ranks  =  getRanks(aut_i, greatest_symbol+1);
    Automaton aut_f;
    //long int timeout = 6*60;
    timespec timeout_start;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timeout_start);

    while(true)
    {
        if (la_dw>1 || la_up>1)
        {
            while(true)
            {

                aut_f = applyMinimizationSequence(aut_i, 1, 1,
                                                  getGreatestUsedState(aut_i),
                                                  greatest_symbol, ranks, true,
                                                  &timeout_start, emptyTestData,
                                                  log_humanread_filename/*,
                                                  timeout*/);
                if (equalSize(aut_i, aut_f)) break;
                else aut_i = aut_f;
            }
        }
        aut_f = applyMinimizationSequence(aut_i, la_dw, la_up,
                                          getGreatestUsedState(aut_i),
                                          greatest_symbol, ranks, true,
                                          &timeout_start, testData,
                                          log_humanread_filename/*, timeout*/);
        if (equalSize(aut_i, aut_f)) break;
        else aut_i = aut_f;
    }

    return aut_f;
}

void applyProcedure1(const Automaton& aut_i, stateDict sDict,
                    unsigned int la_dw, unsigned int la_up, MetaData& metaData,
                    TestData& testData_ru,    Timeout& timeout_ru,
                    TestData& testData_quot,  Timeout& timeout_quot,
                    TestData& testData_qAndP, Timeout& timeout_qAndP,
                    TestData& testData_heavy, TestData& testData_heavy_rels,
                    Timeout& timeout_heavy,
                    string log_humanread_filename, string log_machread_filename,
                    string log_machread_heavy_1_1_times_filename)
{    
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

    state greatest_state    =  getGreatestUsedState(aut_i);
    symbol greatest_symbol  =  getGreatestUsedSymbol(aut_i);
    vector<typerank> ranks  =  getRanks(aut_i, greatest_symbol+1);

    /* Applying Remove Useless States */
    string headline = "\n1) Removing Useless States... \n";
    outputText(headline, log_humanread_filename);
    //std::cout << "\n" << headline << std::endl;

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    try
    {
        Automaton aut_ru = removeUselessStates(aut_i);

        if ( /*equiv(aut_ru, aut_i)*/ true )
        {
            float q_red          =  measureStatesReduction(aut_ru, aut_i);
            float delta_red      =  measureTransitionsReduction(aut_ru, aut_i);
            float transDens_red  =  measureTransDensReduction(aut_ru, aut_i);
            testData_ru.inc();
            testData_ru.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_ru.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_ru.updateReductionBuckets(q_red, delta_red, transDens_red);

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

        }
        else
        {
            printAut(aut_i, &sDict);
            printAut(aut_ru, &sDict);
            exit_with_error("The Remove Useless method did NOT preserve the language!\n");
        }
    }
    catch (timeout_& e)
    {
        outputText("TIMEOUT: The Remove Useless method took longer than "
                  + std::to_string( (float) TIMEOUT/60 ) + " minutes and therefore was aborted.\n",
                   log_humanread_filename);
        timeout_ru.inc();
        timeout_ru.checkSmallest(getNumbUsedStates(aut_i),
                                  getNumbTransitions(aut_i),
                                  getTransitionDensity(aut_i));

        testData_ru.inc();
        testData_ru.updateAvgReductions(100, 100, 100);
        testData_ru.checkGreatestReductions(100, 100, 100);
        testData_ru.updateReductionBuckets(100, 100, 100);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, "TO \t TO \t TO \n");
    }


    /* Applying Quotienting with dw-la sim. */
    headline = "2) Quotienting with dw-" + std::to_string(la_dw) + " sim... \n";
    outputText("\n" + headline, log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    bool didQuotTimeout = false;
    Automaton aut_quot = copyAut(aut_i, true);
    try
    {
        aut_quot = applyQuotDwLa(aut_quot, la_dw,
                                 greatest_state, greatest_symbol,
                                 ranks, true, log_humanread_filename);

        if ( /*equiv(aut_quot, aut_i)*/ true )
        {
            float q_red          =  measureStatesReduction(aut_quot, aut_i);
            float delta_red      =  measureTransitionsReduction(aut_quot, aut_i);
            float transDens_red  =  measureTransDensReduction(aut_quot, aut_i);
            testData_quot.inc();
            testData_quot.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_quot.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_quot.updateReductionBuckets(q_red, delta_red, transDens_red);

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

        didQuotTimeout = true;

        testData_quot.inc();
        testData_quot.updateAvgReductions(100, 100, 100);
        testData_quot.checkGreatestReductions(100, 100, 100);
        testData_quot.updateReductionBuckets(100, 100, 100);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, "TO \t TO \t TO \n");
    }

    /* Applying Quotienting & Pruning with dw-la sim. */
    headline = "3) Quotienting with dw-" + std::to_string(la_dw) + " sim. and Pruning with P(id, dw-" + std::to_string(la_dw) + " sim.)... ";
    outputText("\n" + headline + "\n", log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline + "\n");

    Automaton aut_qAndP = copyAut(aut_i, true);
    if (!didQuotTimeout)
    {
        try
        {
            aut_qAndP = applyQuotAndPrunDwLa(aut_qAndP, la_dw,
                                             greatest_state,
                                             greatest_symbol,
                                             ranks, true, log_humanread_filename);

            if ( /*equiv(aut_qAndP, aut_i)*/ true )
            {
                float q_red          =  measureStatesReduction(aut_qAndP, aut_i);
                float delta_red      =  measureTransitionsReduction(aut_qAndP, aut_i);
                float transDens_red  =  measureTransDensReduction(aut_qAndP, aut_i);
                testData_qAndP.inc();
                testData_qAndP.updateAvgReductions(q_red, delta_red, transDens_red);
                testData_qAndP.checkGreatestReductions(q_red, delta_red, transDens_red);
                testData_qAndP.updateReductionBuckets(q_red, delta_red, transDens_red);

                string q_red_str         = std::to_string(q_red);
                string delta_red_str     = std::to_string(delta_red);
                string transDens_red_str = std::to_string(transDens_red);

                outputText("  The resulting automaton has "
                          + q_red_str + "% of its states, "
                          + delta_red_str + "% of its transitions and "
                          + transDens_red_str + "% of its transition density. \n",
                          log_humanread_filename);

                if (log_machread_filename != "")
                    writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");
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

            if (log_machread_filename != "")
                writeToFile(log_machread_filename, "TO \t TO \t TO \n");
        }
    }
    else
    {
        testData_qAndP.inc();
        testData_qAndP.updateAvgReductions(100, 100, 100);
        testData_qAndP.checkGreatestReductions(100, 100, 100);
        testData_qAndP.updateReductionBuckets(100, 100, 100);
    }


    /* Applying Heavy(k,j). */
    headline = "4) Applying Heavy(" + std::to_string(la_dw) + ", " + std::to_string(la_up) + ")...";
    outputText("\n" + headline + "\n", log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    Automaton aut_heavy = copyAut(aut_i, true);
    try
    {        
        auto start = std::chrono::high_resolution_clock::now();
        aut_heavy = applyHeavy(aut_heavy, sDict, la_dw, la_up,
                               testData_heavy_rels, log_humanread_filename);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

        //if (langIsEmpty(aut_i) && langIsEmpty(aut_heavy))
          //  outputText("---. Both the initial and the minimized automata have the empty language <-----\n", log_humanread_filename);

        if ( /*equiv(aut_heavy, aut_i)*/ true )
        {
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

            outputText("  The resulting automaton has "
                      + q_red_str + "% of its states, "
                      + delta_red_str + "% of its transitions and "
                      + transDens_red_str + "% of its transition density. \n",
                       log_humanread_filename);

            if (log_machread_filename != "")
                writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");

            if (log_machread_heavy_1_1_times_filename != "")
                writeToFile(log_machread_heavy_1_1_times_filename,
                            std::to_string(getNumbUsedStates(aut_i)) + "\t" + std::to_string(getNumbTransitions(aut_i))
                            + "\t" + std::to_string(getNumbUsedStates(aut_heavy)) + "\t"
                            + std::to_string(getNumbTransitions(aut_heavy)) + "\t" + q_red_str + "\t"
                            + delta_red_str + "\t" + std::to_string(seconds) + "\n");

        }
        else
        {
            printAut(aut_i, &sDict);
            printAut(aut_heavy, &sDict);
            exit_with_error("The Heavy(k,j) method did NOT preserve the language!\n");
        }
    }
    catch (timeout_& e)
    {
        outputText("TIMEOUT: The Heavy(k,j) took longer than "
                  + std::to_string( (float) TIMEOUT/60 ) + " minutes and therefore was aborted.\n",
                   log_humanread_filename);
        timeout_heavy.inc();
        timeout_heavy.checkSmallest(getNumbUsedStates(aut_i), getNumbTransitions(aut_i), getTransitionDensity(aut_i));

        testData_heavy.inc();
        testData_heavy.updateAvgReductions(100, 100, 100);
        testData_heavy.checkGreatestReductions(100, 100, 100);
        testData_heavy.updateReductionBuckets(100, 100, 100);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, "TO \t TO \t TO \n");

        if (log_machread_heavy_1_1_times_filename != "")
            writeToFile(log_machread_heavy_1_1_times_filename,
                        std::to_string(getNumbUsedStates(aut_i)) + "\t" + std::to_string(getNumbTransitions(aut_i))
                        + "\t" + std::to_string(getNumbUsedStates(aut_heavy)) + "\t"
                        + std::to_string(getNumbTransitions(aut_heavy)) + "\t" + "TO" + "\t"
                        + "TO" + "\t" + "TO" + "\n");
    }

}

void applyProcedure2(const Automaton& aut_i, stateDict sDict, MetaData& metaData,
                    TestData& testData_ru,      Timeout& timeout_ru,
                    TestData& testData_quot,    Timeout& timeout_quot,
                    TestData& testData_qAndP,   Timeout& timeout_qAndP,
                    TestData& testData_heavy11, TestData& testData_heavy11_rels, Timeout& timeout_heavy11,
                    TestData& testData_heavy24, TestData& testData_heavy24_rels, Timeout& timeout_heavy24,
                    string log_humanread_filename, string log_machread_filename,
                    string log_machread_heavy_times_filename)
{
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

    state greatest_state    =  getGreatestUsedState(aut_i);
    symbol greatest_symbol  =  getGreatestUsedSymbol(aut_i);
    vector<typerank> ranks  =  getRanks(aut_i, greatest_symbol+1);

    /* Applying Remove Useless States */
    string headline = "\n1) Removing Useless States... ";
    outputText(headline, log_humanread_filename);
    //std::cout << "\n" << headline << std::endl;

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    try
    {
        Automaton aut_ru = removeUselessStates(aut_i);

        if ( /*equiv(aut_ru, aut_i)*/ true )
        {
            float q_red          =  measureStatesReduction(aut_ru, aut_i);
            float delta_red      =  measureTransitionsReduction(aut_ru, aut_i);
            float transDens_red  =  measureTransDensReduction(aut_ru, aut_i);
            testData_ru.inc();
            testData_ru.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_ru.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_ru.updateReductionBuckets(q_red, delta_red, transDens_red);

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

        }
        else
        {
            printAut(aut_i, &sDict);
            printAut(aut_ru, &sDict);
            exit_with_error("The Remove Useless method did NOT preserve the language!\n");
        }
    }
    catch (timeout_& e)
    {
        outputText("TIMEOUT: The Remove Useless method took longer than "
                  + std::to_string( (float) TIMEOUT/60 ) + " minutes and therefore was aborted.\n",
                   log_humanread_filename);
        timeout_ru.inc();
        timeout_ru.checkSmallest(getNumbUsedStates(aut_i),
                                  getNumbTransitions(aut_i),
                                  getTransitionDensity(aut_i));

        testData_ru.inc();
        testData_ru.updateAvgReductions(100, 100, 100);
        testData_ru.checkGreatestReductions(100, 100, 100);
        testData_ru.updateReductionBuckets(100, 100, 100);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, "TO \t TO \t TO \n");
    }


    /* Applying Quotienting with dw-1 sim. */
    headline = "2) Quotienting with dw-1 sim... ";
    outputText("\n" + headline, log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    bool didQuotTimeout = false;
    Automaton aut_quot = copyAut(aut_i, true);
    try
    {
        aut_quot = applyQuotDwLa(aut_quot, 1,
                                 greatest_state, greatest_symbol,
                                 ranks, true, log_humanread_filename);

        if ( /*equiv(aut_quot, aut_i)*/ true )
        {
            float q_red          =  measureStatesReduction(aut_quot, aut_i);
            float delta_red      =  measureTransitionsReduction(aut_quot, aut_i);
            float transDens_red  =  measureTransDensReduction(aut_quot, aut_i);
            testData_quot.inc();
            testData_quot.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_quot.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_quot.updateReductionBuckets(q_red, delta_red, transDens_red);

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

        didQuotTimeout = true;

        testData_quot.inc();
        testData_quot.updateAvgReductions(100, 100, 100);
        testData_quot.checkGreatestReductions(100, 100, 100);
        testData_quot.updateReductionBuckets(100, 100, 100);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, "TO \t TO \t TO \n");
    }

    /* Applying Quotienting & Pruning with dw-1 sim. */
    headline = "3) Quot. with dw-1 sim. + Prun. with P(id,dw-1 sim.)... ";
    outputText("\n" + headline + "\n", log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    Automaton aut_qAndP = copyAut(aut_i, true);
    if (!didQuotTimeout)
    {
        try
        {
            aut_qAndP = applyQuotAndPrunDwLa(aut_qAndP, 1,
                                             greatest_state,
                                             greatest_symbol,
                                             ranks, true, log_humanread_filename);

            if ( /*equiv(aut_qAndP, aut_i)*/ true )
            {
                float q_red          =  measureStatesReduction(aut_qAndP, aut_i);
                float delta_red      =  measureTransitionsReduction(aut_qAndP, aut_i);
                float transDens_red  =  measureTransDensReduction(aut_qAndP, aut_i);
                testData_qAndP.inc();
                testData_qAndP.updateAvgReductions(q_red, delta_red, transDens_red);
                testData_qAndP.checkGreatestReductions(q_red, delta_red, transDens_red);
                testData_qAndP.updateReductionBuckets(q_red, delta_red, transDens_red);

                string q_red_str         = std::to_string(q_red);
                string delta_red_str     = std::to_string(delta_red);
                string transDens_red_str = std::to_string(transDens_red);

                outputText("  The resulting automaton has "
                          + q_red_str + "% of its states, "
                          + delta_red_str + "% of its transitions and "
                          + transDens_red_str + "% of its transition density. \n",
                          log_humanread_filename);

                if (log_machread_filename != "")
                    writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");
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

            if (log_machread_filename != "")
                writeToFile(log_machread_filename, "TO \t TO \t TO \n");
        }
    }
    else
    {
        testData_qAndP.inc();
        testData_qAndP.updateAvgReductions(100, 100, 100);
        testData_qAndP.checkGreatestReductions(100, 100, 100);
        testData_qAndP.updateReductionBuckets(100, 100, 100);
    }


    /* Applying Heavy(1,1). */
    unsigned int la_dw = 1, la_up = 1;
    headline = "4) Applying Heavy(" + std::to_string(la_dw) + ", " + std::to_string(la_up) + ")... ";
    outputText("\n" + headline + "\n", log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    Automaton aut_heavy11 = copyAut(aut_i, true);
    try
    {
        auto start = std::chrono::high_resolution_clock::now();
        aut_heavy11 = applyHeavy(aut_heavy11, sDict, la_dw, la_up,
                               testData_heavy11_rels, log_humanread_filename);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

        //if (langIsEmpty(aut_i) && langIsEmpty(aut_heavy))
          //  outputText("---. Both the initial and the minimized automata have the empty language <-----\n", log_humanread_filename);

        if ( /*equiv(aut_heavy11, aut_i)*/ true )
        {
            float q_red          =  measureStatesReduction(aut_heavy11, aut_i);
            float delta_red      =  measureTransitionsReduction(aut_heavy11, aut_i);
            float transDens_red  =  measureTransDensReduction(aut_heavy11, aut_i);
            testData_heavy11.inc();
            testData_heavy11.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_heavy11.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_heavy11.updateReductionBuckets(q_red, delta_red, transDens_red);

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
                            std::to_string(getNumbUsedStates(aut_i)) + "\t" + std::to_string(getNumbTransitions(aut_i))
                            + "\t" + std::to_string(getNumbUsedStates(aut_heavy11)) + "\t"
                            + std::to_string(getNumbTransitions(aut_heavy11)) + "\t" + q_red_str + "\t"
                            + delta_red_str + "\t" + std::to_string(seconds) + "\t");

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

        testData_heavy11.inc();
        testData_heavy11.updateAvgReductions(100, 100, 100);
        testData_heavy11.checkGreatestReductions(100, 100, 100);
        testData_heavy11.updateReductionBuckets(100, 100, 100);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, "TO \t TO \t TO \n");

        if (log_machread_heavy_times_filename != "")
            writeToFile(log_machread_heavy_times_filename,
                        std::to_string(getNumbUsedStates(aut_heavy11)) + "\t"
                        + std::to_string(getNumbTransitions(aut_heavy11)) + "\t" + "TO" + "\t"
                        + "TO" + "\t" + "TO"  + "\t");

    }

    /* Applying Heavy(2,4). */
    la_dw = 2;
    la_up = 4;
    headline = "5) Applying Heavy(" + std::to_string(la_dw) + ", " + std::to_string(la_up) + ")... ";
    outputText("\n" + headline + "\n", log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    Automaton aut_heavy24 = copyAut(aut_i, true);
    try
    {
        auto start = std::chrono::high_resolution_clock::now();
        aut_heavy24 = applyHeavy(aut_heavy24, sDict, la_dw, la_up,
                                 testData_heavy24_rels, log_humanread_filename);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

        //if (langIsEmpty(aut_i) && langIsEmpty(aut_heavy))
          //  outputText("---. Both the initial and the minimized automata have the empty language <-----\n", log_humanread_filename);

        if ( /*equiv(aut_heavy24, aut_i)*/ true )
        {
            float q_red          =  measureStatesReduction(aut_heavy24, aut_i);
            float delta_red      =  measureTransitionsReduction(aut_heavy24, aut_i);
            float transDens_red  =  measureTransDensReduction(aut_heavy24, aut_i);
            testData_heavy24.inc();
            testData_heavy24.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_heavy24.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_heavy24.updateReductionBuckets(q_red, delta_red, transDens_red);

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
                            std::to_string(getNumbUsedStates(aut_heavy24)) + "\t"
                            + std::to_string(getNumbTransitions(aut_heavy24)) + "\t" + q_red_str + "\t"
                            + delta_red_str + "\t" + std::to_string(seconds) + "\n");

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

        testData_heavy24.inc();
        testData_heavy24.updateAvgReductions(100, 100, 100);
        testData_heavy24.checkGreatestReductions(100, 100, 100);
        testData_heavy24.updateReductionBuckets(100, 100, 100);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, "TO \t TO \t TO \n");

        if (log_machread_heavy_times_filename != "")
            writeToFile(log_machread_heavy_times_filename,
                        std::to_string(getNumbUsedStates(aut_heavy24)) + "\t"
                        + std::to_string(getNumbTransitions(aut_heavy24)) + "\t" + "TO" + "\t"
                        + "TO" + "\t" + "TO" + "\n");
    }

}

void applyProcedure3(const Automaton& aut_i, stateDict sDict, MetaData& metaData,
                    TestData& testData_ru,      Timeout& timeout_ru,
                    TestData& testData_quot,    Timeout& timeout_quot,
                    TestData& testData_qAndP,   Timeout& timeout_qAndP,
                    TestData& testData_heavy11, TestData& testData_heavy11_rels, Timeout& timeout_heavy11,
                    TestData& testData_heavy24, TestData& testData_heavy24_rels, Timeout& timeout_heavy24,
                    TestData& testData_heavy36, TestData& testData_heavy36_rels, Timeout& timeout_heavy36,
                    string log_humanread_filename, string log_machread_filename,
                    string log_machread_heavy_times_filename)
{
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

    state greatest_state    =  getGreatestUsedState(aut_i);
    symbol greatest_symbol  =  getGreatestUsedSymbol(aut_i);
    vector<typerank> ranks  =  getRanks(aut_i, greatest_symbol+1);

    /* Applying Remove Useless States */
    string headline = "\n1) Removing Useless States... ";
    outputText(headline, log_humanread_filename);
    //std::cout << "\n" << headline << std::endl;

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

//    try
//    {
//        Automaton aut_ru = removeUselessStates(aut_i);

//        if ( /*equiv(aut_ru, aut_i)*/ true )
//        {
//            float q_red          =  measureStatesReduction(aut_ru, aut_i);
//            float delta_red      =  measureTransitionsReduction(aut_ru, aut_i);
//            float transDens_red  =  measureTransDensReduction(aut_ru, aut_i);
//            testData_ru.inc();
//            testData_ru.updateAvgReductions(q_red, delta_red, transDens_red);
//            testData_ru.checkGreatestReductions(q_red, delta_red, transDens_red);
//            testData_ru.updateReductionBuckets(q_red, delta_red, transDens_red);

//            string q_red_str = std::to_string(q_red);
//            string delta_red_str = std::to_string(delta_red);
//            string transDens_red_str = std::to_string(transDens_red);
//            outputText("  The resulting automaton has "
//                       + q_red_str + "% of its states, "
//                       + delta_red_str + "% of its transitions and "
//                       + transDens_red_str + "% of its transition density. \n",
//                       log_humanread_filename);

//            if (log_machread_filename != "")
//                writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");

//        }
//        else
//        {
//            printAut(aut_i, &sDict);
//            printAut(aut_ru, &sDict);
//            exit_with_error("The Remove Useless method did NOT preserve the language!\n");
//        }
//    }
//    catch (timeout_& e)
//    {
//        outputText("TIMEOUT: The Remove Useless method took longer than "
//                  + std::to_string( (float) TIMEOUT/60 ) + " minutes and therefore was aborted.\n",
//                   log_humanread_filename);
//        timeout_ru.inc();
//        timeout_ru.checkSmallest(getNumbUsedStates(aut_i),
//                                  getNumbTransitions(aut_i),
//                                  getTransitionDensity(aut_i));

//        testData_ru.inc();
//        testData_ru.updateAvgReductions(100, 100, 100);
//        testData_ru.checkGreatestReductions(100, 100, 100);
//        testData_ru.updateReductionBuckets(100, 100, 100);

//        if (log_machread_filename != "")
//            writeToFile(log_machread_filename, "TO \t TO \t TO \n");
//    }


//    /* Applying Quotienting with dw-1 sim. */
//    headline = "2) Quotienting with dw-1 sim... ";
//    outputText("\n" + headline, log_humanread_filename);

//    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

//    bool didQuotTimeout = false;
//    Automaton aut_quot = copyAut(aut_i, true);
//    try
//    {
//        aut_quot = applyQuotDwLa(aut_quot, 1,
//                                 greatest_state, greatest_symbol,
//                                 ranks, true, log_humanread_filename);

//        if ( /*equiv(aut_quot, aut_i)*/ true )
//        {
//            float q_red          =  measureStatesReduction(aut_quot, aut_i);
//            float delta_red      =  measureTransitionsReduction(aut_quot, aut_i);
//            float transDens_red  =  measureTransDensReduction(aut_quot, aut_i);
//            testData_quot.inc();
//            testData_quot.updateAvgReductions(q_red, delta_red, transDens_red);
//            testData_quot.checkGreatestReductions(q_red, delta_red, transDens_red);
//            testData_quot.updateReductionBuckets(q_red, delta_red, transDens_red);

//            string q_red_str = std::to_string(q_red);
//            string delta_red_str = std::to_string(delta_red);
//            string transDens_red_str = std::to_string(transDens_red);
//            outputText("  The resulting automaton has "
//                      + q_red_str         + "% of its states, "
//                      + delta_red_str     + "% of its transitions and "
//                      + transDens_red_str + "% of its transition density. \n",
//                       log_humanread_filename);

//            if (log_machread_filename != "")
//                writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");

//        }
//        else
//        {
//            printAut(aut_i, &sDict);
//            printAut(aut_quot, &sDict);
//            exit_with_error("The Quotienting method did NOT preserve the language!\n");
//        }
//    }
//    catch (timeout_& e)
//    {
//        outputText("TIMEOUT: The Quotienting method took longer than "
//                  + std::to_string( (float) TIMEOUT/60 ) + " minutes and therefore was aborted.\n",
//                   log_humanread_filename);
//        timeout_quot.inc();
//        timeout_quot.checkSmallest(getNumbUsedStates(aut_i),
//                                    getNumbTransitions(aut_i),
//                                    getTransitionDensity(aut_i));

//        didQuotTimeout = true;

//        testData_quot.inc();
//        testData_quot.updateAvgReductions(100, 100, 100);
//        testData_quot.checkGreatestReductions(100, 100, 100);
//        testData_quot.updateReductionBuckets(100, 100, 100);

//        if (log_machread_filename != "")
//            writeToFile(log_machread_filename, "TO \t TO \t TO \n");
//    }

//    /* Applying Quotienting & Pruning with dw-1 sim. */
//    headline = "3) Quot. with dw-1 sim. + Prun. with P(id,dw-1 sim.)... ";
//    outputText("\n" + headline + "\n", log_humanread_filename);

//    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

//    Automaton aut_qAndP = copyAut(aut_i, true);
//    if (!didQuotTimeout)
//    {
//        try
//        {
//            aut_qAndP = applyQuotAndPrunDwLa(aut_qAndP, 1,
//                                             greatest_state,
//                                             greatest_symbol,
//                                             ranks, true, log_humanread_filename);

//            if ( /*equiv(aut_qAndP, aut_i)*/ true )
//            {
//                float q_red          =  measureStatesReduction(aut_qAndP, aut_i);
//                float delta_red      =  measureTransitionsReduction(aut_qAndP, aut_i);
//                float transDens_red  =  measureTransDensReduction(aut_qAndP, aut_i);
//                testData_qAndP.inc();
//                testData_qAndP.updateAvgReductions(q_red, delta_red, transDens_red);
//                testData_qAndP.checkGreatestReductions(q_red, delta_red, transDens_red);
//                testData_qAndP.updateReductionBuckets(q_red, delta_red, transDens_red);

//                string q_red_str         = std::to_string(q_red);
//                string delta_red_str     = std::to_string(delta_red);
//                string transDens_red_str = std::to_string(transDens_red);

//                outputText("  The resulting automaton has "
//                          + q_red_str + "% of its states, "
//                          + delta_red_str + "% of its transitions and "
//                          + transDens_red_str + "% of its transition density. \n",
//                          log_humanread_filename);

//                if (log_machread_filename != "")
//                    writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");
//            }
//            else
//            {
//                printAut(aut_i, &sDict);
//                printAut(aut_qAndP, &sDict);
//                exit_with_error("The Quotienting&Pruning method did NOT preserve the language!\n");
//            }
//        }
//        catch (timeout_& e)
//        {
//            std::cout << "TIMEOUT: The Quotienting&Pruning method took longer than "
//                      << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
//            timeout_qAndP.inc();
//            timeout_qAndP.checkSmallest(getNumbUsedStates(aut_i),
//                                        getNumbTransitions(aut_i),
//                                        getTransitionDensity(aut_i));

//            testData_qAndP.inc();
//            testData_qAndP.updateAvgReductions(100, 100, 100);
//            testData_qAndP.checkGreatestReductions(100, 100, 100);
//            testData_qAndP.updateReductionBuckets(100, 100, 100);

//            if (log_machread_filename != "")
//                writeToFile(log_machread_filename, "TO \t TO \t TO \n");
//        }
//    }
//    else
//    {
//        testData_qAndP.inc();
//        testData_qAndP.updateAvgReductions(100, 100, 100);
//        testData_qAndP.checkGreatestReductions(100, 100, 100);
//        testData_qAndP.updateReductionBuckets(100, 100, 100);
//    }


//    /* Applying Heavy(1,1). */
//    unsigned int la_dw = 1, la_up = 1;
//    headline = "4) Applying Heavy(" + std::to_string(la_dw) + ", " + std::to_string(la_up) + ")... ";
//    outputText("\n" + headline + "\n", log_humanread_filename);

//    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

//    Automaton aut_heavy11 = copyAut(aut_i, true);
//    try
//    {
//        auto start = std::chrono::high_resolution_clock::now();
//        aut_heavy11 = applyHeavy(aut_heavy11, sDict, la_dw, la_up,
//                               testData_heavy11_rels, log_humanread_filename);
//        auto elapsed = std::chrono::high_resolution_clock::now() - start;
//        float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

//        //if (langIsEmpty(aut_i) && langIsEmpty(aut_heavy))
//          //  outputText("---. Both the initial and the minimized automata have the empty language <-----\n", log_humanread_filename);

//        if ( /*equiv(aut_heavy, aut_i)*/ true )
//        {
//            float q_red          =  measureStatesReduction(aut_heavy11, aut_i);
//            float delta_red      =  measureTransitionsReduction(aut_heavy11, aut_i);
//            float transDens_red  =  measureTransDensReduction(aut_heavy11, aut_i);
//            testData_heavy11.inc();
//            testData_heavy11.updateAvgReductions(q_red, delta_red, transDens_red);
//            testData_heavy11.checkGreatestReductions(q_red, delta_red, transDens_red);
//            testData_heavy11.updateReductionBuckets(q_red, delta_red, transDens_red);

//            string q_red_str          =  std::to_string(q_red);
//            string delta_red_str      =  std::to_string(delta_red);
//            string transDens_red_str  =  std::to_string(transDens_red);

//            outputText("  The resulting automaton has "
//                      + q_red_str + "% of its states, "
//                      + delta_red_str + "% of its transitions and "
//                      + transDens_red_str + "% of its transition density. \n",
//                       log_humanread_filename);

//            if (log_machread_filename != "")
//                writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");

//            if (log_machread_heavy_times_filename != "")
//                writeToFile(log_machread_heavy_times_filename,
//                            std::to_string(getNumbUsedStates(aut_i)) + "\t" + std::to_string(getNumbTransitions(aut_i))
//                            + "\t" + std::to_string(getNumbUsedStates(aut_heavy11)) + "\t"
//                            + std::to_string(getNumbTransitions(aut_heavy11)) + "\t" + q_red_str + "\t"
//                            + delta_red_str + "\t" + std::to_string(seconds) + " ");

//        }
//        else
//        {
//            printAut(aut_i, &sDict);
//            printAut(aut_heavy11, &sDict);
//            exit_with_error("The Heavy(1,1) method did NOT preserve the language!\n");
//        }
//    }
//    catch (timeout_& e)
//    {
//        outputText("TIMEOUT: The Heavy(1,1) took longer than "
//                  + std::to_string( (float) TIMEOUT/60 ) + " minutes and therefore was aborted.\n",
//                   log_humanread_filename);
//        timeout_heavy11.inc();
//        timeout_heavy11.checkSmallest(getNumbUsedStates(aut_i), getNumbTransitions(aut_i), getTransitionDensity(aut_i));

//        testData_heavy11.inc();
//        testData_heavy11.updateAvgReductions(100, 100, 100);
//        testData_heavy11.checkGreatestReductions(100, 100, 100);
//        testData_heavy11.updateReductionBuckets(100, 100, 100);

//        if (log_machread_filename != "")
//            writeToFile(log_machread_filename, "TO \t TO \t TO \n");

//        if (log_machread_heavy_times_filename != "")
//            writeToFile(log_machread_heavy_times_filename,
//                        std::to_string(getNumbUsedStates(aut_heavy11)) + "\t"
//                        + std::to_string(getNumbTransitions(aut_heavy11)) + "\t" + "TO" + "\t"
//                        + "TO" + "\t" + "TO"  + " ");

//    }

//    /* Applying Heavy(2,4). */
//    la_dw = 2;
//    la_up = 4;
//    headline = "5) Applying Heavy(" + std::to_string(la_dw) + ", " + std::to_string(la_up) + ")... ";
//    outputText("\n" + headline + "\n", log_humanread_filename);

//    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

//    Automaton aut_heavy24 = copyAut(aut_i, true);
//    try
//    {
//        auto start = std::chrono::high_resolution_clock::now();
//        aut_heavy24 = applyHeavy(aut_heavy24, sDict, la_dw, la_up,
//                                 testData_heavy24_rels, log_humanread_filename);
//        auto elapsed = std::chrono::high_resolution_clock::now() - start;
//        float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

//        //if (langIsEmpty(aut_i) && langIsEmpty(aut_heavy))
//          //  outputText("---. Both the initial and the minimized automata have the empty language <-----\n", log_humanread_filename);

//        if ( /*equiv(aut_heavy, aut_i)*/ true )
//        {
//            float q_red          =  measureStatesReduction(aut_heavy24, aut_i);
//            float delta_red      =  measureTransitionsReduction(aut_heavy24, aut_i);
//            float transDens_red  =  measureTransDensReduction(aut_heavy24, aut_i);
//            testData_heavy24.inc();
//            testData_heavy24.updateAvgReductions(q_red, delta_red, transDens_red);
//            testData_heavy24.checkGreatestReductions(q_red, delta_red, transDens_red);
//            testData_heavy24.updateReductionBuckets(q_red, delta_red, transDens_red);

//            string q_red_str          =  std::to_string(q_red);
//            string delta_red_str      =  std::to_string(delta_red);
//            string transDens_red_str  =  std::to_string(transDens_red);

//            outputText("  The resulting automaton has "
//                      + q_red_str + "% of its states, "
//                      + delta_red_str + "% of its transitions and "
//                      + transDens_red_str + "% of its transition density. \n",
//                       log_humanread_filename);

//            if (log_machread_filename != "")
//                writeToFile(log_machread_filename, q_red_str + "\t" + delta_red_str
//                            + "\t" + transDens_red_str + "\n");

//            if (log_machread_heavy_times_filename != "")
//                writeToFile(log_machread_heavy_times_filename,
//                            std::to_string(getNumbUsedStates(aut_heavy24)) + "\t"
//                            + std::to_string(getNumbTransitions(aut_heavy24)) + "\t" + q_red_str + "\t"
//                            + delta_red_str + "\t" + std::to_string(seconds) + " ");

//        }
//        else
//        {
//            printAut(aut_i, &sDict);
//            printAut(aut_heavy24, &sDict);
//            exit_with_error("The Heavy(2,4) method did NOT preserve the language!\n");
//        }
//    }
//    catch (timeout_& e)
//    {
//        outputText("TIMEOUT: The Heavy(2,4) took longer than "
//                  + std::to_string( (float) TIMEOUT/60 ) + " minutes and therefore was aborted.\n",
//                   log_humanread_filename);
//        timeout_heavy24.inc();
//        timeout_heavy24.checkSmallest(getNumbUsedStates(aut_i), getNumbTransitions(aut_i), getTransitionDensity(aut_i));

//        testData_heavy24.inc();
//        testData_heavy24.updateAvgReductions(100, 100, 100);
//        testData_heavy24.checkGreatestReductions(100, 100, 100);
//        testData_heavy24.updateReductionBuckets(100, 100, 100);

//        if (log_machread_filename != "")
//            writeToFile(log_machread_filename, "TO \t TO \t TO \n");

//        if (log_machread_heavy_times_filename != "")
//            writeToFile(log_machread_heavy_times_filename,
//                        std::to_string(getNumbUsedStates(aut_heavy24)) + "\t"
//                        + std::to_string(getNumbTransitions(aut_heavy24)) + "\t" + "TO" + "\t"
//                        + "TO" + "\t" + "TO" + " ");

//    }

    /* Applying Heavy(3,6). */
    unsigned int la_dw = 3;
    unsigned int la_up = 6;
    headline = "6) Applying Heavy(" + std::to_string(la_dw) + ", " + std::to_string(la_up) + ")... ";
    outputText("\n" + headline + "\n", log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    Automaton aut_heavy36 = copyAut(aut_i, true);
    try
    {
        auto start = std::chrono::high_resolution_clock::now();
        aut_heavy36 = applyHeavy(aut_heavy36, sDict, la_dw, la_up,
                               testData_heavy36_rels, log_humanread_filename);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

        //if (langIsEmpty(aut_i) && langIsEmpty(aut_heavy))
          //  outputText("---. Both the initial and the minimized automata have the empty language <-----\n", log_humanread_filename);

        if ( /*equiv(aut_heavy, aut_i)*/ true )
        {
            float q_red          =  measureStatesReduction(aut_heavy36, aut_i);
            float delta_red      =  measureTransitionsReduction(aut_heavy36, aut_i);
            float transDens_red  =  measureTransDensReduction(aut_heavy36, aut_i);
            testData_heavy36.inc();
            testData_heavy36.updateAvgReductions(q_red, delta_red, transDens_red);
            testData_heavy36.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData_heavy36.updateReductionBuckets(q_red, delta_red, transDens_red);

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
                            std::to_string(getNumbUsedStates(aut_heavy36)) + "\t"
                            + std::to_string(getNumbTransitions(aut_heavy36)) + "\t" + q_red_str + "\t"
                            + delta_red_str + "\t" + std::to_string(seconds) + "\n");

        }
        else
        {
            printAut(aut_i, &sDict);
            printAut(aut_heavy36, &sDict);
            exit_with_error("The Heavy(3,6) method did NOT preserve the language!\n");
        }
    }
    catch (timeout_& e)
    {
        outputText("TIMEOUT: The Heavy(3,6) took longer than "
                  + std::to_string( (float) TIMEOUT/60 ) + " minutes and therefore was aborted.\n",
                   log_humanread_filename);
        timeout_heavy36.inc();
        timeout_heavy36.checkSmallest(getNumbUsedStates(aut_i), getNumbTransitions(aut_i), getTransitionDensity(aut_i));

        testData_heavy36.inc();
        testData_heavy36.updateAvgReductions(100, 100, 100);
        testData_heavy36.checkGreatestReductions(100, 100, 100);
        testData_heavy36.updateReductionBuckets(100, 100, 100);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename, "TO \t TO \t TO \n");

        if (log_machread_heavy_times_filename != "")
            writeToFile(log_machread_heavy_times_filename,
                        std::to_string(getNumbUsedStates(aut_heavy36)) + "\t"
                        + std::to_string(getNumbTransitions(aut_heavy36)) + "\t" + "TO" + "\t"
                        + "TO" + "\t" + "TO" + "\n");

    }

}



///* A test function that focuses on checking if the lookahead dw and up simulation algorithms presented
//   in this project are consistent (ie, using them on the appropriate minimization techniques preserves
//   the language). This is done by, for each automaton A, applying the following:
//    1) computing the (maximal) dw-la simulation D and quotienting A with D;
//    2) pruning the automaton with P(id, strict D);
//    3) computing the (maximal) up-la(id) simulation U and quotienting A with U;
//    4) computing the (maximal) dw-la simulation D again;
//    4') computing a strict (maximal) ordinary up(id) simulation U' and pruning A with P(U',D);
//    5) repeting step 1;
//    6) computing the ordinary dw simulation D' and the up-la(D') simulation U'' and pruning A
//   with P(U'',strict D');
//    7) checking if the resulting automaton is equivalent to the initial one (which is done using libvata's
//   algorithm for lang. inclusion in both directions). */
//static void CheckConsistency(unsigned int la) {
//    auto start = std::chrono::high_resolution_clock::now();
//    auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type.

//    vector<filename> filenames = getAllFilenames(DIRS);
//    const unsigned int NUMB_TESTS = filenames.size();
//    Success* success = new Success(numbRelations, numbQuotientings, numbPrunings);
//    unsigned int numb_failures = 0;
//    Reduction* reduction = new Reduction();
//    Timeout* timeout = new Timeout();

//    vector<unsigned int> sizesRel(numbRelations);
//    vector<float>        numbRefs(numbRelations);
//    vector<float>        timesRel(numbRelations);
//    vector<float>        timesQuot(numbQuotientings);
//    vector<float>        timesPrun(numbPrunings);

//    for (unsigned int i=0; i<NUMB_TESTS; i++)
//    {
//        std::cout << "  Test " << i+1 << " - " << filenames.at(i) << ":\n";
//        string autStr = VATA::Util::ReadFile(filenames.at(i));

//        /* Parsing and taking data from the automaton. */
//        stateDict stateDict;
//        tuple<Automaton,unsigned int, unsigned int> parsing_tuple = parseFromString(autStr,stateDict,false,false);
//        Automaton aut_i = std::get<0>(parsing_tuple);
//         unsigned int numb_states_i = std::get<1>(parsing_tuple);        // What about the unreachable states?
//        const unsigned int numb_transitions_i = getNumbTransitions(aut_i);
//        const unsigned int numb_symbols = std::get<2>(parsing_tuple);       // the number of symbols is the same throughout the reduction methods
//        const vector<typerank> ranks = getRanks(aut_i,numb_symbols);
//        const float avg_ranking = getAvgRank(aut_i);
//        const float transDens_i = getTransitionDensity(aut_i);

//        //printAut(aut_i,stateDict,false);
//        if (numb_transitions_i == 0)
//        {
//            std::cout << "This automaton has 0 transitions and therefore will be skipped. Please remove all empty automata from the sample before running this test, to make the measure values more meaningful. \n\n";
//            continue;
//        }

//        /* Common declarations. */
//        float refin;
//        vector<bool> column(numb_states_i);

//        /* Computing the dw-la sim. */
//        std::cout << "Computing the dw-" << la << " simulation... \n";
//        vector<vector<bool> > dw_la(numb_states_i,column);
//        start = std::chrono::high_resolution_clock::now();
//        try {
//            refin = dw_simulation(aut_i,la,dw_la,numb_states_i,ranks);
//        }
//        catch (timeout_& e) {
//            std::cout << "TIMEOUT: The test took longer than "
//                      << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
//            timeout.inc();
//            timeout.checkSmallest(getNumbUsedStates(aut_i), getNumbTransitions(aut_i),
//     (float) getNumbUsedStates(aut_i) / ((float) getNumbTransitions(aut_i) * (float) numb_symbols));
//            continue;
//        }
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        timesRel[DW_LA_1ST_TIME] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        sizesRel[DW_LA_1ST_TIME] = getSizeOfRel(dw_la, numb_states_i);
//        numbRefs[DW_LA_1ST_TIME] = refin;
//        std::cout << "The dw-" << la << " simulation has " << sizesRel[DW_LA_1ST_TIME] << " pairs and ";
//        std::cout << "took " << numbRefs[DW_LA_1ST_TIME] << " matrix-refinements and "
//                  << timesRel[DW_LA_1ST_TIME] << " seconds to compute. ";

//        /* Quotient with dw-la sim. */
//        start = std::chrono::high_resolution_clock::now();
//        Automaton aut = quotientAutomaton(aut_i,transClosure(dw_la, numb_states_i), numb_states_i);
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        timesQuot[QUOT_WITH_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "The quotienting took " << timesQuot[QUOT_WITH_DW_LA] << " seconds ";

//        /* Prune with P(id,dw-la sim). */
//        start = std::chrono::high_resolution_clock::now();
//        aut = prune(aut, generateIdRelation(numb_states_i), dw_la, true);
//        // We must generate the id relation based on the number of states which existed before the quotienting, since the quotienting does not force the reindexing of the states. Alternatively, we could try to force this reindexation.
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        timesPrun[PRUN_WITH_P_ID_AND_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "and the pruning took " << timesPrun[PRUN_WITH_P_ID_AND_DW_LA] << " seconds. ";


////        Automaton aut_a = aut;
////        numb_states_i = reindexStates(aut);
////        if (!numb_states_i==getNumbUsedStates(aut)) exit_with_error("");

////        bool result1x = VATA::ExplicitTreeAut::CheckInclusion(aut_a, aut);
////        bool result2x = VATA::ExplicitTreeAut::CheckInclusion(aut, aut_a);
////        if (!(result1x && result2x))
////            exit_with_error("Error\n");


//        /* Computing the up-la(id) sim. */
//        std::cout << "\nComputing the up-" << la << "(id) simulation... \n";
//        start = std::chrono::high_resolution_clock::now();
//        //column.resize(numb_states_i);
//        vector<vector<bool> > up_la_with_id_1st_time(numb_states_i, column);
//        try
//        {
//            refin = up_simulation(aut, la, generateIdRelation(numb_states_i), false, up_la_with_id_1st_time, numb_states_i, numb_symbols, ranks);
//        }
//        catch (timeout_& e) {
//            std::cout << "TIMEOUT: Computing the up-la(id) simulation took longer than "
//                      << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
//            timeout.inc();
//            timeout.checkSmallest(getNumbUsedStates(aut), getNumbTransitions(aut),
//      (float) getNumbUsedStates(aut) / ((float) getNumbTransitions(aut) * (float) numb_symbols));
//            continue;
//        }
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        sizesRel[UP_LA_WITH_ID_1ST_TIME] = getSizeOfRel(up_la_with_id_1st_time, numb_states_i);
//        numbRefs[UP_LA_WITH_ID_1ST_TIME] = refin;
//        timesRel[UP_LA_WITH_ID_1ST_TIME] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "The up-" << la << "(id) simulation has "
//                  << sizesRel[UP_LA_WITH_ID_1ST_TIME] << " pairs ";
//        std::cout << "and it took "
//                  << numbRefs[UP_LA_WITH_ID_1ST_TIME]
//                  << " matrix-refinements and "
//                  << timesRel[UP_LA_WITH_ID_1ST_TIME] << " seconds to compute and ";

//        /* Quotienting with the up-la(id) sim the 1st time. */
//        start = std::chrono::high_resolution_clock::now();
//        aut = quotientAutomaton(aut,transClosure(up_la_with_id_1st_time, numb_states_i), numb_states_i);
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        timesQuot[QUOT_WITH_UP_LA_WITH_ID_1ST] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "the quotienting took "
//                  << timesQuot[QUOT_WITH_UP_LA_WITH_ID_1ST] << " seconds. ";

//        /* Computing the strict up-1(id) sim. */
//        std::cout << "\nComputing the strict up-1(id) simulation... \n";
//        vector<vector<bool> > strict_up_1_with_id (numb_states_i, column);
//        start = std::chrono::high_resolution_clock::now();
//        try
//        {
//            refin = up_simulation_strict(aut, 1, generateIdRelation(numb_states_i), false, strict_up_1_with_id, numb_states_i, numb_symbols, ranks);
//        }
//        catch (timeout_& e) {
//            std::cout << "TIMEOUT: Computing the up-la(id) simulation took longer than "
//                      << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
//            timeout.inc();
//            timeout.checkSmallest(getNumbUsedStates(aut), getNumbTransitions(aut),
//      (float) getNumbUsedStates(aut) / ((float) getNumbTransitions(aut) * (float) numb_symbols));
//            continue;
//        }
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        sizesRel[STRICT_UP_1_WITH_ID] = getSizeOfRel(strict_up_1_with_id, numb_states_i);
//        numbRefs[STRICT_UP_1_WITH_ID] = refin;
//        timesRel[STRICT_UP_1_WITH_ID] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "The strict up-1(id) simulation has "
//                  << sizesRel[STRICT_UP_1_WITH_ID] << " pairs ";
//        std::cout << "and the computation of the relation took "
//                  << numbRefs[STRICT_UP_1_WITH_ID]
//                  << " matrix-refinements and " << timesRel[STRICT_UP_1_WITH_ID] << " seconds. ";

////        /* Computing the dw-la sim again. */
////        // Can I base the computation on the same matrix used before for dw-la?
////        // And does this relation actually need be recomputed?
////        std::cout << "\nComputing the dw-" << la << " simulation again... \n";
////        dw_la.assign(numb_states_i,column);
////        start = std::chrono::high_resolution_clock::now();
////        try
////        {
////            refin = dw_simulation(aut, la, dw_la, numb_states_i, ranks);
////        }
////        catch (timeout_& e)
////        {
////            std::cout << "TIMEOUT: The test took longer than "
////                      << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
////            timeout.inc();
////            timeout.checkSmallest(getNumbUsedStates(aut), getNumbTransitions(aut),
////     (float) getNumbUsedStates(aut) / ((float) getNumbTransitions(aut) * (float) numb_symbols));
////            continue;
////        }
////        elapsed = std::chrono::high_resolution_clock::now() - start;
////        /*timesRel[DW_LA_2ND_TIME] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
////        sizesRel[DW_LA_2ND_TIME] = getSizeOfRel(dw_la, numb_states_i);
////        numbRefs[DW_LA_2ND_TIME] = refin;
////        std::cout << "The dw-" << la << " simulation has " << sizesRel[DW_LA_2ND_TIME] << " pairs and ";
////        std::cout << "took " << numbRefs[DW_LA_2ND_TIME] << " matrix-refinements and "
////                  << timesRel[DW_LA_2ND_TIME] << " seconds to compute. ";*/

//        /* Pruning with P(strict up-1(id) sim, dw-la sim) */
//        start = std::chrono::high_resolution_clock::now();
//        aut = prune(aut, strict_up_1_with_id, dw_la, false);
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        timesPrun[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "The pruning took " << timesPrun[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA] << " seconds and ";

//        /* Quotienting again with the up-la(id) sim. */
//        start = std::chrono::high_resolution_clock::now();
//        aut = quotientAutomaton(aut,transClosure(up_la_with_id_1st_time, numb_states_i), numb_states_i);
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        timesQuot[QUOT_WITH_UP_LA_WITH_ID_2ND] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "the quotienting took "
//                  << timesQuot[QUOT_WITH_UP_LA_WITH_ID_2ND] << " seconds. ";

//        /* Computing the dw-1 sim. */
//        std::cout << "\nComputing the dw-1 simulation... \n";
//        vector<vector<bool> > dw_1 (numb_states_i,column);
//        start = std::chrono::high_resolution_clock::now();
//        try
//        {
//            refin = dw_simulation(aut, 1, dw_1, numb_states_i, ranks);   // is la>1 good too?
//        }
//        catch (timeout_& e)
//        {
//            std::cout << "TIMEOUT: The test took longer than "
//                      << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
//            timeout.inc();
//            timeout.checkSmallest(getNumbUsedStates(aut), getNumbTransitions(aut),
//     (float) getNumbUsedStates(aut) / ((float) getNumbTransitions(aut) * (float) numb_symbols));
//            continue;
//        }
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        timesRel[DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        sizesRel[DW_1] = getSizeOfRel(dw_1, numb_states_i);
//        numbRefs[DW_1] = refin;
//        std::cout << "The dw-1 simulation has " << sizesRel[DW_1] << " and ";
//        std::cout << "took " << numbRefs[DW_1] << " matrix-refinements and "
//                  << timesRel[DW_1] << " seconds to compute. ";

//        /* Computing the up-la(dw-1) sim */
//        std::cout << "\nComputing the up-" << la << "(dw-1) simulation... \n";
//        start = std::chrono::high_resolution_clock::now();
//        vector<vector<bool> > up_la_with_dw_1 (numb_states_i,column);
//        try
//        {
//            refin = up_simulation(aut, la, dw_1, false, up_la_with_dw_1, numb_states_i, numb_symbols, ranks);
//        }
//        catch (timeout_& e)
//        {
//            std::cout << "TIMEOUT: Computing the up-la(id) simulation took longer than "
//                      << (float) TIMEOUT/60 << " minutes and therefore was aborted.\n";
//            timeout.inc();
//            timeout.checkSmallest(getNumbUsedStates(aut), getNumbTransitions(aut),
//      (float) getNumbUsedStates(aut) / ((float) getNumbTransitions(aut) * (float) numb_symbols));
//            continue;
//        }
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        sizesRel[UP_LA_WITH_DW_1] = getSizeOfRel(up_la_with_dw_1, numb_states_i);
//        numbRefs[UP_LA_WITH_DW_1] = refin;
//        timesRel[UP_LA_WITH_DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "The up-" << la << "(dw-1) simulation has "
//                  << sizesRel[UP_LA_WITH_DW_1] << " pairs and ";
//        std::cout << "the computation of the relation took " << numbRefs[UP_LA_WITH_DW_1]
//                  << " matrix-refinements and "
//                  << timesRel[UP_LA_WITH_DW_1] << " seconds to compute, ";

//        /* Pruning with P(up-la(dw-1) sim, strict dw-1 sim) */
//        start = std::chrono::high_resolution_clock::now();
//        aut = prune(aut, up_la_with_dw_1, dw_1, true);
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        timesPrun[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
//        std::cout << "the pruning took " << timesPrun[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1] << " seconds.";


//        start = std::chrono::high_resolution_clock::now();
//        bool result = equiv(aut_i, aut);
//        elapsed = std::chrono::high_resolution_clock::now() - start;
//        float time_consCheck = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

//        //std::cout << "The reduced automaton is: \n";
//        //printAut(aut,stateDict,false);

//        if (result) {
//            std::cout << "\nThe consistency check took " << time_consCheck
//                      << " seconds. ";
//            const unsigned int numb_states_f = getNumbUsedStates(aut);
//            float states_red;       // states_red = how many (%) of the initial states are still present after the reduction.
//            if (numb_states_i == 0)
//                states_red = 100;
//            else
//                states_red = ((float) numb_states_f / (float) numb_states_i) * (float) 100;
//            const unsigned int numb_transitions_f = getNumbTransitions(aut);
//            float transitions_red;  // transitions_red = how many (%) of the initial transitions are still present after the reduction.
//            if (numb_transitions_i == 0)
//                transitions_red = 100;
//            else
//                transitions_red = ((float) numb_transitions_f / (float) numb_transitions_i) * (float) 100;
//            float transDens_f
//                    = (float) numb_transitions_f / ( (float) numb_states_f * (float) numb_symbols);
//            float transDens_red = (transDens_f / transDens_i) * (float) 100;

//            std::cout
//                    << "The reduction techniques preserved the language and the resulting automaton has "
//                    << std::to_string(states_red) << "% of its states, " << std::to_string(transitions_red)
//                    << "% of its transitions and " << std::to_string(transDens_red)
//                    << "% of its transition density. \n";

//            //std::cout << "The reduced automaton is: \n";
//            //printAut(aut,stateDict,false);

//            success.inc();
//            success.updateInitialAvg(numb_states_i, numb_transitions_i, numb_symbols, avg_ranking, transDens_i);
//            success.checkInitialGreatest(numb_states_i, numb_transitions_i, numb_symbols, avg_ranking, transDens_i);
//            success.updateAvgSizesRels(sizesRel);
//            success.updateAvgNumbsRefs(numbRefs);
//            success.updateAvgTimes(timesRel, timesQuot, timesPrun, time_consCheck);

//            if (numb_states_f < numb_states_i || numb_transitions_f < numb_transitions_i)
//            {
//                reduction.inc();
//                reduction.updateAvg(states_red, transitions_red);
//                reduction.checkGreatest(states_red, transitions_red);
//            }
//        }
//        else
//        {

//            exit_with_error("The reduction techniques did NOT preserve the language!\n");
//            numb_failures++;
//        }
//        std::cout << endl;
//    }

//    printSummaryConsistency(numb_failures, NUMB_TESTS, la, success, reduction, timeout);
//}

//static void test(Automaton& aut_i, stateDict stateDict)
//{

//    unsigned int numb_states = getNumbUsedStates(aut_i);

//    vector<bool> column(numb_states);

//    /* Computing the dw-1 sim. */
//    vector<vector<bool> > dw_la(numb_states,column);
//    stateDiscontBinaryRelation lv_sim = computeLVDwSimulation(aut_i, numb_states);
//    unsigned int c=0;
//    for (unsigned int p=0; p<stateDict.size(); p++)
//        for (unsigned int q=0; q<stateDict.size(); q++) {
//            if (lv_sim.get(p,q))
//                c++;
//        }

//    /* Quotient with dw-la sim. */
//    Automaton aut = quotientAutomaton(aut_i,transClosure(dw_la, numb_states), numb_states);

//    /* Computing the dw-1 sim. again */
//    lv_sim = computeLVDwSimulation(aut, /*numb_states*/ getNumbUsedStates(aut));
//    c = 0;
//    for (unsigned int p=0; p</*stateDict.size()*/ lv_sim.size(); p++)
//        for (unsigned int q=0; q</*stateDict.size()*/ lv_sim.size(); q++) {
//            if (lv_sim.get(p,q))
//                c++;
//        }

//}

