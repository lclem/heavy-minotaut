
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file declaring automata minimization procedures. It includes the     *
 *  procedures introduced in the literature (see CONCUR'15 paper in README file).   *
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
                       TestData& testData = emptyTestData, timespec* timeout_start = NULL, string log_humanread_filename = "");

void applyProcedure1(const Automaton& aut_i, const vector<typerank> &ranks, stateDict sDict,
                    unsigned int la_dw,  unsigned int la_up,
                    MetaData &metaData             =  emptyMetaData,
                    TestData &testData_ru          =  emptyTestData,
                    TestData &testData_quot        =  emptyTestData,
                    Timeout &timeout_quot          =  emptyTimeout,
                    TestData &testData_qAndP       =  emptyTestData,
                    Timeout &timeout_qAndP         =  emptyTimeout,
                    TestData &testData_heavy       =  emptyTestData,
                    TestData &testData_heavy_rels  =  emptyTestData,
                    Timeout &timeout_heavy         =  emptyTimeout,
                    string log_humanread_filename  =  "",
                    string log_machread_filename   =  "", string log_machread_heavy_1_1_times_filename = "");

void applyProcedure2(const Automaton& aut_i, const vector<typerank> &ranks, stateDict sDict, MetaData& metaData,
                    TestData& testData_ru,
                    TestData& testData_quot,    Timeout& timeout_quot,
                    TestData& testData_qAndP,   Timeout& timeout_qAndP,
                    TestData& testData_heavy11, TestData& testData_heavy11_rels, Timeout& timeout_heavy11,
                    TestData& testData_heavy24, TestData& testData_heavy24_rels, Timeout& timeout_heavy24,
                    string log_humanread_filename, string log_machread_filename,
                    string log_machread_heavy_times_filename);

void applyProcedure22(const Automaton& aut_i, const vector<typerank> &ranks, stateDict sDict, MetaData& metaData,
                    TestData& testData_heavy11, TestData& testData_heavy11_rels, Timeout& timeout_heavy11,
                    TestData& testData_heavy24, TestData& testData_heavy24_rels, Timeout& timeout_heavy24,
                    string log_humanread_filename, string log_machread_filename,
                    string log_machread_heavy_times_filename);

void applyProcedure3(const AutData &autData_i, stateDict sDict, MetaData& metaData,
                    TestData& testData_ru,
                    TestData& testData_quot,    Timeout& timeout_quot,
                    TestData& testData_qAndP,   Timeout& timeout_qAndP,
                    TestData& testData_heavy11, TestData& testData_heavy11_rels, Timeout& timeout_heavy11,
                    TestData& testData_heavy24, TestData& testData_heavy24_rels, Timeout& timeout_heavy24,
                    TestData& testData_heavy36, TestData& testData_heavy36_rels, Timeout& timeout_heavy36,
                    string log_humanread_filename, string log_machread_filename,
                    string log_machread_heavy_times_filename);

AutData quotient_with_combined_relation(const AutData &autData,
                                          unsigned int la_dw, unsigned int la_up,
                                          unsigned int numb_states = 0,
                                          unsigned int greatest_symbol = 0,
                                          timespec *timeout_start = NULL, unsigned int timeout = 0);

void quotient_with_up_la_dw_la(Automaton& aut_i,
                                      unsigned int la_dw, unsigned int la_up,
                                      const vector<typerank>& ranks,
                                      unsigned int numb_states = 0,
                                      unsigned int numb_symbols = 0,
                                      bool use_lvata = false);

void prune_with_up_la_dw_la_and_strict_dw_la(Automaton& aut_i,
                                             stateDict stateDict,
                                             unsigned int la_dw, unsigned int la_up,
                                             const vector<typerank>& ranks,
                                             unsigned int numb_states = 0,
                                             unsigned int numb_symbols = 0,
                                             bool use_lvata = false);

void applyHeavy(const AutData &autData_i, stateDict sDict, MetaData& metaData,
                unsigned int la_dw, unsigned int la_up,
                TestData& testData_heavy, Timeout& timeout_heavy, string log_machread_filename,
                string log_machread_heavy_times_filename);

void applyQuotCombinedPreorder(const AutData& autData, stateDict sDict, MetaData& metaData,
                               unsigned int la_dw, unsigned int la_up,
                               TestData& testData, Timeout& timeout,
                               string log_humanread_filename,
                               string log_machread_filename,
                               string log_machread_times_filename);

AutData applyQuotDwLa(const AutData &autData,
                    unsigned int la_dw,
                    unsigned int greatest_state  = 0,
                    unsigned int greatest_symbol = 0,
                    string log_humanread_filename = "");


#endif // UP_SIMULATION_TESTER_H
