#ifndef PRUNING_H
#define PRUNING_H


#include "Common/automatonHelper.hh"
//#include "Common/util.hh"
#include "Common/simulationHelper.hh"

using std::vector;

Automaton prune(Automaton& aut, const vector<vector<bool> >& rel1, const vector<vector<bool> >& rel2, const bool flag_strictness);


#endif // PRUNING_H
