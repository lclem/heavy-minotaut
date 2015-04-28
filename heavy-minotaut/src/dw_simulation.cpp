
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file for the computation of the lookahead downward           *
 *  simulation (strict or non-strict). It also defines functions which call         *
 *  libvata's ordinary simulation algorithm.                                        *
 * 																					*
 ************************************************************************************/

#include "dw_simulation.hh"

// Global variables of this file
unsigned int visits_Gl;
QueriesCounter counter_localGAH, counter_localBAH, counter_semiGlobalGAH, counter_semiGlobalBAH, counter_globalGAH, counter_globalBAH;

code CODE_DEL, CODE_DEL_POW;

bool defend(const Automaton& aut, const unsigned int numb_states, const state initialState, Step& step, const state q, const vector<bool>& isFinal, const vector<typerank>& ranks, const vector<vector<bool> >& W, vector<vector<set<code> > >& historyOfBadAtks_global, vector<vector<set<code> >>& historyOfGoodAtks_global, float& codeGenerationTime, float& historiesTimeConsump, vector<set<node_no> >& historyOfBadAtks_semi_global, vector<set<node_no> >& historyOfGoodAtks_semi_global) {
     state p = step.getState();
     code step_headlessCode = "";
     auto start = std::chrono::high_resolution_clock::now();
     auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type

     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS]==GLOBAL || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS]==GLOBAL)
     {
         start = std::chrono::high_resolution_clock::now();
         step_headlessCode = step.getHeadlessCode();
         elapsed = std::chrono::high_resolution_clock::now() - start;
         codeGenerationTime += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
     }
     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL)
     {
         start = std::chrono::high_resolution_clock::now();
         bool find1 = (historyOfGoodAtks_global.at(q).at(p).find(step_headlessCode)) != (historyOfGoodAtks_global.at(q).at(p).end());
         elapsed = std::chrono::high_resolution_clock::now() - start;
         historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
         if (find1) {
            counter_globalGAH.incNumbSucLookups();
            return false;
         }
         else
             counter_globalGAH.incNumbUnsucLookups();
     }
     if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL)
     {
         start = std::chrono::high_resolution_clock::now();
         bool find2 = (historyOfBadAtks_global.at(q).at(p).find(step_headlessCode)) != (historyOfBadAtks_global.at(q).at(p).end());
         elapsed = std::chrono::high_resolution_clock::now() - start;
         historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
         if (find2) {
             counter_globalBAH.incNumbSucLookups();
             return true;
         }
         else
             counter_globalBAH.incNumbUnsucLookups();
     }
     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == SEMI_GLOBAL)
     {
         node_no node = step.getNode();
         if (historyOfGoodAtks_semi_global.at(q).find(node) != historyOfGoodAtks_semi_global.at(q).end())
         {
             counter_semiGlobalGAH.incNumbSucLookups();
             return false;
         }
         else
             counter_semiGlobalGAH.incNumbUnsucLookups();
     }
     if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == SEMI_GLOBAL)
     {
         node_no node = step.getNode();
         if (historyOfBadAtks_semi_global.at(q).find(node) != historyOfBadAtks_semi_global.at(q).end())
         {
             counter_semiGlobalBAH.incNumbSucLookups();
             return true;
         }
         else
             counter_semiGlobalBAH.incNumbUnsucLookups();
     }

     vector<vector<bool> > historyOfBadAtks_local;
     if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == LOCAL)
     {
         vector<bool> column(step.numbChildren(),false);
         historyOfBadAtks_local.assign(numb_states,column);
     }

     vector<vector<bool> > historyOfGoodAtks_local;
     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL)
     {
         vector<bool> column(step.numbChildren(),false);
         historyOfGoodAtks_local.assign(numb_states,column);
     }

    if (p==initialState && q!=initialState)
    {
        return false;
    }

    if (!step.isFirstStep() && W[p][q])
        return true;

    if(step.isALeafStep())
        return W[p][q];

    unsigned int s = step.getSymbol();
    vector<Step> next = step.getNext();
    typerank rank = ranks[s];
    for (const transition& t : aut[q]){
        if (t.GetSymbol() == s){
            bool ok = true;
            for (unsigned int i=0; i<rank && ok; i++)
            {
                Step next_i;
                state q_i = NO_STATE;
                try{
                    next_i = next.at(i);
                }
                catch (const std::out_of_range& oor) {
                    std::cerr << "Out of Range error: " << oor.what();
                    exit_with_error(" when acessing next.");
                }
                try {
                    q_i = t.GetChildren().at(i);
                }
                catch (const std::out_of_range& oor) {
                    std::cerr << "Out of Range error: " << oor.what();
                    exit_with_error(" when accessing the position i of the return of GetChildren() of a transition.");
                }

                if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == LOCAL)
                {
                    if (historyOfBadAtks_local.at(q_i).at(i))
                    {
                        counter_localBAH.incNumbSucLookups();
                        continue;
                    }
                    else
                        counter_localBAH.incNumbUnsucLookups();
                }

                if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL)
                {
                    if (historyOfGoodAtks_local.at(q_i).at(i))
                    {
                        counter_localGAH.incNumbSucLookups();
                        ok = false;
                        continue;
                    }
                    else
                        counter_localGAH.incNumbUnsucLookups();
                }

                if (!defend(aut, numb_states, initialState, next_i, q_i, isFinal, ranks, W, historyOfBadAtks_global, historyOfGoodAtks_global, codeGenerationTime, historiesTimeConsump, historyOfBadAtks_semi_global, historyOfGoodAtks_semi_global)){
                    ok = false;

                    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL)
                    {
                        historyOfGoodAtks_local.at(q_i).at(i) = true;
                        counter_localGAH.incNumbInserts();
                    }
                }
                else
                    if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == LOCAL)
                    {
                        counter_localBAH.incNumbInserts();
                        historyOfBadAtks_local.at(q_i).at(i) = true;
                    }
            }
            if (ok)	{

                if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL)
                {
                    start = std::chrono::high_resolution_clock::now();
                    historyOfBadAtks_global.at(q).at(p).insert(step_headlessCode);
                    elapsed = std::chrono::high_resolution_clock::now() - start;
                    historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                    counter_globalBAH.incNumbInserts();
                }
                if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == SEMI_GLOBAL)
                {
                    node_no node = step.getNode();
                    historyOfBadAtks_semi_global.at(q).insert(node);
                    counter_semiGlobalBAH.incNumbInserts();
                }
                return true;
            }
        }
    }

    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL)
    {
        start = std::chrono::high_resolution_clock::now();
        historyOfGoodAtks_global.at(q).at(p).insert(step_headlessCode);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        counter_globalGAH.incNumbInserts();
    }
    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == SEMI_GLOBAL)
    {
        node_no node = step.getNode();
        historyOfGoodAtks_semi_global.at(q).insert(node);
        counter_semiGlobalGAH.incNumbInserts();
    }
    return false;
}

