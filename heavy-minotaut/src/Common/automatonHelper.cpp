
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation of a collection of functions that aid in the manipulation    *
 *  and analysis of automata.                                                       *
 * 																					*
 ************************************************************************************/

#include "automatonHelper.hh"

Automaton addInitialState(const Automaton& old_aut)
{
    Automaton new_aut = copyAutWithoutTrans(old_aut);

    vector<state> children;
    bool hasLeafRules = false;
    state initialState = getGreatestUsedState(old_aut)+1;

    for (const lv_transition& trans : old_aut) {
        if (!trans.GetChildren().empty())
            new_aut.AddTransition(trans.GetChildren(), trans.GetSymbol(), trans.GetParent());
        else
        {
            hasLeafRules = true;
            children = {initialState};
            new_aut.AddTransition(children, trans.GetSymbol(), trans.GetParent());
        }
    }

    if (!hasLeafRules)
    {
        Automaton empty;
        return empty;
    }

    return new_aut;
}

Automaton removeInitialState(const Automaton& old_aut)
{
    state initialState = getInitialState(old_aut);
    // This assumes that the explicit initial state has been added to the automaton,
    // which necessarily happens except when the input automaton had no leaf-rules
    // (in such a case, discarding the automaton is advisable).

    Automaton new_aut = copyAutWithoutTrans(old_aut);
    vector<state> empty_children(0), children, initial_state_children = {initialState};

    for (const lv_transition& trans : old_aut)
    {
        children = trans.GetChildren();
        if (children == initial_state_children)
            new_aut.AddTransition(empty_children, trans.GetSymbol(), trans.GetParent());
        else
            new_aut.AddTransition(children, trans.GetSymbol(), trans.GetParent());
    }

    return new_aut;
}

/* Reindex the states if they are not continuous, e.g.: states = {0,2,5} becomes {0,1,2}.
 * This function was implemented with the aim of being used in between steps that delete states in the automaton,
 * namely Quotienting and Pruning techniques. However, calling it then turned out to make the overall execution slower. */
AutData reindexStates(const AutData& oldAutData)
{

    unsigned int greatestState = getGreatestUsedState(oldAutData);
    if (getNumbUsedStates(oldAutData) == greatestState+1)
        return oldAutData;     /* The states are already continuous. */

    Automaton aut = removeInitialState(getAut(oldAutData));

    state stateCnt = 0;
    stateToStateMap stateMap;
    stateToStateTranslWeak stateTransl(stateMap,
                                      [&stateCnt](const state&){return stateCnt++;});

    aut = aut.ReindexStates(stateTransl);

    aut = addInitialState(aut);

    AutData newAutData = wrapAutData(aut, getRanks(oldAutData));

    return newAutData;
}

AutData removeUselessStates(const AutData& old_autData)
{
    Automaton old_aut = getAut(old_autData);
    Automaton new_aut = removeInitialState(old_aut);
    new_aut = new_aut.RemoveUselessStates();
    new_aut = addInitialState(new_aut);

    AutData new_autData = wrapAutData(new_aut,getRanks(old_autData));

    return new_autData;
}

string removeAlphabetLine(string autStr)
{
    string prefix = "Ops\n";
    string rest = autStr.substr(autStr.find("Automaton"), autStr.length());

    return prefix + rest;
}

set<symbol> getUsedSymbols(const Automaton& aut)
{
    //vector<symbol> symbols;
    set<symbol> symbols;

    for (const lv_transition& trans : aut)
    {
        symbols.insert(trans.GetSymbol());
    }

    return symbols;
}

