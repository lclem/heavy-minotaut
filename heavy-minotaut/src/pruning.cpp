
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file for the transitions pruning procedure.                  *
 * 																					*
 ************************************************************************************/

#include "pruning.hh"

/* Departing states are compared using relation rel1 and children states are compared
 * using rel2 (which may be strict or not, depending on the flag given). */
AutData prune(const AutData& autData, const vector<vector<bool> >& rel1,
                const vector<vector<bool> >& rel2, const bool flag_strictness)
{

    AutData autData_p = copyAutWithoutTrans(autData);
    Automaton aut_p = getAut(autData_p), aut = getAut(autData);

    state parent1;
    symbol symb;
    vector<state> children1, children2;

    bool bad;
    for (const lv_transition& trans1 : aut)
    {
        bad = false;
        parent1 = trans1.GetParent();
        symb = trans1.GetSymbol();
        children1 = trans1.GetChildren();

        for (const lv_transition& trans2 : aut)
            if (symb == trans2.GetSymbol())
            {
                if (areInRel(parent1,trans2.GetParent(),rel1))
                {
                    children2 = trans2.GetChildren();
                    if (areInRelIter(children1, children2, rel2, flag_strictness))
                    {
                        /* Then trans2 is "better" than trans1, and so the latter will not
                        *  be in the pruned automaton. */
                        bad = true;
                        break;
                    }
                }
            }
        if (!bad)
            aut_p.AddTransition(children1,symb,parent1);
    }

    return wrapAutData(aut_p,getRanks(autData));

}
