/************************************************************************************
 *					(insert a nice name for the library here)						*
 *  - a library for minimizing tree automata based on lookahead simulations -		*
 * 																					*
 * 		Copyright (c) 2014	Ricardo Almeida	(LFCS - University of Edinburgh)		*
 * 																					*
 *	Description: This module behaves as a bridge between this project and libvata,  *
 *  providing functions to quotient tree automata. Its main functions are:          *
 *   - convertPreorderToBinEquiv: converts a states preorder (represented as a      *
 *  boolean matrix) into an equivalence binary relation with libvata's type         *
 *  statesMap;                                                                      *
 *   - quotientAutomaton: calls convertPreorderToBinEquiv to obtain a states map    *
 *  from the given preorder, and then calls libvata's function for quotienting      *
 *  automata with that map.                                                         *
 *                                                                                  *
 ************************************************************************************/

#ifndef QUOTIENTING_HH
#define QUOTIENTING_HH


#include "Common/automatonHelper.hh"
//#include "Common/util.hh"

using std::vector;

using statesMap = VATA::ExplicitTreeAut::StateToStateMap;

statesMap convertPreorderToBinEquiv(const Automaton &aut, const vector<vector<bool> >& P);
Automaton quotientAutomaton(const Automaton & old_aut, const vector<vector<bool> >& W, const unsigned int n);


#endif // QUOTIENTING_HH