/* 'steps' is a matrix representing an attack being considered: at each
 * index i, it contains a vector of pointers to the steps of the attack which have depth i.
 * The function 'attack' returns True iff there is at least one successful attack against 'q'
 * which has as prefix the attack represented by 'steps'. 'depth' gives the depth of the
 * current attack, and 'la' gives the lookahead / maximum depth allowed in an attack. */
bool attack(const Automaton& aut, const unsigned int numb_states, const state initialState, vector<vector<Step*> >& steps, const state q, const unsigned int depth, const unsigned int la, const vector<bool>& isFinal, const vector<typerank>& ranks, const vector<vector<bool> >& W, vector<vector<set<code> > >& historyOfBadAtks_global, vector<vector<set<code> >>& historyOfGoodAtks_global, float& codeGenerationTime, float& historiesTimeConsump) {

    vector<set<node_no> > historyOfBadAtks_semi_global;
    if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == SEMI_GLOBAL)
    {
        set<node_no> empty_set;
        historyOfBadAtks_semi_global.assign(numb_states, empty_set);
    }

    vector<set<node_no> > historyOfGoodAtks_semi_global;
    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == SEMI_GLOBAL)
    {
        set<node_no> empty_set;
        historyOfGoodAtks_semi_global.assign(numb_states, empty_set);
    }

    if (depth == la) {		     /* Then there is a successful attack of prefix 'steps' if it is  */
        return !defend(aut, numb_states, initialState, firstStepOf(steps),    /* 'steps' itself.  */
                       q, isFinal, ranks, W, historyOfBadAtks_global,
                       historyOfGoodAtks_global, codeGenerationTime,
                       historiesTimeConsump, historyOfBadAtks_semi_global,
                       historyOfGoodAtks_semi_global);
    }

    bool result_def;
    if (depth > 0) {
        result_def = defend(aut, numb_states, initialState, firstStepOf(steps), q, isFinal, ranks, W, historyOfBadAtks_global, historyOfGoodAtks_global, codeGenerationTime, historiesTimeConsump, historyOfBadAtks_semi_global, historyOfGoodAtks_semi_global);
        if (result_def)
            return false;
    }

    vector<Step*> states;
    try{
        states = steps.at(depth);
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what();
        exit_with_error(" when acessing steps.");
    }

    transitions all_trans = mapGetTrans(aut, states);
    if (all_trans.size() == 0) {
        if (depth == 0)
            return false;
        else
            return !result_def;
    }

    if (OPT[ORDER_OF_ATTACKS_TRANS] == SEMI_ORDERED_BY_INITIAL_TRANS)
    {
        // sort each vector of trans in all_trans so that the trans (if any) to the initial state come first.
        for (unsigned int i=0; i<all_trans.size(); i++)
        {
            vector<transition> vec = all_trans.at(i);
            all_trans.at(i) = moveInitialTransitionsToBeginning(vec);
        }
    }
    else if (OPT[ORDER_OF_ATTACKS_TRANS] == SEMI_ORDERED_BY_SYMBOLS_RANKING)
    {
        // sort each vector of trans in all_trans so that trans by symbols with higher rank come first.
        for (unsigned int i=0; i<all_trans.size(); i++)
        {
            vector<transition> vec = all_trans.at(i);
            all_trans.at(i) = orderTransBySymbolsRankings(vec, ranks);
        }
    }

    vector<MaybeTransition> combination(all_trans.size());
    return attack_loop(aut, numb_states, initialState, isFinal, ranks, la, W, all_trans, 0, combination, steps, q, depth, historyOfBadAtks_global, historyOfGoodAtks_global, codeGenerationTime, historiesTimeConsump);

}

/* The function 'attack_loop' calls the function 'attack' on each different
 * possibility of extending the current attack, but exits (with true) when one of those
 * calls returns true. The combinations of the transitions used in the
 * extension of the attack are generated not a priori but on-the-fly. */