vector<typerank> getRanks(const Automaton& aut)
{
    set<symbol> symbols = getUsedSymbols(aut);
    unsigned int greatest_symbol = getGreatestUsedSymbol(aut);  // Could be improved.
    vector<typerank> ranks(greatest_symbol+1);  /* This alloc guarantees we keep the ranks for all symbs, */
                                                /* even if their int representation is not continuous.    */

    // Get a pointer to the alphabet of type onTheFlyAlphabet which is being used already
    // by 'aut'.
    std::shared_ptr<onTheFlyAlphabet> alph;
    if (!(alph = std::dynamic_pointer_cast<onTheFlyAlphabet>(aut.GetAlphabet())))
    {	// The alphabet should be ExplicitTreeAut::onTheFlyAlphabet,
        // if it isn't then throw 'false'.
        assert(false);
    }

    symbol s;
    typerank rank;
    for (auto stringSymbolPair : alph->GetSymbolDict())
    {
        s = stringSymbolPair.second;
        if (symbols.find(s) == symbols.end())
            continue;
        rank = stringSymbolPair.first.rank;
        if (rank==0) rank = 1;      /* Since we make the initial state explicit. */
        ranks.at(s) = rank;
        if (s > greatest_symbol)
            outputText("Oh no");
    }

    return ranks;
}


AutData parseFromString(string autStr, stateDict& stateDict)
{
    /* Parsing the automaton. */
    std::unique_ptr<VATA::Parsing::AbstrParser> parser(new VATA::Parsing::TimbukParser());

    //autStr = removeAlphabetLine(autStr);

    Automaton aut;
    Automaton::AlphabetType onTheFlyAlph(new Automaton::OnTheFlyAlphabet);
    aut.SetAlphabet(onTheFlyAlph);
    aut.LoadFromString(*parser, autStr, stateDict);

    aut = addInitialState(aut);

    /* Getting the ranks of the symbols. */
    vector<typerank> ranks = getRanks(aut);

    AutData autData = AutData (aut,ranks);

    return autData;
}

Automaton parseFromString2(string autStr, stateDict& stateDict, vector<typerank>& ranks)
{
    /* Parsing the automaton. */
    std::unique_ptr<VATA::Parsing::AbstrParser> parser(new VATA::Parsing::TimbukParser());

    //autStr = removeAlphabetLine(autStr);

    Automaton aut;
    Automaton::AlphabetType onTheFlyAlph(new Automaton::OnTheFlyAlphabet);
    aut.SetAlphabet(onTheFlyAlph);
    aut.LoadFromString(*parser, autStr, stateDict);

    aut = addInitialState(aut);

    /* Getting the ranks of the symbols. */
    ranks = getRanks(aut);

    return aut;
}

AutData parseFromFile(string filename, stateDict& stateDict)
{
    return parseFromString(VATA::Util::ReadFile(filename), stateDict);
}

/* Note that this function REWRITES the output automaton if a file with the same same already exists! */
void saveToFile(const Automaton& aut, string filename, bool removeInitialSt)
{
    Automaton aut_;
    if (removeInitialSt)
        aut_ = removeInitialState(aut);
    else
        aut_ = aut;

    writeToFile(filename, autToStringTimbuk(aut_), true);
}

void saveToFile(const AutData& autData, string filename, bool removeInitialSt)
{
    saveToFile(getAut(autData), filename, removeInitialSt);
}

const Automaton &getAut(const AutData &autData)
{
    const Automaton& aut = std::get<0>(autData);
    return aut;
}

const vector<typerank>& getRanks(const AutData &autData)
{
    return std::get<1>(autData);
}

AutData wrapAutData(const Automaton& aut, const vector<typerank>& ranks)
{
    return AutData (aut,ranks);
}

vector<unsigned int> mapGetNumbTrans(const vector<vector<transition> >& vec) {
    unsigned int size = vec.size();

    vector<unsigned int> result(size);
    for(unsigned int i=0; i<size; i++)
        result.at(i) = vec.at(i).size();

    return result;
}

/* Gets all the used states (i.e., states which appear in transitions
 * as parents or children) of the given automaton or are final states. */
