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
void simple_Heavy_tester(unsigned int la_dw, unsigned int la_up, string dir/*, int timeout = 0*/);

#endif // MINIMIZATIONTECHNIQUES_TESTER_H
