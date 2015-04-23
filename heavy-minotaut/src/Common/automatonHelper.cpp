
#include "automatonHelper.hh"

state NO_STATE;
state NO_SYMBOL;
//state INITIAL_STATE;
//bool USING_INITIAL_STATE = false;
typerank GREATEST_RANK;

Automaton addInitialState(const Automaton& old_aut)
{
    //if (USING_INITIAL_STATE)
      //  return old_aut;

    Automaton new_aut = copyAutWithoutTrans(old_aut);

    vector<state> children;
    bool hasLeafRules = false;
    state initialState = getGreatestUsedState(old_aut)+1;

    /* INITIAL_STATE = numb_states;     Warning: this makes the initial state be equal to the number of states in the automaton, which
                                       is a value that can be taken by another state. However, since the states are indexed continuously from 0
                                        during the parsing step, this will only happen if some state has been deleted in the meantime. This is done
                                        when we remove dead states, quotient states, etc. */

    for (const lv_transition& trans : old_aut) {
        if (!trans.GetChildren().empty())
            new_aut.AddTransition(trans.GetChildren(), trans.GetSymbol(), trans.GetParent());
        else
        {
            hasLeafRules = true;
            //USING_INITIAL_STATE = true;
            children = {/*INITIAL_STATE*/ initialState};
            new_aut.AddTransition(children, trans.GetSymbol(), trans.GetParent());
        }
    }

    if (!hasLeafRules)
    {
        Automaton empty;
        return empty;
      //  throw autHasNoLeafRules();
    }

    return new_aut;
}

Automaton removeInitialState(const Automaton& old_aut)
{
  //  if (!USING_INITIAL_STATE)
    //    return old_aut;

    state initialState = getInitialState(old_aut);
    // This assumes that the explicit initial state has been added to the automaton,
    // which necessarily happens except when the input automaton had no leaf-rules
    // (in such a case, discarding the automaton is advisable).

    Automaton new_aut = copyAutWithoutTrans(old_aut);
    vector<state> empty_children(0), children, initial_state_children = {/*INITIAL_STATE*/ initialState};

    for (const lv_transition& trans : old_aut)
    {
        children = trans.GetChildren();
        if (children == initial_state_children)
            new_aut.AddTransition(empty_children, trans.GetSymbol(), trans.GetParent());
        else
            new_aut.AddTransition(children, trans.GetSymbol(), trans.GetParent());
    }

    //USING_INITIAL_STATE = false;

    return new_aut;
}

Automaton removeUselessStates(const Automaton& old_aut)
{
    Automaton new_aut = removeInitialState(old_aut);
    new_aut = new_aut.RemoveUselessStates();
    return addInitialState(new_aut);
}

string removeAlphabetLine(string autStr)
{
    string prefix = "Ops\n";
    string rest = autStr.substr(autStr.find("Automaton"), autStr.length());

    return prefix + rest;
}