stateSet getUsedStates(const Automaton& aut)
{
    // Create an unordered set called states.
    std::unordered_set<state> states;

    for (const lv_transition trans : aut)
    {
        // For each transition in 'aut', insert its parent in 'states'.
        states.insert(trans.GetParent());

        for (const state s : trans.GetChildren())
        {
            // Now for each children in that same transition,
            // insert it in 'states'.
            states.insert(s);
        }
    }

    finalStateSet f = aut.GetFinalStates();
    for (const state s : f)
    {
        states.insert(s);
    }

    return states;
}

unsigned int getNumbUsedStates(const Automaton& aut, bool ignoreInitialSt)
{
    if (ignoreInitialSt && getNumbUsedStates(aut) > 0)
        return getUsedStates(aut).size()-1;
    else
        return getUsedStates(aut).size();
}

unsigned int getNumbUsedStates(const AutData& autData, bool ignoreInitialSt)
{
    return getNumbUsedStates(getAut(autData), ignoreInitialSt);
}

string getNumbUsedStates_str(const AutData& autData, bool ignoreInitialSt)
{
    return std::to_string(getNumbUsedStates(autData, ignoreInitialSt));
}

string getNumbUsedStates_str(const Automaton& aut, bool ignoreInitialSt)
{
    return std::to_string(getNumbUsedStates(aut, ignoreInitialSt));
}

float getTransitionDensity(const Automaton& aut, bool ignoreLeafRules)
{
    float transDens = 0;

    unsigned int numb_states = getNumbUsedStates(aut, ignoreLeafRules);
    unsigned int numb_transitions = getNumbTransitions(aut, ignoreLeafRules);
    unsigned int numb_symbols = getNumbSymbols(aut, ignoreLeafRules);

    if (numb_states == 0 || numb_symbols == 0)
        transDens = 0;
    else
        transDens = (float) numb_transitions / ((float) numb_states * (float) numb_symbols);

    return transDens;
}

float getTransitionDensity(const AutData& autData, bool ignoreLeafRules)
{
    return getTransitionDensity(getAut(autData), ignoreLeafRules);
}

string getTransitionDensity_str(const Automaton& aut, bool ignoreLeafRules)
{
    return std::to_string(getTransitionDensity(aut, ignoreLeafRules));
}

vector<bool> getIsFinal(const Automaton& aut, const unsigned int n) {
    vector<bool> isFinal(n);

    finalStateSet f = aut.GetFinalStates();

    for(unsigned int i=0; i<n; i++)
        isFinal[i] = f.count(i);	/* count returns '1' if i is found in f and '0' otherwise. */

    return isFinal;
}

bool* getIsFinal2(const Automaton& aut, const unsigned int n) {
    bool* isFinal = new bool[n];

    finalStateSet f = aut.GetFinalStates();

    for(unsigned int i=0; i<n; i++)
        isFinal[i] = f.count(i);	/* count returns '1' if i is found in f and '0' otherwise. */

    return isFinal;
}

unsigned int getNumbLeafSymbols(const Automaton& aut)
{
    return (getLeafSymbols(aut)).size();
}

/* Returns the number of symbols in the alphabet of the automaton.
 * This function appears to not always work correctly. */
unsigned int getNumbSymbols(const Automaton& aut, bool ignoreLeafSymbols)
{
    unordered_set<symbol> symb;
    for (const transition& trans : aut)
        symb.insert(trans.GetSymbol());

    unsigned int numb = symb.size();

    if (ignoreLeafSymbols)
        return numb - getNumbLeafSymbols(aut);
    else
        return numb;
}

float getAvgRank(const Automaton& aut)
{
    // Get a pointer to the alphabet of type onTheFlyAlphabet which is being used already
    // by 'aut'.
    std::shared_ptr<onTheFlyAlphabet> alph;
    if (!(alph = std::dynamic_pointer_cast<onTheFlyAlphabet>(aut.GetAlphabet())))
    {	// The alphabet should be ExplicitTreeAut::onTheFlyAlphabet,
        // if it isn't then throw 'false'.
        assert(false);
    }

    typerank ranks = 0;
    size_t c = 0;
    for (const auto stringSymbolPair : alph->GetSymbolDict())
    {
        c++;
        ranks += stringSymbolPair.first.rank;
    }

    if (c==0)
        return 0;
    else
        return (float) ranks / (float) c;

}

