
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file for a collection of functions that aid in the manipulation      *
 *  and analysis of automata.                                                       *
 * 																					*
 ************************************************************************************/

#ifndef _AUTOMATONHELPER_HH_
#define _AUTOMATONHELPER_HH_


#include "common.hh"
#include "debugging.hh"

using Automaton                  =  VATA::ExplicitTreeAut;
using typerank                   =  unsigned int;                    /* A type for the rank of a symbol. */
using AutData                    =  tuple<Automaton, vector<typerank>>;
using Convert                    =  VATA::Util::Convert;
using state                      =  VATA::AutBase::StateType; //long unsigned int;	 	   /* Internal representation of states */
using stateSet                   =  std::unordered_set<state>;
using stateDict                  =  VATA::AutBase::StateDict;
using stateExt                   =  const std::basic_string<char>;   /* External representation of states */
using stateToStateMap            =  std::unordered_map<state, state>;
using stateToStateTranslWeak     =  VATA::Util::TranslatorWeak<stateToStateMap>;
using alphabet                   =  std::shared_ptr<Automaton::AbstractAlphabet>;
using abstrAlphabet              =  Automaton::AlphabetType;
using onTheFlyAlphabet           =  Automaton::OnTheFlyAlphabet;
using symbol                     =  unsigned int;                    /* Internal representation of alphavet symbols */
using symbolExt                  =  Automaton::StringRank;	 	     /* External representation of symbols */
using lv_transition              =  Automaton::Transition;
using transition                 =  lv_transition;
using my_transition              =  tuple<vector<state>, symbol, state>;
using my_transitions             =  vector<vector<my_transition> >;
using transitions                =  vector<vector<transition> >;
using stateDiscontBinaryRelation =  Automaton::StateDiscontBinaryRelation;
using simParam                   =  VATA::SimParam;
using finalStateSet              =  std::unordered_set<state>;

// delete later
string removeAlphabetLine(string autStr);

AutData parseFromString(string autStr, stateDict& stateDict);
Automaton parseFromString2(string autStr, stateDict& stateDict, vector<typerank>& ranks);
AutData parseFromFile(string filename, stateDict& stateDict);
const Automaton& getAut(const AutData &autData);
const vector<typerank> &getRanks(const AutData &autData);
AutData wrapAutData(const Automaton &aut, const vector<typerank> &ranks);
tuple<Automaton,Automaton> parseFromString2(string autStr, stateDict& stateDict, bool flag_removeUseless=true, bool flag_removeUnreachable=true);
AutData copyAutWithoutTrans(const AutData &old_autData);
Automaton copyAutWithoutTrans(const Automaton &old_aut);
Automaton copyAut(const Automaton &old_aut, bool deep_copy = true);
AutData copyAut(const AutData &old_autData, bool deep_copy = true);
Automaton addInitialState(const Automaton& old_aut);
Automaton removeInitialState(const Automaton& old_aut);
AutData removeUselessStates(const AutData& old_autData);
vector<unsigned int> mapGetNumbTrans(const vector<vector<transition> > &);
stateSet getUsedStates(const Automaton&);
state getGreatestUsedState(const Automaton& aut);
state getGreatestUsedState(const AutData& aut);
symbol getGreatestUsedSymbol(const Automaton &aut);
symbol getGreatestUsedSymbol(const AutData &autData);
state getInitialState(const Automaton& aut);
unsigned int getNumbUsedStates(const Automaton&, bool ignoreInitialSt = false);
unsigned int getNumbUsedStates(const AutData&, bool ignoreInitialSt = false);
string getNumbUsedStates_str(const Automaton&, bool ignoreInitialSt = false);
string getNumbUsedStates_str(const AutData&, bool ignoreInitialSt = false);
unsigned int getNumbTransitions(const Automaton &aut, bool ignoreLeafRules = false);
unsigned int getNumbTransitions(const AutData &autData, bool ignoreLeafRules = false);
string getNumbTransitions_str(const Automaton &aut, bool ignoreLeafRules = false);
string getNumbTransitions_str(const AutData& autData, bool ignoreLeafRules = false);
vector<symbol> getLeafSymbols(const Automaton& aut);
unsigned int getNumbLeafTransitions(const Automaton&);
unsigned int getNumbLeafSymbols(const Automaton& aut);
unsigned int getNumbSymbols(const Automaton & aut, bool ignoreLeafSymbols = false);
float getTransitionDensity(const Automaton& aut, bool ignoreLeafRules = false);
float getTransitionDensity(const AutData& autData, bool ignoreLeafRules = false);
string getTransitionDensity_str(const Automaton& aut, bool ignoreLeafRules = false);
float getAvgRank(const Automaton &aut);
string transitionToString(const transition&);
vector<bool> getIsFinal(const Automaton &, const unsigned int n);
bool* getIsFinal2(const Automaton &aut, const unsigned int n);
symbolExt translateSymbol(const Automaton &, symbol);
stateExt translateState(const Automaton &aut, const stateDict&, const state);
vector<vector<pair<transition,size_t>> > obtainTransBotUp(const Automaton&, const unsigned int);
typerank getGreatestRank(const vector<typerank>& ranks);
stateDiscontBinaryRelation computeLVDwSimulation(const Automaton &aut, const unsigned int numb_states);
unsigned int getSizeLVStateBinRelation(stateDiscontBinaryRelation& binRel, const unsigned int numb_states);
float measureStatesReduction(const Automaton &smaller, const Automaton &larger, bool ignoreInitialSt = false);
float measureStatesReduction(const AutData &smaller, const AutData &larger, bool ignoreInitialSt = false);
float measureTransitionsReduction(const Automaton &smaller, const Automaton &larger, bool ignoreInitialSt = false);
float measureTransitionsReduction(const AutData &smaller, const AutData &larger, bool ignoreInitialSt = false);
float measureTransDensReduction(const Automaton &smaller, const Automaton &larger, bool ignoreInitialSt = false);
float measureTransDensReduction(const AutData &smaller, const AutData &larger, bool ignoreInitialSt = false);
vector<float> measureTransOverlaps(const Automaton& aut);
vector<float> measureTransOverlaps(const AutData& autData);
string measureTransOverlaps_str2Dec(const Automaton& aut);
string measureTransOverlaps_str2Dec(const AutData& autData);
bool equiv(const Automaton &aut1, const Automaton &aut2);
bool equiv(const AutData &aut1, const AutData &aut2);
bool langIsEmpty(const Automaton& aut);
Automaton complement(const Automaton& aut1);
AutData complement(const AutData& autData);
string convert2DecStr(float f);
//Automaton complement(const Automaton& aut1, TestData& testData, string log);
//Automaton complement(const Automaton& aut1, TestData& testData, string log);
string autToStringTimbuk(const Automaton& aut);
void printAut(const Automaton &, stateDict * = NULL);
void printAut(const AutData& autData);
void printAutData(const Automaton &aut, bool printTransOverlap = false);
void saveToFile(const AutData& autData, string filename, bool removeInitialSt = true);
void saveToFile(const Automaton& aut, string filename, bool removeInitialSt = true);
AutData reindexStates(const AutData& oldAutData);

inline bool equalSize(const Automaton& aut_1, const Automaton& aut_2)
{
    return (getNumbUsedStates(aut_1) == getNumbUsedStates(aut_2)) &&
          (getNumbTransitions(aut_1) == getNumbTransitions(aut_2));
}
inline bool equalSize(const AutData& aut_1, const AutData& aut_2)
{
    return equalSize(getAut(aut_1),getAut(aut_2));
}


#endif
