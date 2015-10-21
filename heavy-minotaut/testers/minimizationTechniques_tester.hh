
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file defining a collection of functions that perform tests   *
 *  to minimization techniques based on the computation of both lookahead downward  *
 *  simulation and lookahead upward simulation. At the end of each minimization, a  *
 *  consistency check is performed (a check that the language has been preserved).  *
 * 																					*
 ************************************************************************************/

#ifndef MINIMIZATIONTECHNIQUES_TESTER_H
#define MINIMIZATIONTECHNIQUES_TESTER_H

#include "../src/minimizationTechniques.hh"

#include "common.hh"
#include "../src/dw_simulation.hh"
#include "../src/up_simulation.hh"
#include "../src/quotienting.hh"
#include "../src/pruning.hh"
#include "../src/Common/statisticalResults.hh"

void minimizationTechniques_tester();
void simple_Heavy_tester(unsigned int sequence_version, unsigned int la_dw, unsigned int la_up, string input_dir, string output_dir);

#endif // MINIMIZATIONTECHNIQUES_TESTER_H
