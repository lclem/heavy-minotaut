
#include "pruning.hh"

Automaton prune(Automaton& aut, const vector<vector<bool> >& rel1, const vector<vector<bool> >& rel2, const bool flag_strictness) {
    Automaton aut_p = copyAutWithoutTrans(aut);

    state parent1;
    symbol symb;
    vector<state> children1, children2;

    bool bad;
    for (const lv_transition& trans1 : aut) {
        bad = false;
        parent1 = trans1.GetParent();
        symb = trans1.GetSymbol();
        children1 = trans1.GetChildren();
        /*if (children1.empty())
            children1 = {initialState};*/

        for (const lv_transition& trans2 : aut)
            if (symb == trans2.GetSymbol()) {
                if (areInRel(parent1,trans2.GetParent(),rel1)) {
                    children2 = trans2.GetChildren();
                    /*if (children2.empty())
                        children2 = {initialState}; */
                    if (areInRelIter(children1, children2, rel2, flag_strictness)) {
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

    return aut_p;
}