state getGreatestUsedState(const Automaton& aut)
{
    state p=0;

    for (const lv_transition& trans : aut)
    {
        if (trans.GetParent() > p)
            p = trans.GetParent();

        for (const state& s : trans.GetChildren())
        {
            if (s > p)
                p = s;
        }
    }

    return p;
}

state getGreatestUsedState(const AutData& aut)
{

    return getGreatestUsedState(getAut(aut));

}


symbol getGreatestUsedSymbol(const Automaton& aut)
{
    symbol s=0;

    for (const lv_transition& trans : aut)
    {
        if (trans.GetSymbol() > s)
            s = trans.GetSymbol();
    }

    return s;
}

symbol getGreatestUsedSymbol(const AutData& autData)
{
    return getGreatestUsedSymbol(getAut(autData));
}

/* Returns a vector that at each index i contains a vector of pairs:
 * the first element is a transition (bottom-up) of aut in which i is a child and the second
 * element is the position at each i appears among the children of that transition. */
vector<vector<pair<transition,size_t>> > obtainTransBotUp(const Automaton& aut, unsigned int numbStates) {
    vector<vector<pair<transition,size_t>> > trans_botup(numbStates);

    for (const lv_transition trans : aut) {
        vector<state> children = trans.GetChildren();
        unsigned int size = children.size();

        for (unsigned int i=0; i<size; i++)
            trans_botup.at(children.at(i)).push_back(std::make_pair(trans,i));

    }

    return trans_botup;
}

bool equiv(const Automaton& aut1, const Automaton& aut2)
{
    Automaton aut1_ = removeInitialState(aut1);
    Automaton aut2_ = removeInitialState(aut2);

    bool result1 = true;
    result1 = VATA::ExplicitTreeAut::CheckInclusion(aut1_, aut2_);
    bool result2 = false;
    result2 = VATA::ExplicitTreeAut::CheckInclusion(aut2_, aut1_);

    return (result1 && result2);
}

bool equiv(const AutData& autData1, const AutData& autData2)
{

    return equiv(getAut(autData1),getAut(autData2));

}

bool langIsEmpty(const Automaton& aut)
{
    Automaton empty;
    return equiv(empty,aut);
}

Automaton complement(const Automaton& aut1)
{
    Automaton aut1_ = removeInitialState(aut1);
    Automaton aut2  = aut1_.Complement();

    //equiv(aut1, aut1);

    return addInitialState(aut2);
}

AutData complement(const AutData& autData)
{
    return wrapAutData(complement(getAut(autData)), getRanks(autData));
}

state getInitialState(const Automaton& aut)
{
    return getGreatestUsedState(aut);
}


/* String and IO functions */

string transitionToString(const lv_transition& trans)
{
    return "<state=" + std::to_string(trans.GetParent()) + ", symbol="
    + std::to_string(trans.GetSymbol()) + ", children="
    + Convert::ToString(trans.GetChildren()) + ">";
}

/* Given a symbol in its internal representation, the function returns
 * its external representation. */
symbolExt translateSymbol(const Automaton& aut, symbol symb_int){

    symbolExt symb_ext = aut.GetAlphabet()->GetSymbolBackTransl()->operator()(symb_int);

    return symb_ext;
}

/* Given a state in its internal representation, the function returns
 * its external representation. */
stateExt translateState(const Automaton& aut, const stateDict& dict, const state s_int) {

    std::basic_string<char> s_ext;

    state initialState = getInitialState(aut);
    if (s_int == initialState)
        s_ext = "ψ";
    else
        s_ext = dict.TranslateBwd(s_int);

    return s_ext;
}

/* Returns the number of transitions in the given automaton */
unsigned int getNumbTransitions(const Automaton& aut, bool ignoreLeafRules)
{
    unsigned int c = 0;
    for (const lv_transition trans : aut)
        c++;

    if (ignoreLeafRules)
        return c - getNumbLeafTransitions(aut);
    else
        return c;
}