bool attack_loop(const Automaton& aut, const unsigned int numb_states,
                 const state initialState, const vector<bool>& isFinal,
                 const vector<typerank>& ranks, const unsigned int la,
                 const vector<vector<bool> >& W,
                 const vector<vector<transition> >& all_trans, const unsigned int index,
                 vector<MaybeTransition>& combination, vector<vector<Step*> > steps,
                 const state q, const unsigned int depth,
                 vector<vector<set<code> > >& historyOfBadAtks_global,
                 vector<vector<set<code> >>& historyOfGoodAtks_global,
                 float& codeGenerationTime, float& historiesTimeConsump)
{

    if (index == all_trans.size()) {
        extendAttack(steps, depth, combination, ranks);
        return attack(aut, numb_states, initialState, steps, q, depth+1, la, isFinal, ranks, W, historyOfBadAtks_global, historyOfGoodAtks_global, codeGenerationTime, historiesTimeConsump);
    }
    else {
        if (all_trans.at(index).empty()) {
            combination.at(index) = MaybeTransition();
            return attack_loop(aut, numb_states, initialState, isFinal, ranks, la, W, all_trans, index+1, combination, steps, q, depth, historyOfBadAtks_global, historyOfGoodAtks_global, codeGenerationTime, historiesTimeConsump);
        }
        else {
            for (unsigned int i=0; i < all_trans.at(index).size(); i++) {
                combination.at(index) = MaybeTransition(all_trans.at(index).at(i));
                if (attack_loop(aut, numb_states, initialState, isFinal, ranks, la, W, all_trans, index+1, combination, steps, q, depth, historyOfBadAtks_global, historyOfGoodAtks_global, codeGenerationTime, historiesTimeConsump))
                    return true;
                }
            }
        }

    return false;
}

float refine(const Automaton& aut, const state initialState,
             const vector<bool>& isFinal, const vector<typerank>& ranks,
             const unsigned int la, vector<vector<bool> >& W,
             const unsigned int n,
             vector<vector<set<code> > >& historyOfBadAtks_global,
             vector<vector<set<code> >>& historyOfGoodAtks_global,
             float& codeGenerationTime, float& historiesTimeConsump,
             timespec* timestart_timeout = NULL, unsigned int timeout = 0) {
    unsigned int counter = 0, visits = 0;
    vector<vector<Step*> > steps;

    if (timestart_timeout == NULL)
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, timestart_timeout);

    try {
        while (true) {
            for (state p=0; p<n; p++)
                for (state q=0; q<n; q++) {
                    visits++;

                    timespec nowTime;
                    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &nowTime);
                    if (nowTime.tv_sec - (*timestart_timeout).tv_sec >
                            (timeout==0) ? TIMEOUT : timeout)
                    {
                        throw timeout_();
                    }

                    if (!W[p][q] || q==p) {
                        /* False remains False, and W always includes the id relation. */
                        if (++counter == n*n) {
                            throw finished();
                        }
                        else
                            continue;
                    }

                    Step first = Step(p,true);
                    vector<Step*> s(1);
                    s.at(0) = &first;
                    steps.resize(1);
                    steps.at(0) = s;

                    if (attack(aut, n, initialState, steps, q, 0, la, isFinal, ranks, W,
                               historyOfBadAtks_global, historyOfGoodAtks_global,
                               codeGenerationTime, historiesTimeConsump)) {
                        W[p][q] = false;
                        counter = 0;
                        if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL) {
                            auto start = std::chrono::high_resolution_clock::now();
                            for (vector<set<code> >& v : historyOfBadAtks_global)
                                for (set<code>& s : v)
                                    s.clear();
                            auto elapsed = std::chrono::high_resolution_clock::now() - start;
                            historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                        }
                    }
                    else {
                        if (++counter == n*n)
                            throw finished();
                    }
                }
        }
    }
    catch (finished& e)
    {
    }

    return (float) visits / ((float) n*n);
}

