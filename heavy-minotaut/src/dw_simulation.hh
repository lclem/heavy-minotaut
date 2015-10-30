
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
extern unsigned int pr_depth;

bool attack_loop(const Automaton& aut, const unsigned int numb_states, const state initialState,
                 const vector<typerank>& ranks, const unsigned int la,
                 const vector<vector<bool> >& W, const vector<vector<transition> >& all_trans,
                 const unsigned int index,
                 vector<MaybeTransition>& combination, vector<vector<Step*> > steps, const state q,
                 const unsigned int depth, vector<vector<set<code> > >& historyOfBadAtks,
                 vector<vector<set<code> >>& historyOfGoodAtks,
                 float& codeGenerationTime, float& historiesTimeConsump, timespec *timestart_timeout, unsigned int timeout);

bool attack_loop_with_3VL(const Automaton& aut, const unsigned int numb_states, const state initialState, const vector<bool>& isFinal,
                 const vector<typerank>& ranks, const unsigned int la,
                 vector<vector<bool> >& W, bool &hasWChanged, const vector<vector<transition> >& all_trans,
                 const unsigned int index,
                 vector<MaybeTransition>& combination, vector<vector<Step*> > steps, const state q,
                 const unsigned int depth, vector<vector<codes_map > >& historyOfBadAtks,
                 vector<vector<codes_map >>& historyOfGoodAtks,
                 float& codeGenerationTime, float& historiesTimeConsump);

bool attack_loop_pr(const Automaton& aut, const unsigned int numb_states,
                 const state initialState,
                 const vector<typerank>& ranks, const unsigned int la,
                 const vector<vector<transition> >& all_trans, const unsigned int index,
                 vector<MaybeTransition>& combination, vector<vector<Step*> > steps, const state q, const unsigned int depth,
                 unsigned int &codeGenerationTime, unsigned int &historiesTimeConsump, unsigned int &counter_normalAtks, unsigned int &counter_goodAtks, unsigned int &counter_badAtks,
                 timespec* timestart_timeout, unsigned int timeout = 0);

vector<vector<bool> > dw_simulation(const AutData &autData, const unsigned int la,
                                    float *refinements = NULL, bool use_lvata = true,
                                    timespec *timeout_start = NULL, unsigned int timeout = 0);

void pre_refine_lin(vector<vector<bool> >& W, const Automaton& aut,
                    const unsigned int depth,
                    const unsigned int numb_states,
                    const unsigned int numb_symbols,
                    const vector<typerank>& ranks,
                    const state initialState);
void pre_refine_branch(vector<vector<bool> >& W, const Automaton& aut,
                        const unsigned int depth,
                        const unsigned int numb_states,
                        const vector<typerank>& ranks,
                        const state initialState,
                        timespec* timestart_timeout = NULL, unsigned int timeout = 0);

#endif
