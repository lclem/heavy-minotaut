
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file for the computation of the lookahead upward simulation  *
 *  (strict or non-strict).                                                         *
 * 																					*
 ************************************************************************************/

#include "up_simulation.hh"

bool defend(const Automaton& aut, Step& step, const state q, vector<vector<pair<transition,size_t>> >& trans_botup,
            const bool* isFinal, const vector<typerank> ranks,
            const vector<vector<bool> >& param_rel, const bool strict,
            const vector<vector<bool> >& W,
            timespec* timestart_timeout, unsigned int timeout = 0)
{

    if (timestart_timeout != NULL) check_timeout(aut, *timestart_timeout, timeout);

    state p = step.getState();

    if (OPT[WEAK_ACCEPTANCE_COND]==OFF && isFinal[p] && !isFinal[q])
        return false;

    if (OPT[WEAK_ACCEPTANCE_COND]==ON && (step.isLastStep() || step.isFirstStep()) && isFinal[p] && !isFinal[q])
        return false;

    if (!step.isFirstStep() && W[p][q])
        return true;

    if(step.isLastStep())
        return W[p][q];

    Step parent = step.getParent();
    symbol s = parent.getSymbol();
    unsigned int index = step.getIndex();

    vector<pair<transition, size_t> > transitions = vectorVectorPairTransitionIntAt(trans_botup,q);

    unsigned int size = transitions.size();

    vector<state> children;
    pair<transition,unsigned int> pair_;
    for (unsigned int k=0; k<size; k++) {
        pair_ = transitions.at(k);
        if (pair_.second!=index || (pair_.first).GetSymbol() != s)
            continue;
        children = (pair_.first).GetChildren();
        children.erase(children.begin()+index);   // the siblings of q
        vector<state> children2 = mapGetState(parent.getChildren());
        children2.erase(children2.begin()+index); // the siblings of p
        if (!areInRelIter(children2, children, param_rel, strict))
            continue;
        if (defend(aut,parent,(pair_.first).GetParent(),trans_botup,isFinal,ranks,
                   param_rel,strict,W,timestart_timeout, timeout))
            return true;
    }

    return false;
}

bool attack(const Automaton& aut, Step* leafStep, Step& rootStep, const state q,
            const unsigned int depth, const unsigned int la,
            vector<vector<pair<transition,size_t>> >& trans_botup, const bool* isFinal, const vector<typerank> ranks,
            const vector<vector<bool> >& param_rel, const bool strict, const vector<vector<bool> >& W,
            timespec* timestart_timeout, unsigned int timeout = 0)
{

    if (timestart_timeout != NULL) check_timeout(aut, *timestart_timeout, timeout);

    if (depth==la)
        return !defend(aut,*leafStep,q,trans_botup,isFinal,ranks,param_rel,strict,W,timestart_timeout, timeout);

    bool result_def;
    if (depth>0)
    {
        result_def = defend(aut,*leafStep,q,trans_botup,isFinal,ranks,param_rel,strict,W,timestart_timeout, timeout);
        if (result_def) return false;
    }

    vector<pair<transition,size_t>> trans_from_p = vectorVectorPairTransitionIntAt(trans_botup,rootStep.getState());
    if (trans_from_p.empty())
    {
        if (depth==0)       // in this case, leafStep and rootStep refer to the same.
        {
            if (isFinal[leafStep->getState()] && !isFinal[q])
                return true;
            else
                return false;
        }
        else
            return !result_def;
    }

    unsigned int size = trans_from_p.size();
    transition trans;
    for (unsigned int i=0; i<size; i++)
    {
        trans = trans_from_p.at(i).first;
        Step parent = Step(trans.GetParent(), trans.GetSymbol(), ranks[trans.GetSymbol()], trans.GetChildren(), false);
        rootStep.setParent(&parent);
        rootStep.setIndex(trans_from_p.at(i).second);
        parent.setChild(rootStep, trans_from_p.at(i).second);

        if (attack(aut, leafStep,parent,q,depth+1,la,trans_botup,isFinal,ranks,param_rel,strict,W, timestart_timeout, timeout))
            return true;
    }

    return false;
}