Automaton parseFromString(string autStr, stateDict& stateDict, bool flag_removeUseless=true, bool flag_removeUnreachable=true) {
    std::unique_ptr<VATA::Parsing::AbstrParser> parser(new VATA::Parsing::TimbukParser());

    autStr = removeAlphabetLine(autStr);

    // create and load the automaton
    Automaton aut;
    Automaton::AlphabetType onTheFlyAlph(new Automaton::OnTheFlyAlphabet);
    aut.SetAlphabet(onTheFlyAlph);
    aut.LoadFromString(*parser, autStr, stateDict);

//    try
//    {
        aut = addInitialState(aut);
//    }
//    catch (autHasNoLeafRules& e)
//    {
//        Automaton empty;
//        std::cout << "The given automaton has no leaf-rules so the empty automaton will be returned instead. \n";
//        return std::make_tuple(empty, 0, 0);
//    }

    if (flag_removeUseless)
        aut = aut.RemoveUselessStates();  //Warning: there is the assumption that final states are never useless, so this never deletes them
    if (flag_removeUnreachable)
        aut = aut.RemoveUnreachableStates();

    //printAut(aut);

    //unsigned int numb_states = getNumbUsedStates(aut);  /* Number of used states including the initial state (if it has been added). */
    NO_STATE = /*numb_states+1;*/ getGreatestUsedState(aut)+1;
    //unsigned int numb_symbols = getNumbSymbols(aut) /*getGreatestUsedSymbol(aut)*/;
    NO_SYMBOL = /*numb_symbols*/getGreatestUsedSymbol(aut)+1;

    return /*std::make_tuple(*/aut/*, numb_states, numb_symbols)*/;
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
    std::unordered_set<unsigned int> states;

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

unsigned int getNumbUsedStates(const Automaton& aut)
{
    return getUsedStates(aut).size();
}

float getTransitionDensity(const Automaton& aut)
{
    float transDens = 0;

    unsigned int numb_states = getNumbUsedStates(aut), numb_transitions = getNumbTransitions(aut), numb_symbols = getNumbSymbols(aut);

    if (numb_states == 0 || numb_symbols == 0)
        transDens = 0;
    else
        transDens = (float) numb_transitions / ((float) numb_states * (float) numb_symbols);

    return transDens;
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

/* Returns the number of symbols in the alphabet of the automaton.
 * This function appears to not always work correctly. */
unsigned int getNumbSymbols(const Automaton& aut)
{
    /*
    // Get a pointer to the alphabet of type onTheFlyAlphabet which is being used already
    // by 'aut'.
    std::shared_ptr<onTheFlyAlphabet> alph;
    if (!(alph = std::dynamic_pointer_cast<onTheFlyAlphabet>(aut.GetAlphabet())))
    {	// The alphabet should be ExplicitTreeAut::onTheFlyAlphabet,
        // if it isn't then throw 'false'.
        assert(false);
    }

    int c = 0;
    for (const auto stringSymbolPair : alph->GetSymbolDict())
        c++;

    return c;
    */

    unordered_set<symbol> symb;
    for (const transition& trans : aut)
        symb.insert(trans.GetSymbol());

    return symb.size();
}

vector<typerank> getRanks(const Automaton& aut, const unsigned int greatest_symbol) {
    vector<typerank> ranks(greatest_symbol);

    // Get a pointer to the alphabet of type onTheFlyAlphabet which is being used already
    // by 'aut'.
    std::shared_ptr<onTheFlyAlphabet> alph;
    if (!(alph = std::dynamic_pointer_cast<onTheFlyAlphabet>(aut.GetAlphabet())))
    {	// The alphabet should be ExplicitTreeAut::onTheFlyAlphabet,
        // if it isn't then throw 'false'.
        assert(false);
    }

    typerank rank;
    symbol s;
    for (auto stringSymbolPair : alph->GetSymbolDict())
    {
        s = stringSymbolPair.second;
        rank = stringSymbolPair.first.rank;
        if (rank==0) rank = 1;      /* Since we make the initial state explicit. */
        ranks[s] = rank;
    }

    return ranks;
}

typerank* getRanks2(const Automaton& aut, const unsigned int n) {
    typerank* ranks = new typerank[n];

    // Get a pointer to the alphabet of type onTheFlyAlphabet which is being used already
    // by 'aut'.
    std::shared_ptr<onTheFlyAlphabet> alph;
    if (!(alph = std::dynamic_pointer_cast<onTheFlyAlphabet>(aut.GetAlphabet())))
    {	// The alphabet should be ExplicitTreeAut::onTheFlyAlphabet,
        // if it isn't then throw 'false'.
        assert(false);
    }

    typerank rank;
    symbol s;
    for (auto stringSymbolPair : alph->GetSymbolDict())
    {
        s = stringSymbolPair.second;
        rank = stringSymbolPair.first.rank;
        if (rank==0) rank = 1;      /* Since we make the initial state explicit. */
        ranks[s] = rank;
    }

    return ranks;
}

float getAvgRank(const Automaton& aut) {
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

/* Returns a vector that at each index i contains a vector of pairs:
 * the first element is a transition (bottom-up) of aut in which i is a child and the second
 * element is the position at each i appears among the children of that transition. */
vector<vector<pair<transition,size_t>> > obtainTransBotUp(const Automaton& aut, unsigned int numbStates) {
    vector<vector<pair<transition,size_t>> > trans_botup(numbStates);

    for (const lv_transition trans : aut) {
        vector<state> children = trans.GetChildren();
        unsigned int size = children.size();

       //transition trans_ = std::make_tuple(trans.GetChildren(),trans.GetSymbol(),trans.GetParent());


                                       /*if (size==0) {
                                          trans_botup.at(initialState).push_back(std::make_pair(trans_,0));
                                                                      }*/


       for (unsigned int i=0; i<size; i++)
            trans_botup.at(children.at(i)).push_back(std::make_pair(trans,i));

    }

    return trans_botup;
}

bool equiv(const Automaton& aut1, const Automaton& aut2)
{
    Automaton aut1_ = removeInitialState(aut1);
    Automaton aut2_ = removeInitialState(aut2);

    //std::cout << "hello" << std::endl;
    //printAut(aut1_);
    //printAut(aut2_);

    bool result1 = true;
    result1 = VATA::ExplicitTreeAut::CheckInclusion(aut1_, aut2_);
    bool result2 = false;
    result2 = VATA::ExplicitTreeAut::CheckInclusion(aut2_, aut1_);

    //aut1_ = addInitialState(aut1);
    //aut2_ = addInitialState(aut2);

    return (result1 && result2);
}

bool langIsEmpty(const Automaton& aut)
{
    Automaton empty;
    return equiv(empty,aut);
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
unsigned int getNumbTransitions(const Automaton& aut)
{
    unsigned int c = 0;
    for (const lv_transition trans : aut)
        c++;

    return c;
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

/* Returns a copy of the given automaton but without any transition. */
Automaton copyAutWithoutTrans(const Automaton& old_aut){
    Automaton new_aut;

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

/*
my_transitions obtainTrans(Automaton& aut, unsigned int numb_states) {
    vector<tuple<vector<state>, symbol, state> > t;
    vector<vector<tuple<vector<state>, symbol, state> > > my_transitions (numb_states,t);

    tuple<vector<state>, symbol, state> my_transition;
    for (unsigned int i=0; i<numb_states; i++) {
        for (const lv_transition trans : aut[i]) {

                my_transition = std::make_tuple(trans.GetChildren(), trans.GetSymbol(), i);
            my_transitions.at(i).push_back(my_transition);
        }
    }

    return my_transitions;
}
*/

void setGreatestRank(const vector<typerank>& ranks) {
    GREATEST_RANK = *std::max_element(ranks.begin(),ranks.end());
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

bool foo(const vector<state>& vec1, const vector<state>& vec2)
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
    std::set<transition> transWhichOverlap;     // using a set forces that each transition shall be counted only once

//    for (Automaton::iterator it1=aut.begin(); it1!= aut.end(); ++it1)
//    {
//        bool shouldIinsertIt1 = false;
//        for (Automaton::iterator it2=it1; it2!=it1 && it2!=aut.end(); ++it2)
//        {
//            if ((*it1).GetSymbol()==(*it2).GetSymbol() && statesIntersect((*it1).GetChildren(), (*it2).GetChildren()))
//            {
//                transWhichOverlap.insert(*it2);
//                shouldIinsertIt1 = true;
//            }
//        }
//        if (shouldIinsertIt1)
//            transWhichOverlap.insert(*it1);
//    }
    for (const transition& trans1 : aut)
    {
        for (const transition& trans2 : aut)
        {
            if (trans2==trans1)
                continue;
            if (trans1.GetParent()==trans2.GetParent() && trans1.GetSymbol()==trans2.GetSymbol() && foo(trans1.GetChildren(), trans2.GetChildren()))
            {
                transWhichOverlap.insert(trans2);
            }
        }
    }
    unsigned int numb_transitions = getNumbTransitions(aut);
    float numb_transWhichOverlap = ((float) transWhichOverlap.size() / (float) numb_transitions) * (float) 100;

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
                //if ((*it1).GetSymbol()==(*it2).GetSymbol() && std::find((*it2).GetChildren().begin(), (*it2).GetChildren().end(), state) != (*it2).GetChildren().end())
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
                //if ((*it1).GetSymbol()==(*it2).GetSymbol() && std::find((*it2).GetChildren().begin(), (*it2).GetChildren().end(), state) != (*it2).GetChildren().end())
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

    vector<float> result = {numb_transWhichOverlap, avg1, avg2};

    return result;
}

state reindexStates(Automaton& aut)
{
    aut = removeInitialState(aut);

    state stateCnt = 0;
    stateToStateMap stateMap;
    stateToStateTranslWeak stateTrans(stateMap, [&stateCnt](const state&){return stateCnt++;});

    Automaton tmpAut = aut/*.RemoveUselessStates()*/;
    aut = tmpAut.ReindexStates(stateTrans);

    aut = addInitialState(aut);

    return /*USING_INITIAL_STATE ? stateCnt+1 : stateCnt*/ getNumbUsedStates(aut);
}

/* Returns the value of numbStates(smaller) / numbStates(larger) in %. */
float measureStatesReduction(const Automaton& smaller, const Automaton& larger)
{
    float states_red;

    if (getNumbUsedStates(larger) == 0)
        states_red = 100;
    else
        states_red = ((float) getNumbUsedStates(smaller)
                      / (float) getNumbUsedStates(larger)) * (float) 100;

    return states_red;
}

float measureTransitionsReduction(const Automaton& smaller, const Automaton& larger)
{
    float transitions_red;

    if (getNumbTransitions(larger) == 0)
        transitions_red = 100;
    else
        transitions_red = ((float) getNumbTransitions(smaller)
                           / (float) getNumbTransitions(larger)) * (float) 100;

    return transitions_red;
}

float measureTransDensReduction(const Automaton& smaller, const Automaton& larger)
{
    return (getTransitionDensity(smaller) / getTransitionDensity(larger)) * (float) 100;
}

void printAut(const Automaton& aut, stateDict* dict) {
    dict = NULL;        // I've given up printing fancy states names and considering whether the initial state is implicit or not.

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
    {		/* // For each transition, print first its external representation ...
                std::cout << "   " << translateSymbol(aut, trans.GetSymbol());

                // Create a vector of type string called 'childrenStr' with the same size
                // as the number of children in trans.
                std::vector<std::string> childrenStr(trans.GetChildren().size());
                // Now place each child in the vector and then perform an Haskell-like map
                // on this vector to replace each state by its external representation.
                std::transform(
                    trans.GetChildren().cbegin(),
                    trans.GetChildren().cend(),
                    childrenStr.begin(),
                    [&stateDict](const state s){return dict.TranslateBwd(s);}
                );

                std::cout << Convert::ToString(childrenStr);
                std::cout << " -> " << translateState(dict, trans.GetParent()); */

                // ... and then its internal one.
        std::cout << "    [parent state = " << trans.GetParent();
        std::cout << ", symbol = " << trans.GetSymbol();
        std::cout << ", children states = " << Convert::ToString(trans.GetChildren());
        std::cout << "]\n";
    }

    std::cout << std::flush;
}

string autToStringTimbuk(const Automaton& aut, stateDict* dict)
{
    dict = NULL;        // I've given up printing fancy states names and considering whether the initial state is implicit or not.
    string result;

    result = "Ops \n";
    result += "Automaton anonymous \n";

    stateSet states = getUsedStates(aut);
    result += "States ";
    for (const state& s : states)
    {
        //result += (dict==NULL ? "" : translateState(aut, *dict, s)) + " ";
        result += std::to_string(s) + " ";
    }
    result += "\n";

    finalStateSet f = aut.GetFinalStates();
    result += "Final States ";
    for (const state& s : f)
    {
        //result += (dict==NULL ? "" : translateState(aut, *dict, s)) + " ";
        result += std::to_string(s) + " ";
    }
    result += "\n";

    result += "Transitions \n";
    for (const lv_transition& trans : aut)
    {
        //result += trans.GetSymbol() + "(" + Convert::ToString(trans.GetChildren()) + ") -> " + translateState(aut, *dict, trans.GetParent()) + "\n";
        result += std::to_string(trans.GetSymbol()) + "(";
        /*for (const state& s : trans.GetChildren())
            result += translateState(aut, *dict, s);*/

        for(unsigned int i=0; i<trans.GetChildren().size(); i++)
        {
            //result += translateState(aut, *dict, trans.GetChildren().at(i));
            result += std::to_string(trans.GetChildren().at(i));
            if (i < trans.GetChildren().size()-1)
                result += ",";
        }
        //result += ") -> " + translateState(aut, *dict, trans.GetParent()) + "\n";
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
