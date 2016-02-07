
#ifndef _MAIN_HH_
#define _MAIN_HH_


#include <boost/lexical_cast.hpp>
#include <iterator>
using namespace std;

#include "../src/Common/common.hh"
#include "../src/step.hh"
#include "../src/Common/debugging.hh"
#include "../src/Common/automatonHelper.hh"
#include "../src/dw_simulation.hh"
#include "../src/up_simulation.hh"
#include "../src/quotienting.hh"
#include "../src/pruning.hh"

#include "../testers/minimizationTechniques_testers.hh"
#include "../testers/minimizationAndComplement_testers.hh"

string minimize_cmd = "minimize";
string minimize_and_complement_cmd = "minimize_and_complement";


#endif
