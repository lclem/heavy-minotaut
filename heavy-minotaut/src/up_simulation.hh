
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file for the computation of the lookahead upward simulation (strict  *
 *  or non-strict).                                                                 *
 * 																					*
 ************************************************************************************/

#ifndef _UP_SIMULATION_HH_
#define _UP_SIMULATION_HH_

#include "Common/exceptions.hh"
#include "Common/automatonHelper.hh"
#include "Common/simulationHelper.hh"
#include "step.hh"

//#include <vata/explicit_tree_aut.hh>

extern bool OPT_RESTART_REFINE_3;

float up_simulation(const Automaton& aut, const unsigned int la, const vector<vector<bool> >& param_rel, const bool strict, vector<vector<bool> >& W, const unsigned int n, const unsigned int numb_symbols, const vector<typerank> &ranks);
float up_simulation_strict(const Automaton& aut, const unsigned int la, const vector<vector<bool> >& param_rel, const bool strict, vector<vector<bool> >& W, const unsigned int n, const unsigned int numb_symbols, const vector<typerank> &ranks);

#endif