float refine(const Automaton& aut, const bool* isFinal, const vector<typerank> ranks,
             vector<vector<pair<transition,size_t>> >& trans_botup,
             const unsigned int la, const vector<vector<bool> >& param_rel,
             const bool strict, vector<vector<bool> >& W, const unsigned int n,
             timespec* timestart_timeout = NULL, unsigned int timeout = 0)
{
    unsigned int counter = 0, visits = 0;

    if (timestart_timeout == NULL)
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, timestart_timeout);

    try
    {
        while (true)
        {
            for (state p=0; p<n; p++)
                for (state q=0; q<n; q++)
                {
                    visits++;

                    if (!W[p][q] || q==p) {     // False remains False; and W always includes the id relation.
                        if (++counter == n*n) {
                            throw finished();
                        }
                        else
                            continue;
                    }

                    Step first = Step(p,true);
                    if (attack(aut, &first, first, q, 0, la, trans_botup, isFinal, ranks,
                               param_rel, strict, W, timestart_timeout, timeout)) {
                        W[p][q] = false;
                        counter = 0;
                    }
                    else
                        if (++counter == n*n) 	throw finished();
                }
        }
    }
    catch (finished& e)
    {
    }

    return (float) visits / ((float) n*n);
}


/* Returns a 3D vector post_len such that post_len[p][s][i] contains the number of transitions
   (bottom-up) by s which have p as the i-th child. */
vector<vector<vector<unsigned int> > > getPostLen(const vector<vector<pair<transition,size_t>> >& trans_botup,
                                                  const unsigned int numb_states, const unsigned int numb_symbols,
                                                  const vector<typerank>& ranks)
{
    setGreatestRank(ranks);
    vector<unsigned int> vec_indexes(GREATEST_RANK,0);
    vector<vector<unsigned int> > vec_symbols(numb_symbols, vec_indexes);
    vector<vector<vector<unsigned int> > > post_len(numb_states, vec_symbols);

    unsigned int size = trans_botup.size();
    for (unsigned int state=0; state<size; state++) {
        const vector<pair<transition,size_t> >& trans_to_state = trans_botup.at(state);
        // trans that have 'state' as one of the children
        for (const pair<transition,size_t> pair_ : trans_to_state) {
            try {
                post_len.at(state).at(pair_.first.GetSymbol()).at(pair_.second)++;
            }
            catch (const std::out_of_range& oor) {
                std::cerr << "Out of Range error: " << oor.what()
                          << "when trying to increment the position [" << state
                          << "][" << pair_.first.GetSymbol() << "][" << pair_.second << "] of post_len.";
                exit_with_error("\n");
            }
        }
    }

    return post_len;
}

vector<vector<bool> >& pre_refine(vector<vector<bool> >& W, const Automaton& aut,
                                  const vector<vector<vector<unsigned int> > >& post_len,
                                  const unsigned int numb_states, const unsigned int numb_symbols,
                                  timespec* timestart_timeout = NULL, unsigned int timeout = 0)
{

    // might be easy to adapt to depths greater than 1
    for (unsigned int p=0; p<numb_states; p++)
        for (unsigned int q=0; q<numb_states; q++)
        {
            if (timestart_timeout != NULL) check_timeout(aut, *timestart_timeout, timeout);

            for (unsigned int s=0; s<numb_symbols && W.at(p).at(q); s++)
                for (unsigned int i=0; i<GREATEST_RANK && W.at(p).at(q); i++)
                    if (post_len.at(p).at(s).at(i) && !post_len.at(q).at(s).at(i))
                        W.at(p).at(q) = false;
        }

    return W;
}


float up_simulation(const Automaton& aut, const unsigned int la,
                    const vector<vector<bool> >& param_rel, const bool param_strict,
                    vector<vector<bool> >& W, const unsigned int n,
                    const unsigned int numb_symbols, const vector<typerank>& ranks,
                    timespec* timeout_start, unsigned int timeout)
{
    if (n==0)
        return 0;

    initializeW(aut, W, n);
    bool* isFinal = getIsFinal2(aut,n);

    auto start = std::chrono::high_resolution_clock::now();
    vector<vector<pair<transition,size_t>> > trans_botup = obtainTransBotUp(aut,n);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    // Pre-refinement
    start = std::chrono::high_resolution_clock::now();
    vector<vector<vector<unsigned int> > > post_len = getPostLen(trans_botup, n, numb_symbols, ranks);
    W = pre_refine(W, aut, post_len, n, numb_symbols, timeout_start);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    float refin = refine(aut, isFinal, ranks, trans_botup, la, param_rel, param_strict,
                         W, n, timeout_start, timeout);

    delete[] isFinal;

    return refin;
}

float up_simulation_strict(const Automaton& aut, const unsigned int la, const vector<vector<bool> >& param_rel,
                           const bool strict, vector<vector<bool> >& W, const unsigned int n,
                           const unsigned int numb_symbols, const vector<typerank>& ranks,
                           timespec* timeout_start, unsigned int timeout) {
    float refin = up_simulation(aut, la, param_rel, strict, W, n, numb_symbols, ranks, timeout_start, timeout);

    extractStrictRelation(W, n);

    return refin;
}
