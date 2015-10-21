
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
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
#include "exceptions.hh"

using Automaton                  =  VATA::ExplicitTreeAut;
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
using typerank                   =  unsigned int;                    /* A type for the rank of a symbol. */
using lv_transition              =  Automaton::Transition;
using transition                 =  lv_transition;
using my_transition              =  tuple<vector<state>, symbol, state>;
using my_transitions             =  vector<vector<my_transition> >;
using transitions                =  vector<vector<transition> >;
using stateDiscontBinaryRelation =  Automaton::StateDiscontBinaryRelation;
using simParam                   =  VATA::SimParam;
using finalStateSet              =  std::unordered_set<state>;

Automaton parseFromString(string, stateDict&, bool, bool);
tuple<Automaton,Automaton> parseFromString2(string autStr, stateDict& stateDict, bool flag_removeUseless=true, bool flag_removeUnreachable=true);
Automaton copyAutWithoutTrans(const Automaton &old_aut);
Automaton copyAut(const Automaton &old_aut, bool deep_copy = true);
Automaton addInitialState(const Automaton& old_aut);
Automaton removeInitialState(const Automaton& old_aut);
Automaton removeUselessStates(const Automaton& old_aut);
vector<unsigned int> mapGetNumbTrans(const vector<vector<transition> > &);
stateSet getUsedStates(const Automaton&);
state getGreatestUsedState(const Automaton& aut);
symbol getGreatestUsedSymbol(const Automaton& aut);
state getInitialState(const Automaton& aut);
unsigned int getNumbUsedStates(const Automaton &);
unsigned int getNumbTransitions(const Automaton&);
unsigned int getNumbLeafTransitions(const Automaton&);
unsigned int getNumbSymbols(const Automaton &);
float getTransitionDensity(const Automaton& aut);
float getAvgRank(const Automaton &aut);
string transitionToString(const transition&);
vector<bool> getIsFinal(const Automaton &, const unsigned int n);
bool* getIsFinal2(const Automaton &aut, const unsigned int n);
vector<typerank> getRanks(const Automaton& aut, const unsigned int greatest_symbol);
typerank* getRanks2(const Automaton &aut, const unsigned int n);
symbolExt translateSymbol(const Automaton &, symbol);
stateExt translateState(const Automaton &aut, const stateDict&, const state);
vector<vector<pair<transition,size_t>> > obtainTransBotUp(const Automaton&, const unsigned int);
void setGreatestRank(const vector<typerank> &ranks);
stateDiscontBinaryRelation computeLVDwSimulation(const Automaton &aut, const unsigned int numb_states);
unsigned int getSizeLVStateBinRelation(stateDiscontBinaryRelation& binRel, const unsigned int numb_states);
float measureStatesReduction(const Automaton &smaller, const Automaton &larger);
float measureTransitionsReduction(const Automaton &smaller, const Automaton &larger);
float measureTransDensReduction(const Automaton &smaller, const Automaton &larger);
vector<float> measureTransOverlaps(const Automaton& aut);
state reindexStates(Automaton& aut);
bool equiv(const Automaton &aut1, const Automaton &aut2);
bool equiv2(const Automaton &aut1, const Automaton &aut2);
bool langIsEmpty(const Automaton& aut);
string autToStringTimbuk(const Automaton& aut);
void printAut(const Automaton &, stateDict * = NULL);
void printAutData(const Automaton &aut, bool printTransOverlap = false);

inline bool equalSize(const Automaton& aut_1, const Automaton& aut_2)
{
    return (getNumbUsedStates(aut_1) == getNumbUsedStates(aut_2)) &&
          (getNumbTransitions(aut_1) == getNumbTransitions(aut_2));
}

extern typerank GREATEST_RANK;
extern state NO_STATE;
extern state NO_SYMBOL;


#endif