defence defend_with_3VL_V2(const Automaton& aut, const unsigned int numb_states,
                           const state initialState, Step& step, const state q,
                           const vector<bool>& isFinal, const vector<typerank>& ranks,
                           vector<vector<bool> >& W, bool& hasWChanged,
                           vector<vector<codes_map > >& historyOfBadAtks_global,
                           vector<vector<codes_map > >& historyOfGoodAtks_global,
                           float& codeGenerationTime, float& historiesTimeConsump,
                           vector<nodes_map >& historyOfBadAtks_semi_global,
                           vector<nodes_map >& historyOfGoodAtks_semi_global)
{
     state p = step.getState();
     code step_code;
     auto start = std::chrono::high_resolution_clock::now();
     auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type

     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL)
     {
         start = std::chrono::high_resolution_clock::now();
         step_code = step.getHeadlessCode();
         elapsed = std::chrono::high_resolution_clock::now() - start;
         codeGenerationTime += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
     }
     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL_V2 || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL_V2)
     {
         start = std::chrono::high_resolution_clock::now();
         step_code = step.getSetCode();
         elapsed = std::chrono::high_resolution_clock::now() - start;
         codeGenerationTime += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
     }
     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL || OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL_V2)
     {
         start = std::chrono::high_resolution_clock::now();
         codes_map::const_iterator got = historyOfGoodAtks_global.at(q).at(p).find(step_code);
         elapsed = std::chrono::high_resolution_clock::now() - start;
         historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
         if (got != (historyOfGoodAtks_global.at(q).at(p).end())) {
            counter_globalGAH.incNumbSucLookups();
            return got->second;
         }
         else
             counter_globalGAH.incNumbUnsucLookups();
     }
     if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL_V2)
     {
         start = std::chrono::high_resolution_clock::now();
         bool find2 = (historyOfBadAtks_global.at(q).at(p).find(step_code)) != (historyOfBadAtks_global.at(q).at(p).end());
         elapsed = std::chrono::high_resolution_clock::now() - start;
         historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
         if (find2) {
             counter_globalBAH.incNumbSucLookups();
             return success;
         }
         else
             counter_globalBAH.incNumbUnsucLookups();
     }
     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == SEMI_GLOBAL)
     {
         node_no node = step.getNode();
         codes_map::const_iterator got = historyOfGoodAtks_semi_global.at(q).find(node);
         if (got != historyOfGoodAtks_semi_global.at(q).end())
         {
             counter_semiGlobalGAH.incNumbSucLookups();
             return got->second;
         }
         else
             counter_semiGlobalGAH.incNumbUnsucLookups();
     }
     if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == SEMI_GLOBAL)
     {
         node_no node = step.getNode();
         if (historyOfBadAtks_semi_global.at(q).find(node) != historyOfBadAtks_semi_global.at(q).end())
         {
             counter_semiGlobalBAH.incNumbSucLookups();
             return success;
         }
         else
             counter_semiGlobalBAH.incNumbUnsucLookups();
     }

     vector<vector<defence> > historyOfBadAtks_local;
     if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == LOCAL)
     {
         vector<defence> column(step.numbChildren(),false);
         historyOfBadAtks_local.assign(numb_states,column);
     }

     vector<vector<defence> > historyOfGoodAtks_local;
     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL)
     {
         vector<defence> column(step.numbChildren(),false);
         historyOfGoodAtks_local.assign(numb_states,column);
     }

    if (p==initialState && q!=initialState)
    {
        return strong_fail;
    }

    if (!step.isFirstStep() && W[p][q])
    {
        return success;
    }

    if (step.isALeafStep())
    {
        if (W[p][q])
            return success;
        else
        {
            return weak_fail;
        }
    }

    unsigned int s = step.getSymbol();
    vector<Step> next = step.getNext();
    typerank rank = ranks[s];
    defence global_result = strong_fail;
    for (const transition& t : aut[q])
    {
        if (t.GetSymbol() == s)
        {
            defence local_result = success;
            for (unsigned int i=0; i<rank; i++)
            {
                Step next_i;
                state q_i = NO_STATE;
                try
                {
                    next_i = next.at(i);
                }
                catch (const std::out_of_range& oor)
                {
                    std::cerr << "Out of Range error: " << oor.what();
                    exit_with_error(" when acessing next.");
                }
                try
                {
                    q_i = t.GetChildren().at(i);
                }
                catch (const std::out_of_range& oor) {
                    std::cerr << "Out of Range error: " << oor.what();
                    exit_with_error(" when accessing the position i of the return of GetChildren() of a transition.");
                }

                if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == LOCAL)
                {
                    if (isSuccess(historyOfBadAtks_local.at(q_i).at(i)))
                    {
                        counter_localBAH.incNumbSucLookups();
                        continue;
                    }
                    else
                        counter_localBAH.incNumbUnsucLookups();
                }

                if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL)
                {
                    if (isStrongFail(historyOfGoodAtks_local.at(q_i).at(i)))
                    {
                        counter_localGAH.incNumbSucLookups();
                        local_result = strong_fail;
                        break;
                    }
                    else if (isWeakFail(historyOfGoodAtks_local.at(q_i).at(i)))
                    {
                        counter_localGAH.incNumbSucLookups();
                        local_result = weak_fail;
                        /*continue;*/ break;
                    }
                    else
                        counter_localGAH.incNumbUnsucLookups();
                }

                defence result_def
                        = defend_with_3VL_V2(aut, numb_states, initialState,
                                             next_i, q_i, isFinal, ranks, W,
                                             hasWChanged, historyOfBadAtks_global,
                                             historyOfGoodAtks_global,
                                             codeGenerationTime, historiesTimeConsump,
                                             historyOfBadAtks_semi_global,
                                             historyOfGoodAtks_semi_global);
                if (isSuccess(result_def))
                {
                    if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == LOCAL)
                    {
                        counter_localBAH.incNumbInserts();
                        historyOfBadAtks_local.at(q_i).at(i) = success;
                    }
                }
                else
                {
                    if (isWorse(result_def,local_result))
                        local_result = result_def;

                    if (isStrongFail(result_def))
                    {
                         // The following 2 code lines appear to create a bottleneck.
                         W[next_i.getState()][q_i] = false;
                         hasWChanged = true;

                         if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL)
                        {
                            historyOfGoodAtks_local.at(q_i).at(i) = strong_fail;
                            counter_localGAH.incNumbInserts();
                        }
                        break;
                    }

                    if (isWeakFail(result_def))
                    {
                        if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL)
                        {
                            historyOfGoodAtks_local.at(q_i).at(i) = weak_fail;
                            counter_localGAH.incNumbInserts();
                        }
                        break;
                    }
                }
            }

            if (isBetter(local_result, global_result))
                global_result = local_result;

            if (isSuccess(local_result))	{

                if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL_V2)
                {
                    start = std::chrono::high_resolution_clock::now();
                    pair<code,defence> pair_ (step_code, success);
                    historyOfBadAtks_global.at(q).at(p).insert(pair_);
                    elapsed = std::chrono::high_resolution_clock::now() - start;
                    historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                    counter_globalBAH.incNumbInserts();
                }
                if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == SEMI_GLOBAL)
                {
                    pair<node_no,defence> pair_ (step.getNode(), success);
                    historyOfBadAtks_semi_global.at(q).insert(pair_);
                    counter_semiGlobalBAH.incNumbInserts();
                }
                return success;
            }
        }
    }

    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL || OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL_V2)
    {
        start = std::chrono::high_resolution_clock::now();
        pair<code,defence> pair_ (step_code, global_result);
        historyOfGoodAtks_global.at(q).at(p).insert(pair_);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        counter_globalGAH.incNumbInserts();
    }
    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == SEMI_GLOBAL)
    {
        pair<node_no,defence> pair_ (step.getNode(), global_result);
        historyOfGoodAtks_semi_global.at(q).insert(pair_);
        counter_semiGlobalGAH.incNumbInserts();
    }

    return global_result;
}

