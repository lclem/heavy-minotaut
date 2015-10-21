
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file declaring a collection of functions that perform tests          *
 *  to minimization techniques based on the computation of the lookahead downward   *
 *  simulation algorithm. At the end of each minimization, a consitency check is    *
 *  performed (a check that the language has been preserved).                       *
 * 																					*
 ************************************************************************************/

#ifndef DW_SIMULATION_TESTER_H
#define DW_SIMULATION_TESTER_H


#include "common.hh"
#include "../src/dw_simulation.hh"
#include "../src/quotienting.hh"
#include "../src/pruning.hh"
#include "../src/Common/statisticalResults.hh"

void dw_simulation_tester();


#endif // DW_SIMULATION_TESTER_H
