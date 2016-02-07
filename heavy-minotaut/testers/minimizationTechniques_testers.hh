
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file defining a collection of functions that perform tests   *
 *  to minimization techniques based on the computation of both lookahead downward  *
 *  simulation and lookahead upward simulation.                                     *
 * 																					*
 ************************************************************************************/

#ifndef minimizationTechniques_testers_H
#define minimizationTechniques_testers_H


#include "../src/minimizationTechniques.hh"

#include "common.hh"
#include "../src/dw_simulation.hh"
#include "../src/up_simulation.hh"
#include "../src/quotienting.hh"
#include "../src/pruning.hh"
#include "../src/Common/statisticalResults.hh"

void minimizationTechniques_testers();
void simple_Heavy_tester(unsigned int la_dw, unsigned int la_up, string input_dir, string output_dir);
void call_heavy_tester(unsigned int la_dw, unsigned int la_up, string dir, string log="");
void heavy_tester(unsigned int la_dw, unsigned int la_up, string input_dir,
                  string output_dir = "", unsigned int numb_tests = 0, string log_avgs = "");
void heavy_tester(unsigned int la_dw, unsigned int la_up, vector<string> input_dirs,
                  string output_dir = "", unsigned int numb_tests = 0, string log_avgs_ = "");


#endif // minimizationTechniques_testers_H