unsigned int getNumbTransitions(const AutData& autData, bool ignoreLeafRules)
{
    return getNumbTransitions(getAut(autData), ignoreLeafRules);
}

string getNumbTransitions_str(const AutData& autData, bool ignoreLeafRules)
{
    return std::to_string(getNumbTransitions(autData,ignoreLeafRules));
}

string getNumbTransitions_str(const Automaton& aut, bool ignoreLeafRules)
{
    return std::to_string(getNumbTransitions(aut,ignoreLeafRules));
}

unsigned int getNumbLeafTransitions(const Automaton& aut)
{
    unsigned int c = 0;
    state initialState = getInitialState(aut);
    // This assumes that the explicit initial state has been added to the automaton,
    // which necessarily happens except when the input automaton had no leaf-rules
    // (in such a case, discarding the automaton is advisable).
    vector<state> initial_state_children = {initialState};
    for (const lv_transition trans : aut)
    {
        if (trans.GetChildren() == initial_state_children)
            c++;
    }

    return c;
}

vector<symbol> getLeafSymbols(const Automaton& aut)
{
    vector<symbol> leaf_symbols;
    state initialState = getInitialState(aut);
    // This assumes that the explicit initial state has been added to the automaton,
    // which necessarily happens except when the input automaton had no leaf-rules
    // (in such a case, discarding the automaton is advisable).
    vector<state> initial_state_children = {initialState};
    for (const lv_transition& trans : aut)
    {
        if (trans.GetChildren() == initial_state_children)
            leaf_symbols.push_back(trans.GetSymbol());
    }

    //leaf_symbols.

    return removeDuplicates(leaf_symbols);
}

/* Computes a copy of the given automaton. This will be a full copy if
 * deep_copy=true (the default). If it is false, it will possibly be a shallow copy (the copy
 * constructor for Automaton will be used if defined). */
Automaton copyAut(const Automaton& old_aut, bool deep_copy)
{
    Automaton new_aut;

    if (!deep_copy)     // Then use the user-defined object copy constructor.
        new_aut = old_aut;
    else
    {
        finalStateSet f = old_aut.GetFinalStates();
        for (const state& s : f)
        {
            new_aut.SetStateFinal(s);
        }

        const abstrAlphabet& alpha = old_aut.GetAlphabet();
        abstrAlphabet newAlpha = abstrAlphabet(new onTheFlyAlphabet(static_cast<const onTheFlyAlphabet&>(*alpha)));
        new_aut.SetAlphabet(newAlpha);

        for (const lv_transition& trans : old_aut)
            new_aut.AddTransition(trans.GetChildren(), trans.GetSymbol(), trans.GetParent());
    }

    return new_aut;
}

AutData copyAut(const AutData& old_autData, bool deep_copy)
{

    Automaton new_aut = copyAut(getAut(old_autData), deep_copy);

    return wrapAutData(new_aut,getRanks(old_autData));

}

/* Returns a copy of the given automaton but without any transition. */
Automaton copyAutWithoutTrans(const Automaton& old_aut)
{
    Automaton new_aut/*, old_aut = getAut(old_autData)*/;

    finalStateSet f = old_aut.GetFinalStates();
    for (const state& s : f)
    {
        new_aut.SetStateFinal(s);
    }

    const abstrAlphabet& alpha = old_aut.GetAlphabet();
    abstrAlphabet newAlpha = abstrAlphabet(new onTheFlyAlphabet(static_cast<const onTheFlyAlphabet&>(*alpha)));
    new_aut.SetAlphabet(newAlpha);

    return new_aut;
}

AutData copyAutWithoutTrans(const AutData& old_autData)
{

    Automaton new_aut = copyAutWithoutTrans(getAut(old_autData));

    return wrapAutData(new_aut,getRanks(old_autData));
}