defence defend_with_3VL_V1(const Automaton& aut, const unsigned int numb_states,
                           const state initialState, Step& step, const state q,
                           const vector<bool>& isFinal, const vector<typerank>& ranks,
                           vector<vector<bool> >& W, bool& hasWChanged,
                           vector<vector<codes_map > >& historyOfBadAtks_global,
                           vector<vector<codes_map > >& historyOfGoodAtks_global,
                           float& codeGenerationTime, float& historiesTimeConsump,
                           vector<nodes_map >& historyOfBadAtks_semi_global,
                           vector<nodes_map >& historyOfGoodAtks_semi_global)
{
     state p = step.getState();
     code step_code;
     auto start = std::chrono::high_resolution_clock::now();
     auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type

     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL)
     {
         start = std::chrono::high_resolution_clock::now();
         step_code = step.getHeadlessCode();
         elapsed = std::chrono::high_resolution_clock::now() - start;
         codeGenerationTime += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
     }
     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL_V2 || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL_V2)
     {
         start = std::chrono::high_resolution_clock::now();
         step_code = step.getSetCode();
         elapsed = std::chrono::high_resolution_clock::now() - start;
         codeGenerationTime += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
     }
     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL || OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL_V2)
     {
         start = std::chrono::high_resolution_clock::now();
         codes_map::const_iterator got = historyOfGoodAtks_global.at(q).at(p).find(step_code);
         elapsed = std::chrono::high_resolution_clock::now() - start;
         historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
         if (got != (historyOfGoodAtks_global.at(q).at(p).end())) {
            counter_globalGAH.incNumbSucLookups();
            return got->second;
         }
         else
             counter_globalGAH.incNumbUnsucLookups();
     }
     if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL_V2)
     {
         start = std::chrono::high_resolution_clock::now();
         bool find2 = (historyOfBadAtks_global.at(q).at(p).find(step_code)) != (historyOfBadAtks_global.at(q).at(p).end());
         elapsed = std::chrono::high_resolution_clock::now() - start;
         historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
         if (find2) {
             counter_globalBAH.incNumbSucLookups();
             return success;
         }
         else
             counter_globalBAH.incNumbUnsucLookups();
     }
     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == SEMI_GLOBAL)
     {
         node_no node = step.getNode();
         codes_map::const_iterator got = historyOfGoodAtks_semi_global.at(q).find(node);
         if (got != historyOfGoodAtks_semi_global.at(q).end())
         {
             counter_semiGlobalGAH.incNumbSucLookups();
             return got->second;
         }
         else
             counter_semiGlobalGAH.incNumbUnsucLookups();
     }
     if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == SEMI_GLOBAL)
     {
         node_no node = step.getNode();
         if (historyOfBadAtks_semi_global.at(q).find(node) != historyOfBadAtks_semi_global.at(q).end())
         {
             counter_semiGlobalBAH.incNumbSucLookups();
             return success;
         }
         else
             counter_semiGlobalBAH.incNumbUnsucLookups();
     }

     vector<vector<defence> > historyOfBadAtks_local;
     if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == LOCAL)
     {
         vector<defence> column(step.numbChildren(),false);
         historyOfBadAtks_local.assign(numb_states,column);
     }

     vector<vector<defence> > historyOfGoodAtks_local;
     if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL)
     {
         vector<defence> column(step.numbChildren(),false);
         historyOfGoodAtks_local.assign(numb_states,column);
     }

    if (p==initialState && q!=initialState)
    {
        return strong_fail;
    }

    if (!step.isFirstStep() && W[p][q])
    {
        return success;
    }

    if (step.isALeafStep())
    {
        if (W[p][q])
            return success;
        else
        {
            return weak_fail;
        }
    }

    unsigned int s = step.getSymbol();
    vector<Step> next = step.getNext();
    typerank rank = ranks[s];
    defence global_result = strong_fail;
    for (const transition& t : aut[q])
    {
        if (t.GetSymbol() == s)
        {
            defence local_result = success;
            for (unsigned int i=0; i<rank; i++)
            {
                Step next_i;
                state q_i = NO_STATE;
                try
                {
                    next_i = next.at(i);
                }
                catch (const std::out_of_range& oor)
                {
                    std::cerr << "Out of Range error: " << oor.what();
                    exit_with_error(" when acessing next.");
                }
                try
                {
                    q_i = t.GetChildren().at(i);
                }
                catch (const std::out_of_range& oor) {
                    std::cerr << "Out of Range error: " << oor.what();
                    exit_with_error(" when accessing the position i of the return of GetChildren() of a transition.");
                }

                if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == LOCAL)
                {
                    if (isSuccess(historyOfBadAtks_local.at(q_i).at(i)))
                    {
                        counter_localBAH.incNumbSucLookups();
                        continue;
                    }
                    else
                        counter_localBAH.incNumbUnsucLookups();
                }

                if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL)
                {
                    if (isStrongFail(historyOfGoodAtks_local.at(q_i).at(i)))
                    {
                        counter_localGAH.incNumbSucLookups();
                        local_result = strong_fail;
                        break;
                    }
                    else if (isWeakFail(historyOfGoodAtks_local.at(q_i).at(i)))
                    {
                        counter_localGAH.incNumbSucLookups();
                        local_result = weak_fail;
                        continue;
                    }
                    else
                        counter_localGAH.incNumbUnsucLookups();
                }

                defence result_def
                        = defend_with_3VL_V1(aut, numb_states, initialState, next_i,
                                             q_i, isFinal, ranks, W, hasWChanged,
                                             historyOfBadAtks_global,
                                             historyOfGoodAtks_global,
                                             codeGenerationTime, historiesTimeConsump,
                                             historyOfBadAtks_semi_global,
                                             historyOfGoodAtks_semi_global);
                if (isSuccess(result_def))
                {
                    if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == LOCAL)
                    {
                        counter_localBAH.incNumbInserts();
                        historyOfBadAtks_local.at(q_i).at(i) = success;
                    }
                }
                else
                {
                    if (isWorse(result_def,local_result))
                        local_result = result_def;

                    if (isStrongFail(result_def))
                    {
                        if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL)
                        {
                            historyOfGoodAtks_local.at(q_i).at(i) = strong_fail;
                            counter_localGAH.incNumbInserts();
                        }
                        break;
                    }

                    if (isWeakFail(result_def))
                    {
                        if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL)
                        {
                            historyOfGoodAtks_local.at(q_i).at(i) = weak_fail;
                            counter_localGAH.incNumbInserts();
                        }
                    }
                }
            }

            if (isBetter(local_result, global_result))
                global_result = local_result;

            if (isSuccess(local_result))	{

                if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL_V2)
                {
                    start = std::chrono::high_resolution_clock::now();
                    pair<code,defence> pair_ (step_code, success);
                    historyOfBadAtks_global.at(q).at(p).insert(pair_);
                    elapsed = std::chrono::high_resolution_clock::now() - start;
                    historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                    counter_globalBAH.incNumbInserts();
                }
                if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == SEMI_GLOBAL)
                {
                    pair<node_no,defence> pair_ (step.getNode(), success);
                    historyOfBadAtks_semi_global.at(q).insert(pair_);
                    counter_semiGlobalBAH.incNumbInserts();
                }
                return success;
            }
        }
    }

    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL || OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL_V2)
    {
        start = std::chrono::high_resolution_clock::now();
        pair<code,defence> pair_ (step_code, global_result);
        historyOfGoodAtks_global.at(q).at(p).insert(pair_);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        counter_globalGAH.incNumbInserts();
    }
    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == SEMI_GLOBAL)
    {
        pair<node_no,defence> pair_ (step.getNode(), global_result);
        historyOfGoodAtks_semi_global.at(q).insert(pair_);
        counter_semiGlobalGAH.incNumbInserts();
    }

    return global_result;
}

