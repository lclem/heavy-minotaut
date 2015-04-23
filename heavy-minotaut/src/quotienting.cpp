
#include "quotienting.hh"

/* P is a boolean matrix nxn */
statesMap convertPreorderToBinEquiv(const Automaton& aut, unsigned int n, const vector<vector<bool> >& P) {
    statesMap map;
    state initialState = getInitialState(aut);
    //unsigned int n = getNumbUsedStates(aut);

    /*if (USING_INITIAL_STATE)
        n--;    // so that the loop below always respects i!=INITIAL_STATE.*/

    for (unsigned int i = 0; i < n && i != initialState; i++) {
        for (unsigned int j = 0; j <= i; j++) {
            bool result1, result2;
            try
            {
                result1 = P.at(i).at(j);
            }
            catch (std::out_of_range& e)
            {
                std::cout << "out_of_range error when trying to access position (" << i << "," << j << ") of P";
                exit_with_error("");
            }
            try
            {
                result2 = P.at(j).at(i);
            }
            catch (std::out_of_range& e)
            {
                std::cout << "out_of_range error when trying to access position (" << j << "," << i << ") of P";
                exit_with_error("");
            }

            if (result1 && result2)
            {
                map.insert(std::make_pair(i, j));
            }
        }
    }

    /*if (USING_INITIAL_STATE)*/
    //    map.insert(std::make_pair(initialState, initialState));

    return map;
}

Automaton quotientAutomaton(const Automaton& old_aut, const vector<vector<bool> >& W,
                            const unsigned int n) {
    if (n==0) return old_aut;

    statesMap map = convertPreorderToBinEquiv(old_aut,n,W);

    Automaton aut;
    aut = removeInitialState(old_aut);
    aut = aut.CollapseStates(map);
    aut = addInitialState(aut);

    return aut/*_collapsed*/;
}