typerank getGreatestRank(const vector<typerank>& ranks)
{
    return *std::max_element(ranks.begin(),ranks.end());
}

stateDiscontBinaryRelation computeLVDwSimulation(const Automaton& aut, const unsigned int numb_states) {
    simParam sp;
    sp.SetRelation(simParam::e_sim_relation::TA_DOWNWARD);
    sp.SetNumStates(numb_states);
    stateDiscontBinaryRelation sim = aut.ComputeSimulation(sp);

    return sim;
}

unsigned int getSizeLVStateBinRelation(stateDiscontBinaryRelation& binRel, unsigned int numb_states) {
    unsigned int c = 0;

    for (unsigned int p=0; p<numb_states; p++)
        for (unsigned int q=0; q<numb_states; q++) {
            if (binRel.get(p,q))
                c++;
        }

    return c;
}

bool statesIntersect(const vector<state>& vec1, const vector<state>& vec2)
{
    return (std::find_first_of(vec1.begin(), vec1.end(), vec2.begin(), vec2.end()) != vec1.end());
}

bool measureTO_aux(const vector<state>& vec1, const vector<state>& vec2)
{
    if (vec1.size() != vec2.size())
        exit_with_error("Function foo received two vectors of different size.\n");

    for (unsigned int i=0; i<vec1.size(); i++)
        if (vec1.at(i) == vec2.at(i)) return true;

    return false;
}

/* Measures the overlap of transitions in the given automaton and returns a tuple as follows:
 * - the first element is the number of transitions (%) for which at least one of their children states
 * is also part of another transition with the same parent and symbol and at that same position;
 * - the second element is an average of how many (%) children states of a transition that overlaps
 * are also children states of a different transition with the same parent and symbol (maintaining their positions).
 * - the third element gives an average of, for each transition that overlaps, how many times each of its children
 * states appears at that same position in an additional transition with the same parent and symbol.
*/
vector<float> measureTransOverlaps(const Automaton& aut)
{
    std::set<transition> transWhichOverlap;     // Using a set forces that each transition shall be counted only once.

    for (const transition& trans1 : aut)
    {
        for (const transition& trans2 : aut)
        {
            if (trans2==trans1)
                continue;
            if (trans1.GetParent()==trans2.GetParent() && trans1.GetSymbol()==trans2.GetSymbol() && measureTO_aux(trans1.GetChildren(), trans2.GetChildren()))
            {
                transWhichOverlap.insert(trans2);
            }
        }
    }

    unsigned int numb_transitions = getNumbTransitions(aut, true);
    float avg_transWhichOverlap = ((float) transWhichOverlap.size() / (float) numb_transitions) * (float) 100;

    float avg = 0, n = 0;
    for (std::set<transition>::iterator it1=transWhichOverlap.begin(); it1!=transWhichOverlap.end(); ++it1)
    {
        size_t numb_children = (*it1).GetChildren().size(), c = 0;
        if (numb_children==0) continue;

        for (unsigned int i=0; i<numb_children; i++)
        {
            for (std::set<transition>::iterator it2=transWhichOverlap.begin(); it2!=transWhichOverlap.end(); ++it2)
            {
                if (it2==it1)
                    continue;
                if ((*it1).GetParent()==(*it2).GetParent() && (*it1).GetSymbol()==(*it2).GetSymbol() && (*it1).GetChildren().at(i)==(*it2).GetChildren().at(i))
                {
                    c++;
                    break;
                }
            }
        }

        avg += (float) c / (float) numb_children;
        n++;
    }
    if (n > 0)
        avg = (avg / (float) n) * (float) 100;
    else
        avg = 0;
    float avg1 = avg;

    avg = 0;
    n = 0;
    for (std::set<transition>::iterator it1=transWhichOverlap.begin(); it1!=transWhichOverlap.end(); ++it1)
    {
        size_t numb_children = (*it1).GetChildren().size(), c = 0;
        if (numb_children==0) continue;

        for (unsigned int i=0; i<numb_children; i++)
        {
            for (std::set<transition>::iterator it2=transWhichOverlap.begin(); it2!=transWhichOverlap.end(); ++it2)
            {
                if (it2==it1)
                    continue;
                if ((*it1).GetParent()==(*it2).GetParent() && (*it1).GetSymbol()==(*it2).GetSymbol() && (*it1).GetChildren().at(i)==(*it2).GetChildren().at(i))
                {
                    c++;
                }
            }
        }
        avg += (float) c / (float) numb_children;
        n++;
    }
    if (n > 0)
        avg = (avg / (float) n);
    else
        avg = 0;
    float avg2 = avg;

    vector<float> result = {avg_transWhichOverlap, avg1, avg2};

    return result;
}