/* 'steps' is a matrix representing an attack being considered: at each
 * index i, it contains a vector of pointers to the steps of the attack which have depth i.
 * The function 'attack' returns True iff there is at least one successful attack against 'q'
 * which has as prefix the attack represented by 'steps'. 'depth' gives the depth of the
 * current attack, and 'la' gives the lookahead / maximum depth allowed in an attack. */
bool attack_with_3VL(const Automaton& aut, const unsigned int numb_states,
                     const state initialState, vector<vector<Step*> >& steps,
                     const state q, const unsigned int depth, const unsigned int la,
                     const vector<bool>& isFinal, const vector<typerank>& ranks,
                     vector<vector<bool> >& W, bool& hasWChanged,
                     vector<vector<codes_map >>& historyOfBadAtks_global,
                     vector<vector<codes_map >>& historyOfGoodAtks_global,
                     float& codeGenerationTime, float& historiesTimeConsump)
{

    vector<nodes_map> historyOfBadAtks_semi_global;
    if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == SEMI_GLOBAL)
    {
        nodes_map empty_map;
        historyOfBadAtks_semi_global.assign(numb_states, empty_map);
    }

    vector<nodes_map> historyOfGoodAtks_semi_global;
    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == SEMI_GLOBAL)
    {
        nodes_map empty_map;
        historyOfGoodAtks_semi_global.assign(numb_states, empty_map);
    }

    defence result_def = success;
    if (depth == la)
    {		/* Then there is a successful attack of prefix 'steps' if it is 'steps' itself. */
        if (OPT[THREE_VALUES_LOGIC] == ON_V1)
            result_def = defend_with_3VL_V1(aut, numb_states, initialState,
                                            firstStepOf(steps), q, isFinal, ranks,
                                            W, hasWChanged, historyOfBadAtks_global,
                                            historyOfGoodAtks_global, codeGenerationTime,
                                            historiesTimeConsump,
                                            historyOfBadAtks_semi_global,
                                            historyOfGoodAtks_semi_global);
        else if (OPT[THREE_VALUES_LOGIC] == ON_V2)
            result_def = defend_with_3VL_V2(aut, numb_states, initialState,
                                            firstStepOf(steps), q, isFinal, ranks,
                                            W, hasWChanged,
                                            historyOfBadAtks_global,
                                            historyOfGoodAtks_global,
                                            codeGenerationTime, historiesTimeConsump,
                                            historyOfBadAtks_semi_global,
                                            historyOfGoodAtks_semi_global);
        return (!isSuccess(result_def));
    }

    if (depth > 0) {
        if (OPT[THREE_VALUES_LOGIC] == ON_V1)
            result_def = defend_with_3VL_V1(aut, numb_states, initialState,
                                            firstStepOf(steps), q,
                                            isFinal, ranks, W, hasWChanged,
                                            historyOfBadAtks_global,
                                            historyOfGoodAtks_global,
                                            codeGenerationTime, historiesTimeConsump,
                                            historyOfBadAtks_semi_global,
                                            historyOfGoodAtks_semi_global);
        else if (OPT[THREE_VALUES_LOGIC] == ON_V2)
            result_def = defend_with_3VL_V2(aut, numb_states, initialState,
                                            firstStepOf(steps), q,
                                            isFinal, ranks, W, hasWChanged,
                                            historyOfBadAtks_global,
                                            historyOfGoodAtks_global,
                                            codeGenerationTime, historiesTimeConsump,
                                            historyOfBadAtks_semi_global,
                                            historyOfGoodAtks_semi_global);
        if (isSuccess(result_def))
            return false;
        else
            if ((OPT[THREE_VALUES_LOGIC] == ON_V1 || OPT[THREE_VALUES_LOGIC] == ON_V2) && isStrongFail((result_def)))
            {
                W.at(firstStepOf(steps).getState()).at(q) = false;
                hasWChanged = true;
                return true;
            }
    }

    vector<Step*> states;
    try{
        states = steps.at(depth);
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what();
        exit_with_error(" when acessing steps.");
    }

    transitions all_trans = mapGetTrans(aut, states);
    if (all_trans.size() == 0) {
        if (depth == 0)
            return false;
        else
            return !(isSuccess(result_def));
    }

    if (OPT[ORDER_OF_ATTACKS_TRANS] == SEMI_ORDERED_BY_INITIAL_TRANS)
    {
        // sort each vector of trans in all_trans so that the trans (if any) to the initial state come first.
        for (unsigned int i=0; i<all_trans.size(); i++)
        {
            vector<transition> vec = all_trans.at(i);
            all_trans.at(i) = moveInitialTransitionsToBeginning(vec);
        }
    }
    else if (OPT[ORDER_OF_ATTACKS_TRANS] == SEMI_ORDERED_BY_SYMBOLS_RANKING)
    {
        // sort each vector of trans in all_trans so that trans with by symbols with higher rank come first.
        for (unsigned int i=0; i<all_trans.size(); i++)
        {
            vector<transition> vec = all_trans.at(i);
            all_trans.at(i) = orderTransBySymbolsRankings(vec, ranks);
        }
    }

    vector<MaybeTransition> combination(all_trans.size());
    return attack_loop_with_3VL(aut, numb_states, initialState, isFinal, ranks,
                                la, W, hasWChanged, all_trans, 0, combination, steps,
                                q, depth, historyOfBadAtks_global,
                                historyOfGoodAtks_global, codeGenerationTime,
                                historiesTimeConsump);
}

