#ifndef COMBINED_PREORDER_H
#define COMBINED_PREORDER_H

#include "Common/exceptions.hh"
#include "Common/automatonHelper.hh"
#include "Common/simulationHelper.hh"
#include "dw_simulation.hh"
#include "up_simulation.hh"

vector<vector<bool> > combined_relation(const AutData &autData, /*vector<vector<bool> >& W_combined,*/
                      const unsigned int la_dw, const unsigned int la_up,
                      const unsigned int numb_states, const unsigned int greatest_symbol,
                      timespec* timeout_start = NULL, unsigned int timeout = 0);


#endif // COMBINED_PREORDER_H
