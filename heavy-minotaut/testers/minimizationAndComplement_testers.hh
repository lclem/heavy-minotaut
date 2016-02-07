#ifndef MINIMIZATIONANDCOMPLEMENT_TESTERS_CPP
#define MINIMIZATIONANDCOMPLEMENT_TESTERS_CPP


#include "common.hh"
#include "../src/minimizationTechniques.hh"
#include "../src/Common/statisticalResults.hh"

AutData complement_after_heavy(const AutData& autData_i, unsigned int la_dw, unsigned int la_up,
                               TestData& testData_h, TestData& testData_hAndc, string log);
void complement_after_heavy_tester(unsigned int la_dw, unsigned int la_up,
                                   string input_dir, string output_dir = "", unsigned int numb_tests = 0, string log_avgs = "");
void complement_after_heavy_tester(unsigned int la_dw, unsigned int la_up,
                                   vector<string> input_dir, string output_dir, unsigned int numb_tests = 0);

Automaton test();


#endif // MINIMIZATIONANDCOMPLEMENT_TESTERS_CPP