vector<float> measureTransOverlaps(const AutData& autData)
{
    return measureTransOverlaps(getAut(autData));
}

string measureTransOverlaps_str2Dec(const Automaton& aut)
{
    vector<float> overlaps = measureTransOverlaps(aut);
    return "(" + convert2DecStr(overlaps.at(0)) + ", " +
            convert2DecStr(overlaps.at(1)) + ", " +
            convert2DecStr(overlaps.at(2)) + ")";
}

string measureTransOverlaps_str2Dec(const AutData& autData)
{
    return measureTransOverlaps_str2Dec(getAut(autData));
}

/* Returns the value of numbStates(smaller) / numbStates(larger) in %. */
float measureStatesReduction(const Automaton& smaller, const Automaton& larger, bool ignoreInitialSt)
{
    float states_red;

    if (getNumbUsedStates(larger,ignoreInitialSt) == 0)
        states_red = 100;
    else
        states_red = ((float) getNumbUsedStates(smaller,ignoreInitialSt)
                      / (float) getNumbUsedStates(larger,ignoreInitialSt)) * (float) 100;

    return states_red;
}

float measureStatesReduction(const AutData& smaller, const AutData& larger, bool ignoreInitialSt)
{
    return measureStatesReduction(getAut(smaller), getAut(larger), ignoreInitialSt);
}

float measureTransitionsReduction(const Automaton& smaller, const Automaton& larger, bool ignoreInitialSt)
{
    float transitions_red;

    if (getNumbTransitions(larger,ignoreInitialSt) == 0)
        transitions_red = 100;
    else
        transitions_red = ((float) getNumbTransitions(smaller,ignoreInitialSt)
                           / (float) getNumbTransitions(larger,ignoreInitialSt)) * (float) 100;

    return transitions_red;
}

float measureTransitionsReduction(const AutData& smaller, const AutData& larger, bool ignoreInitialSt)
{
    return measureTransitionsReduction(getAut(smaller), getAut(larger),ignoreInitialSt);
}

float measureTransDensReduction(const Automaton& smaller, const Automaton& larger, bool ignoreInitialSt)
{
    return (getTransitionDensity(smaller,ignoreInitialSt) / getTransitionDensity(larger,ignoreInitialSt)) * (float) 100;
}

float measureTransDensReduction(const AutData& smaller, const AutData& larger, bool ignoreInitialSt)
{
    return measureTransDensReduction(getAut(smaller), getAut(larger),ignoreInitialSt);
}

/* Converts a float to a string representation containing 2 decimal values. */
string convert2DecStr(float f)
{
    return (boost::format("%.2f") % f).str();
}

