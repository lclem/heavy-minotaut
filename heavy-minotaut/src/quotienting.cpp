
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

/* Converts a states preorder (represented as a boolean matrix) into an equivalence binary relation
 * with libvata's type;                      */
statesMap convertPreorderToBinEquiv(const Automaton& aut, const vector<vector<bool> >& P)
{
    unsigned int n = P.size();
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
AutData quotientAutomaton(const AutData& old_autData, const vector<vector<bool> >& W)
{

    if (W.size()==0) return old_autData;

    Automaton old_aut = getAut(old_autData);
    statesMap map = convertPreorderToBinEquiv(old_aut,W);

    Automaton aut = removeInitialState(old_aut);
    aut = aut.CollapseStates(map);
    aut = addInitialState(aut);

    AutData new_autData = wrapAutData(aut,getRanks(old_autData));

    return new_autData;

}
