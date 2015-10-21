
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file for the transitions pruning procedure.                          *
 * 																					*
 ************************************************************************************/

#ifndef PRUNING_H
#define PRUNING_H


#include "Common/automatonHelper.hh"
#include "Common/simulationHelper.hh"

using std::vector;

Automaton prune(Automaton& aut, const vector<vector<bool> >& rel1,
                const vector<vector<bool> >& rel2, const bool flag_strictness);


#endif // PRUNING_H