void printAut(const Automaton& aut, stateDict* dict) {
    dict = NULL;        // I've given up printing fancy state names and considering whether the initial state is implicit or not.

    /* Print the used states */
    stateSet states = getUsedStates(aut);
    std::cout << "Q contains " << states.size() << " (used) states. ";
    std::cout << "Their representations are: ";
    for (const state& s : states)
    {
        std::cout << s << " (" << (dict==NULL ? "" : translateState(aut, *dict, s)) << ") ";
    }
    std::cout << "\n";

    /* Print the final states */
    finalStateSet f = aut.GetFinalStates();
    std::cout << "F contains " << f.size() << " final states. They are: ";
    for (const state& s : f)
    {
        std::cout << s << " (" << (dict==NULL ? "" : translateState(aut, *dict, s)) << " ) ";
    }
    std::cout << "\n";

    std::cout << "Sigma contains " << getNumbSymbols(aut) << " symbols "
              << "and their average rank is " << getAvgRank(aut) << ".\n";

    /* Print the alphabet  */
    /*
    // Get a pointer to the alphabet of type OnTheFlyAlphabet which is being used already
    // by 'aut'.
    std::shared_ptr<onTheFlyAlphabet> alph;
    if (!(alph = std::dynamic_pointer_cast<onTheFlyAlphabet>(aut.GetAlphabet())))
    {	// The alphabet should be ExplicitTreeAut::OnTheFlyAlphabet,
        // if it isn't then throw 'false'.
        assert(false);
    }

    std::cout << "Sigma contains " << sizeOfSigma(alph) << " symbols: ";
    for (auto stringSymbolPair : alph->GetSymbolDict())
    {	// For each symbol,
        // print its external representation (which includes its ranking) followed by its internal representation.
        std::cout << stringSymbolPair.first << " (" << stringSymbolPair.second << "); ";
    }
    std::cout << "\n";
    */

    /* Print the transitions */
    std::cout << "Delta contains " << getNumbTransitions(aut) << " transitions";

    std::cout << ":\n";
    for (const lv_transition& trans : aut)
    {
        std::cout << "    [parent state = " << trans.GetParent();
        std::cout << ", symbol = " << trans.GetSymbol();
        std::cout << ", children states = " << Convert::ToString(trans.GetChildren());
        std::cout << "]\n";
    }

    std::cout << std::flush;
}

void printAut(const AutData& autData)
{

    printAut(getAut(autData));

}

string autToStringTimbuk(const Automaton& aut)
{
    string result;

    result = "Ops \n";
    result += "Automaton anonymous \n";

    stateSet states = getUsedStates(aut);
    result += "States ";
    for (const state& s : states)
    {
        result += std::to_string(s) + " ";
    }
    result += "\n";

    finalStateSet f = aut.GetFinalStates();
    result += "Final States ";
    for (const state& s : f)
    {
        result += std::to_string(s) + " ";
    }
    result += "\n";

    result += "Transitions \n";
    for (const lv_transition& trans : aut)
    {
        result += std::to_string(trans.GetSymbol()) + "(";

        for(unsigned int i=0; i<trans.GetChildren().size(); i++)
        {
            result += std::to_string(trans.GetChildren().at(i));
            if (i < trans.GetChildren().size()-1)
                result += ",";
        }
        result += ") -> " + std::to_string(trans.GetParent()) + "\n";
    }

    return result;
}

void printAutData(const Automaton& aut, bool printTransOverlap) {
    /* Print the used states. */
    stateSet states = getUsedStates(aut);
    std::cout << "Q contains " << states.size() << " (used) states and ";
    std::cout << "F contains " << aut.GetFinalStates().size() << " final states. ";
    std::cout << "Sigma contains " << getNumbSymbols(aut) << " symbols "
              << "and their average rank is " << getAvgRank(aut) << ". ";
    std::cout << "Delta contains " << getNumbTransitions(aut) << " transitions and the transition density is "
              << getTransitionDensity(aut) << ". ";
    if (printTransOverlap)
    {
        vector<float> transOverlaps = measureTransOverlaps(aut);
        std::cout << transOverlaps.at(0)
                  << "% of the transitions overlap with some other transition (i.e., at least one of their children states is also part, at the same position, of another transition with the same parent and symbol). On average, for each transition that overlaps, the percentage of children states that are thus shared with a different transition "
                  << transOverlaps.at(1)
                  << "%. And, on average, for each transition that overlaps, each of its children states appears, at that same position, in "
                  << transOverlaps.at(2) << " additional transitions with the same parent and symbol. ";
    }
    std::cout << "\n";
}
