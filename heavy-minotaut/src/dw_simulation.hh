
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file for the computation of the lookahead downward simulation        *
 *  (strict or non-strict). It also declares functions which call libvata's         *
 *  ordinary simulation algorithm.                                                  *
 * 																					*
 ************************************************************************************/

#ifndef _DW_SIMULATION_HH_
#define _DW_SIMULATION_HH_

#include "Common/exceptions.hh"
#include "Common/automatonHelper.hh"
#include "Common/simulationHelper.hh"
#include "step.hh"
#include "Common/statisticalResults.hh"
#include "Common/executionOptions.hh"

extern bool OPT_RESTART_REFINE_3;
extern string filename_Gl;

bool attack_loop(const Automaton& aut, const unsigned int numb_states, const state initialState, const vector<bool>& isFinal,
                 const vector<typerank>& ranks, const unsigned int la,
                 const vector<vector<bool> >& W, const vector<vector<transition> >& all_trans,
                 const unsigned int index,
                 vector<MaybeTransition>& combination, vector<vector<Step*> > steps, const state q,
                 const unsigned int depth, vector<vector<set<code> > >& historyOfBadAtks,
                 vector<vector<set<code> >>& historyOfGoodAtks,
                 float& codeGenerationTime, float& historiesTimeConsump);

bool attack_loop_with_3VL(const Automaton& aut, const unsigned int numb_states, const state initialState, const vector<bool>& isFinal,
                 const vector<typerank>& ranks, const unsigned int la,
                 vector<vector<bool> >& W, bool &hasWChanged, const vector<vector<transition> >& all_trans,
                 const unsigned int index,
                 vector<MaybeTransition>& combination, vector<vector<Step*> > steps, const state q,
                 const unsigned int depth, vector<vector<codes_map > >& historyOfBadAtks,
                 vector<vector<codes_map >>& historyOfGoodAtks,
                 float& codeGenerationTime, float& historiesTimeConsump);

float dw_simulation(const Automaton&, const unsigned int, vector<vector<bool> >&,
                    const unsigned int, const vector<typerank> &ranks,
                    timespec *timeout_start = NULL, unsigned int timeout = 0);

void dw_ord_simulation_lvata(const Automaton& aut, vector<vector<bool> >& W, const unsigned int numb_states);

void dw_ord_simulation_lvata_strict(const Automaton& aut, vector<vector<bool> >& W,
                                    const unsigned int numb_states);

#endif
