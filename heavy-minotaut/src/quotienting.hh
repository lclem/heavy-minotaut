
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file declaring auxiliary functions that call libvata's states        *
 *  quotienting procedure.                                                          *
 * 																					*
 ************************************************************************************/

#ifndef QUOTIENTING_HH
#define QUOTIENTING_HH


#include "Common/automatonHelper.hh"

using std::vector;

using statesMap = VATA::ExplicitTreeAut::StateToStateMap;

statesMap convertPreorderToBinEquiv(const Automaton &aut, const vector<vector<bool> >& P);
AutData quotientAutomaton(const AutData &old_autData, const vector<vector<bool> >& W);


#endif // QUOTIENTING_HH
