
#include "up_simulation.hh"


bool defend(Step& step, const state q, vector<vector<pair<transition,size_t>> >& trans_botup, const bool* isFinal,
            const vector<typerank> ranks, const vector<vector<bool> >& param_rel, const bool strict, const vector<vector<bool> >& W) {
    state p = step.getState();

    if (isFinal[p] && !isFinal[q])
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
        if (defend(parent,(pair_.first).GetParent(),trans_botup,isFinal,ranks,param_rel,strict,W))
            return true;
    }

    return false;
}

bool attack(Step* leafStep, Step& rootStep, const state q, const unsigned int depth, const unsigned int la, vector<vector<pair<transition,size_t>> >& trans_botup, const bool* isFinal, const vector<typerank> ranks, const vector<vector<bool> >& param_rel, const bool strict, const vector<vector<bool> >& W) {
    if (depth==la)
        return !defend(*leafStep,q,trans_botup,isFinal,ranks,param_rel,strict,W);

    bool result_def;
    if (depth>0) {
        result_def = defend(*leafStep,q,trans_botup,isFinal,ranks,param_rel,strict,W);
        if (result_def) return false;
    }

    vector<pair<transition,size_t>> trans_from_p = vectorVectorPairTransitionIntAt(trans_botup,rootStep.getState());
    if (trans_from_p.empty()) {
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
    for (unsigned int i=0; i<size; i++) {
        trans = trans_from_p.at(i).first;
        Step parent = Step(trans.GetParent(), trans.GetSymbol(), ranks[trans.GetSymbol()], trans.GetChildren(), false);
        rootStep.setParent(&parent);
        rootStep.setIndex(trans_from_p.at(i).second);
        parent.setChild(rootStep, trans_from_p.at(i).second);

        if (attack(leafStep,parent,q,depth+1,la,trans_botup,isFinal,ranks,param_rel,strict,W))
            return true;
    }

    return false;
}

/*pair<vector<vector<bool> >,*/ float/*>*/ refine(const bool* isFinal, const vector<typerank> ranks, vector<vector<pair<transition,size_t>> >& trans_botup, const unsigned int la, const vector<vector<bool> >& param_rel, const bool strict, vector<vector<bool> >& W, const unsigned int n) {
    unsigned int counter = 0, visits = 0;

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

                    if (!W[p][q] || q==p) {     // False remains False; and W always includes the id relation.
                        if (++counter == n*n) {
                            throw finished();
                        }
                        else
                            continue;
                    }

                    Step first = Step(p,true);
                    if (attack(&first, first, q, 0, la, trans_botup, isFinal, ranks, param_rel, strict, W)) {
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

    return /*std::make_pair(W,*/(float) visits / ((float) n*n)/*)*/;
}


/* Returns a 3D vector post_len such that post_len[p][s][i] contains the number of transitions
   (bottom-up) by s which have p as the i-th child. */
vector<vector<vector<unsigned int> > > getPostLen(const vector<vector<pair<transition,size_t>> >& trans_botup, const unsigned int numb_states, const unsigned int numb_symbols, const vector<typerank>& ranks) {
    setGreatestRank(ranks);
    vector<unsigned int> vec_indexes(GREATEST_RANK,0);
    vector<vector<unsigned int> > vec_symbols(numb_symbols, vec_indexes);
    vector<vector<vector<unsigned int> > > post_len(numb_states, vec_symbols);

    unsigned int size = trans_botup.size();
    for (unsigned int state=0; state<size; state++) {
        const vector<pair<transition,size_t> >& trans_to_state = trans_botup.at(state);     // trans that have 'state' has one of the children
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

vector<vector<bool> >& pre_refine(vector<vector<bool> >& W, const vector<vector<vector<unsigned int> > >& post_len, const unsigned int numb_states, const unsigned int numb_symbols) {

    // might be easy to adapt to depths greater than 1
    for (unsigned int p=0; p<numb_states; p++)
        for (unsigned int q=0; q<numb_states; q++)
            for (unsigned int s=0; s<numb_symbols && W.at(p).at(q); s++)
                for (unsigned int i=0; i<GREATEST_RANK && W.at(p).at(q); i++)
                    if (post_len.at(p).at(s).at(i) && !post_len.at(q).at(s).at(i))
                        W.at(p).at(q) = false;

    return W;
}


float up_simulation(const Automaton& aut, const unsigned int la,
                    const vector<vector<bool> >& param_rel, const bool param_strict,
                    vector<vector<bool> >& W, const unsigned int n,
                    const unsigned int numb_symbols, const vector<typerank>& ranks) {
    if (n==0)
        return 0;

    initializeW(aut, W, n);
    bool* isFinal = getIsFinal2(aut,n);
    //const unsigned int m = getNumbSymbols(aut);
    //vector<typerank> ranks = getRanks(aut,numb_symbols);

    auto start = std::chrono::high_resolution_clock::now();
    vector<vector<pair<transition,size_t>> > trans_botup = obtainTransBotUp(aut,n);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    //float time_sec =
      //      std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    //std::cout << "The computation of the bottom-up transitions structure took "
        //      << time_sec << " seconds. ";

    // Pre-refinement
    start = std::chrono::high_resolution_clock::now();
    vector<vector<vector<unsigned int> > > post_len = getPostLen(trans_botup, n, numb_symbols, ranks);
    W = pre_refine(W, post_len, n, numb_symbols);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    //time_sec =
      //      std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    //std::cout << "The Pre-Refinement took " << time_sec << " seconds. ";

    /*pair<vector<vector<bool> >,*/ float/*>*/ refin = refine(isFinal, ranks, trans_botup, la, param_rel, param_strict, W, n);

    delete[] isFinal;
    //delete[] ranks;

    return refin;
}

float up_simulation_strict(const Automaton& aut, const unsigned int la, const vector<vector<bool> >& param_rel, const bool strict, vector<vector<bool> >& W, const unsigned int n, const unsigned int numb_symbols, const vector<typerank>& ranks) {
    float refin = up_simulation(aut, la, param_rel, strict, W, n, numb_symbols, ranks);

    extractStrictRelation(W, n);

    return refin;
}