/* The function 'attack_loop' calls the function 'attack' on each different
 * possibility of extending the current attack, but exits (with true) when one of those
 * calls returns true. The combinations of the transitions used in the
 * extension of the attack are generated not a priori but on-the-fly. */
bool attack_loop_with_3VL(const Automaton& aut, const unsigned int numb_states,
                          const state initialState, const vector<bool>& isFinal,
                          const vector<typerank>& ranks, const unsigned int la,
                          vector<vector<bool> >& W, bool& hasWChanged,
                          const vector<vector<transition> >& all_trans,
                          const unsigned int index,
                          vector<MaybeTransition>& combination,
                          vector<vector<Step*> > steps, const state q,
                          const unsigned int depth,
                          vector<vector<codes_map>>& historyOfBadAtks_global,
                          vector<vector<codes_map>>& historyOfGoodAtks_global,
                          float& codeGenerationTime, float& historiesTimeConsump)
{
    if (index == all_trans.size())
    {
        extendAttack(steps, depth, combination, ranks);
        return attack_with_3VL(aut, numb_states, initialState, steps, q, depth+1,
                               la, isFinal, ranks, W, hasWChanged,
                               historyOfBadAtks_global, historyOfGoodAtks_global,
                               codeGenerationTime, historiesTimeConsump);
    }
    else
    {
        if (all_trans.at(index).empty())
        {
            combination.at(index) = MaybeTransition();
            return attack_loop_with_3VL(aut, numb_states, initialState, isFinal,
                                        ranks, la, W, hasWChanged, all_trans,
                                        index+1, combination, steps, q, depth,
                                        historyOfBadAtks_global, historyOfGoodAtks_global,
                                        codeGenerationTime, historiesTimeConsump);
        }
        else {
            for (unsigned int i=0; i < all_trans.at(index).size(); i++) {
                combination.at(index) = MaybeTransition(all_trans.at(index).at(i));
                if (attack_loop_with_3VL(aut, numb_states, initialState, isFinal,
                                         ranks, la, W, hasWChanged,
                                         all_trans, index+1, combination, steps, q, depth,
                                         historyOfBadAtks_global,
                                         historyOfGoodAtks_global,
                                         codeGenerationTime, historiesTimeConsump))
                    return true;
                }
            }
        }

    return false;
}

float refine_with_3VL(const Automaton& aut, const state initialState,
                    const vector<bool>& isFinal, const vector<typerank>& ranks,
                    const unsigned int la, vector<vector<bool> >& W,
                    const unsigned int n,
                    vector<vector<codes_map>>& historyOfBadAtks_global,
                    vector<vector<codes_map>>& historyOfGoodAtks_global,
                    float& codeGenerationTime, float& historiesTimeConsump)
{
    unsigned int counter = 0, visits = 0;
    vector<vector<Step*> > steps;

    timespec startTime;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &startTime);

    try {
        while (true) {
            for (state p=0; p<n; p++)
                for (state q=0; q<n; q++) {
                    visits++;

                    timespec nowTime;
                    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &nowTime);
                    if (nowTime.tv_sec - startTime.tv_sec > TIMEOUT)
                    {
                        throw timeout_();
                    }

                    if (!W[p][q] || q==p) {
                        /* False remains False, and W always includes the id relation. */
                        if (++counter == n*n) {
                            throw finished();
                        }
                        else
                            continue;
                    }

                    Step first = Step(p,true);
                    vector<Step*> s(1);
                    s.at(0) = &first;
                    steps.resize(1);
                    steps.at(0) = s;

                    bool hasWChanged = false;
                    if (attack_with_3VL(aut, n, initialState, steps, q, 0, la,
                                        isFinal, ranks, W, hasWChanged,
                                        historyOfBadAtks_global, historyOfGoodAtks_global,
                                        codeGenerationTime, historiesTimeConsump))
                    {
                        W[p][q] = false;
                        counter = 0;
                        if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL_V2) {
                            auto start = std::chrono::high_resolution_clock::now();
                            for (vector<codes_map>& v : historyOfBadAtks_global)
                                for (codes_map& m : v)
                                    m.clear();
                            auto elapsed = std::chrono::high_resolution_clock::now() - start;
                            historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                        }
                    }
                    else if (hasWChanged)
                    {
                        counter = 0;
                        if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL_V2) {
                            auto start = std::chrono::high_resolution_clock::now();
                            for (vector<codes_map>& v : historyOfBadAtks_global)
                                for (codes_map& m : v)
                                    m.clear();
                            auto elapsed = std::chrono::high_resolution_clock::now() - start;
                            historiesTimeConsump += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                        }
                    }
                    else if (++counter == n*n)
                            throw finished();
                }
        }
    }
    catch (finished& e)
    {
    }

    return (float) visits / ((float) n*n);
}

