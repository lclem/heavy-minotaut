
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file declaring automata minimization procedures. It includes         *
 *  the Heavy algorithm (see README.txt/Publications).                              *
 * 																					*
 ************************************************************************************/

#ifndef UP_SIMULATION_TESTER_H
#define UP_SIMULATION_TESTER_H


#include "../src/dw_simulation.hh"
#include "../src/up_simulation.hh"
#include "../src/combined_relation.hh"
#include "../src/quotienting.hh"
#include "../src/pruning.hh"
#include "../src/Common/statisticalResults.hh"

/* Encodings */
extern const unsigned int DW_LA_1ST_TIME         ;
extern const unsigned int UP_LA_WITH_ID_1ST_TIME ;
extern const unsigned int STRICT_UP_1_WITH_ID    ;
extern const unsigned int DW_LA_2ND_TIME         ;
extern const unsigned int UP_LA_WITH_ID_2ND_TIME ;
extern const unsigned int DW_1                   ;
extern const unsigned int UP_LA_WITH_DW_1        ;
extern const unsigned int numbRelations          ;

extern const unsigned int QUOT_WITH_DW_LA             ;
extern const unsigned int QUOT_WITH_UP_LA_WITH_ID_1ST ;
extern const unsigned int QUOT_WITH_UP_LA_WITH_ID_2ND ;
extern const unsigned int numbQuotientings            ;

extern const unsigned int PRUN_WITH_P_ID_AND_DW_LA            ;
extern const unsigned int PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA  ;
extern const unsigned int PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1;
extern const unsigned int numbPrunings                        ;


AutData removeDeadStates(const AutData &autData);

AutData heavy(const AutData &autData, unsigned int la_dw,
                     unsigned int la_up,
                     TestData& testData = emptyTestData,
                     string log_humanread_filename = "");
AutData heavyGradual(const AutData &autData_i, unsigned int la_dw, unsigned int la_dw_max,
                       unsigned int greatest_symbol,
                       TestData& testData = emptyTestData, seconds timeout_start = 0, string log_humanread_filename = "");

AutData quotient_with_combined_relation(const AutData &autData,
                                          unsigned int la_dw, unsigned int la_up,
                                          unsigned int numb_states = 0,
                                          unsigned int greatest_symbol = 0,
                                          seconds timeout_start = 0, seconds timeout = 0);

void prune_with_up_la_dw_la_and_strict_dw_la(Automaton& aut_i,
                                             stateDict stateDict,
                                             unsigned int la_dw, unsigned int la_up,
                                             const vector<typerank>& ranks,
                                             unsigned int numb_states = 0,
                                             unsigned int numb_symbols = 0,
                                             bool use_lvata = false);

AutData applyHeavy(const AutData &autData_i, MetaData& metaData,
                unsigned int la_dw, unsigned int la_up,
                TestData& testData_heavy, Timeout& timeout_heavy, string log);

void applyQuotCombinedPreorder(const AutData& autData, stateDict sDict, MetaData& metaData,
                               unsigned int la_dw, unsigned int la_up,
                               TestData& testData, Timeout& timeout,
                               string log_humanread_filename,
                               string log_machread_filename,
                               string log_machread_times_filename);

AutData quotient_with_dw_la_sim(const AutData &autData,
                    unsigned int la_dw,
                    unsigned int greatest_state  = 0,
                    unsigned int greatest_symbol = 0,
                    string log_humanread_filename = "");

AutData applyMinimizationSequence(const AutData& autData_i,
                                  unsigned int la_dw, unsigned int la_up,
                                  unsigned int greatest_state  = 0,
                                  unsigned int greatest_symbol = 0,
                                  seconds timeout_start = 0,
                                  TestData& testData = emptyTestData,
                                  string log_humanread_filename = "");


#endif // UP_SIMULATION_TESTER_H
