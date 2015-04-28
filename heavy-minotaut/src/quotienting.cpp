
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file with auxiliary functions that call libvata's states     *
 *  quotienting procedure.                                                          *
 * 																					*
 ************************************************************************************/

#include "quotienting.hh"

/* Converts a states preorder (represented as a nxn boolean matrix) into an equivalence binary relation
 * with libvata's type;                      */
statesMap convertPreorderToBinEquiv(const Automaton& aut, unsigned int n, const vector<vector<bool> >& P) {
    statesMap map;
    state initialState = getInitialState(aut);

    for (unsigned int i = 0; i < n && i != initialState; i++) {
        for (unsigned int j = 0; j <= i; j++) {
            bool result1 = false, result2 = false;
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

    return map;
}

/* Calls convertPreorderToBinEquiv to obtain a states map from the given preorder,
 * and then calls libvata's function for quotienting automata with that map. */
Automaton quotientAutomaton(const Automaton& old_aut, const vector<vector<bool> >& W,
                            const unsigned int n) {
    if (n==0) return old_aut;

    statesMap map = convertPreorderToBinEquiv(old_aut,n,W);

    Automaton aut;
    aut = removeInitialState(old_aut);
    aut = aut.CollapseStates(map);
    aut = addInitialState(aut);

    return aut;
}