/* Gets a vector that for each state 'p' and each symbol 's' contains the number of transitions
   in the automaton from 'p' and by 's'. */
vector<vector<unsigned int> > getPostLen(const Automaton& aut, const unsigned int numb_states, const unsigned int numb_symbols) {
    vector<vector<unsigned int> > post_len(numb_states);
    std::generate(post_len.begin(), post_len.end(), [&] { vector<unsigned int> vec_symbols(numb_symbols, 0); return vec_symbols; });

    for (const transition& trans : aut) {
        try {
            post_len.at(trans.GetParent()).at(trans.GetSymbol())++;
        }
        catch (const std::out_of_range& oor) {
            std::cerr << "Out of Range error: " << oor.what()
                      << "when trying to increment position [" << trans.GetParent()
                      << "][" << trans.GetSymbol() << "] of post_len.";
            exit_with_error("\n");
        }
    }

    return post_len;
}

vector<vector<bool> >& pre_refine(vector<vector<bool> >& W,
                                  const vector<vector<unsigned int> >& post_len,
                                  const unsigned int numb_states,
                                  const unsigned int numb_symbols,
                                  const state initialState)
{

    for (unsigned int i=0; i<numb_states; i++)
        for (unsigned int j=0; j<numb_states; j++)
            if (i==initialState && j!=initialState)
                W.at(i).at(j) = false;
            else
            {
                for (unsigned int s=0; s<numb_symbols && W.at(i).at(j); s++)
                    if (post_len.at(i).at(s) && !post_len.at(j).at(s))
                        W.at(i).at(j) = false;
            }

    return W;
}

float dw_simulation(const Automaton& aut, const unsigned int la,
                    vector<vector<bool> >& W, const unsigned int n,
                    const vector<typerank>& ranks, timespec* timeout_start, unsigned
                    int timeout)
{

    counter_localGAH      = QueriesCounter();
    counter_localBAH      = QueriesCounter();
    counter_semiGlobalGAH = QueriesCounter();
    counter_semiGlobalBAH = QueriesCounter();
    counter_globalGAH     = QueriesCounter();
    counter_globalBAH     = QueriesCounter();

    if (n==0)
        return 0;

    initializeW(aut, W, n);

    vector<bool> isFinal = getIsFinal(aut,n);
    const unsigned int m = getGreatestUsedSymbol(aut)+1;
    const state initialState = getInitialState(aut);

    // Pre-refinement
    vector<vector<unsigned int> > post_len = getPostLen(aut, n, m);
    W = pre_refine(W, post_len, n, m, initialState);

    // Define the (global) histories of attacks
    std::set<code> set;
    vector<std::set<code>> column(n,set);
    vector<vector<std::set<code>> > historyOfGoodAtks_global(n,column);
    codes_map map;
    vector<codes_map> column_3VL(n,map);
    vector<vector<codes_map>> historyOfGoodAtks_global_3VL(n,column_3VL);
    std::set<code> steps_set;
    vector<std::set<code>> steps_column(n,steps_set);
    vector<vector<std::set<code> >> historyOfBadAtks_global(n,steps_column);
    vector<vector<codes_map>> historyOfBadAtks_global_3VL = historyOfGoodAtks_global_3VL;

    float codeGenerationTime = 0, historiesTimeConsump = 0;

    float answer;
    if (OPT[THREE_VALUES_LOGIC] == ON_V1 || OPT[THREE_VALUES_LOGIC] == ON_V2)
        answer = refine_with_3VL(aut, initialState, isFinal, ranks, la, W, n,
                                 historyOfBadAtks_global_3VL,
                                 historyOfGoodAtks_global_3VL,
                                 codeGenerationTime, historiesTimeConsump);
    else
        answer = refine(aut, initialState, isFinal, ranks, la, W, n,
                        historyOfBadAtks_global, historyOfGoodAtks_global,
                        codeGenerationTime, historiesTimeConsump, timeout_start,
                        timeout);


    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS]==GLOBAL    || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS]==GLOBAL
     || OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS]==GLOBAL_V2 || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS]==GLOBAL_V2 )
    {
        std::cout << "The code generation for the history structures took in total " << codeGenerationTime   / (float) 1000000 << " sec.\n";
        std::cout << "The inserts/lookups/clearings in the history structures took in total " << historiesTimeConsump / (float) 1000000 << " sec.\n";
    }

    return answer;
}

float dw_simulation_strict(const Automaton& aut, const unsigned int la,
                           vector<vector<bool> >& W, const unsigned int n,
                           const vector<typerank>& ranks)
{
    float refin = dw_simulation(aut, la, W, n, ranks);

    extractStrictRelation(W, n);
    return refin;
}

/* Calls libvata's algorithm for computing the ordinary dw simulation on the
   given automaton, and converts the relation structure to a vector of vector of bools. */
void dw_ord_simulation_lvata(const Automaton& aut, vector<vector<bool> >& W, const unsigned int numb_states)
{
    stateDiscontBinaryRelation lv_sim = computeLVDwSimulation(aut, numb_states);
    initializeW(aut, W, numb_states);
    convertBinaryRelToBoolMatrix(aut, lv_sim, W);
}

void dw_ord_simulation_lvata_strict(const Automaton& aut, vector<vector<bool> >& W, const unsigned int numb_states)
{
    dw_ord_simulation_lvata(aut, W, numb_states);
    extractStrictRelation(W, numb_states);
}

