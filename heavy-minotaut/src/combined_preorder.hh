#ifndef COMBINED_PREORDER_H
#define COMBINED_PREORDER_H

#include "Common/exceptions.hh"
#include "Common/automatonHelper.hh"
#include "Common/simulationHelper.hh"
#include "dw_simulation.hh"
#include "up_simulation.hh"

float combined_preorder(const Automaton& aut, vector<vector<bool> >& W_combined,
                        const unsigned int la_dw, const unsigned int la_up,
                        const unsigned int numb_states, const unsigned int numb_symbols, const vector<typerank>& ranks,
                        timespec* timeout_start, unsigned int timeout);


#endif // COMBINED_PREORDER_H
