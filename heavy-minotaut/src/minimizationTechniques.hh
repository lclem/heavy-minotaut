#ifndef UP_SIMULATION_TESTER_H
#define UP_SIMULATION_TESTER_H


#include "../src/dw_simulation.hh"
#include "../src/up_simulation.hh"
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


Automaton removeDeadStates(const Automaton& aut_i);

Automaton applyHeavy(Automaton& aut_i, stateDict stateDict, unsigned int la_dw,
                     unsigned int la_up,
                     TestData& testData = emptyTestData,
                     string log_humanread_filename = "");

void applyProcedure1(const Automaton& aut_i, stateDict sDict,
                    unsigned int la_dw,  unsigned int la_up,
                    MetaData &metaData             =  emptyMetaData,
                    TestData &testData_ru          =  emptyTestData,
                    Timeout &timeout_ru            =  emptyTimeout,
                    TestData &testData_quot        =  emptyTestData,
                    Timeout &timeout_quot          =  emptyTimeout,
                    TestData &testData_qAndP       =  emptyTestData,
                    Timeout &timeout_qAndP         =  emptyTimeout,
                    TestData &testData_heavy       =  emptyTestData,
                    TestData &testData_heavy_rels  =  emptyTestData,
                    Timeout &timeout_heavy         =  emptyTimeout,
                    string log_humanread_filename  =  "",
                    string log_machread_filename   =  "", string log_machread_heavy_1_1_times_filename = "");

void applyProcedure2(const Automaton& aut_i, stateDict sDict, MetaData& metaData,
                    TestData& testData_ru,      Timeout& timeout_ru,
                    TestData& testData_quot,    Timeout& timeout_quot,
                    TestData& testData_qAndP,   Timeout& timeout_qAndP,
                    TestData& testData_heavy11, TestData& testData_heavy11_rels, Timeout& timeout_heavy11,
                    TestData& testData_heavy24, TestData& testData_heavy24_rels, Timeout& timeout_heavy24,
                    string log_humanread_filename, string log_machread_filename,
                    string log_machread_heavy_times_filename);

void applyProcedure3(const Automaton& aut_i, stateDict sDict, MetaData& metaData,
                    TestData& testData_ru,      Timeout& timeout_ru,
                    TestData& testData_quot,    Timeout& timeout_quot,
                    TestData& testData_qAndP,   Timeout& timeout_qAndP,
                    TestData& testData_heavy11, TestData& testData_heavy11_rels, Timeout& timeout_heavy11,
                    TestData& testData_heavy24, TestData& testData_heavy24_rels, Timeout& timeout_heavy24,
                    TestData& testData_heavy36, TestData& testData_heavy36_rels, Timeout& timeout_heavy36,
                    string log_humanread_filename, string log_machread_filename,
                    string log_machread_heavy_times_filename);


#endif // UP_SIMULATION_TESTER_H
