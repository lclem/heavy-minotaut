#ifndef EXECUTIONOPTIONS_HH
#define EXECUTIONOPTIONS_HH

#include "common.hh"

// Aliases
typedef unsigned int opt;

// Opts
extern const unsigned int ORDER_OF_ATTACKS_TRANS;
extern const unsigned int NO_PARTICULAR_ORDER,
                          SEMI_ORDERED_BY_INITIAL_TRANS,
                          SEMI_ORDERED_BY_SYMBOLS_RANKING;

extern const unsigned int TYPE_OF_HISTORY_OF_GOOD_ATKS;
extern const unsigned int NONE,
                          LOCAL,
                          SEMI_GLOBAL,
                          GLOBAL,
                          GLOBAL_V2;
extern const unsigned int TYPE_OF_HISTORY_OF_BAD_ATKS;

extern const unsigned int THREE_VALUES_LOGIC;
extern const unsigned int OFF,
                          ON_V1,
                          ON_V2;

extern vector<unsigned int> OPT;

// Functions
void startOPT();
void setOPT(unsigned int option, unsigned int value);


#endif // EXECUTIONOPTIONS_HH
