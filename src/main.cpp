
#include "../include/main.hh"


bool dbg = false;		/* After being declared in its header, a global variable must be defined in a different file. Any files that then make use of the value of this variable must do an include of the first header. */
long int TIMEOUT = 10*60;    // seconds

int main(int argc, char* argv[])
{
    startOPT();
    simple_Heavy_tester(1, 1, "test automata/random_tabakov_vardi_timbuk/samples of 50 TA each/1<=td<=7 and 0.1<=ad<=0.9/tv_q=100_sigma=2_td=1.0_ad=0.1//");

    if (argc == 0)
    {
        minimizationTechniques_tester();
        return 1;
    }

    if (argc < 4)
    {
        std::cout << "Please use '"
                  << argv[0] << " la-dw la-up dir [timeout]' to apply Heavy(la-dw,la-up) to each automaton "
                  << "(with timbuk syntax) saved in a file in directory dir. "
                  << "The timeout parameter is optional and must be in seconds. "
                  << "Each automaton in a file ta.timbuk will be minimized and saved in the file ta_minimized_with_Heavy(x,y).timbuk.\n";
        return 0;
    }
    else
    {
        unsigned int la_dw = boost::lexical_cast<unsigned int>(argv[1]);
        unsigned int la_up = boost::lexical_cast<unsigned int>(argv[2]);
        if (argc == 5)
            TIMEOUT = (boost::lexical_cast<long int>(argv[4]));
        string dir = argv[3];
        dir += "/";

        simple_Heavy_tester(la_dw, la_up, dir);

        return 1;
    }
}



/*
        // Test 1 - Quotienting with dw-2 simulation and Pruning with P(id, dw-2 sim).
        string filename = "./test automata//forester_redblack_timbuk//32843200//B32843200_139820680990389";
        //string filename = "./test automata/small automata/testing downward simulation/la=2/2.txt";
        string autStr = VATA::Util::ReadFile(filename);

        //Create the dictionary for translating state names to internal state numbers.
        stateDict stateDict;
        pair<Automaton,unsigned int> parsing_pair = parseFromString(autStr,stateDict,false,false);
        Automaton aut = parsing_pair.first;
        const unsigned int numb_states = parsing_pair.second;   // What about the unreachable states?
        
        printAut(aut,stateDict,true);

        vector<bool> column(numb_states);
        vector<vector<bool> > dw_sim(numb_states,column);
        dw_sim = dw_simulation(aut,2,dw_sim,numb_states);
        std::cout << "The dw-2 simulation is ";
        printW(dw_sim,numb_states,stateDict);

        Automaton aut_smaller = quotientAutomaton(aut,transClosure(dw_sim,numb_states),numb_states);
        std::cout << "\nThe quotiented automaton is:\n";
        printAut(aut_smaller,stateDict);
        const unsigned int numb_states_smaller = getNumbUsedStates(aut_smaller);
        const float ratio = (float) numb_states_smaller / (float) numb_states;

        bool result1 = VATA::ExplicitTreeAut::CheckInclusion(aut_smaller, aut);
        bool result2 = VATA::ExplicitTreeAut::CheckInclusion(aut, aut_smaller);

       if (result1 && result2)
           std::cout << "The quotienting preserved the language and the quotiented automaton has "
                     << std::to_string(ratio) << " of the states of the original automaton\n";
       else
           std::cout << "The quotienting did NOT preserve the language!\n";

       Automaton aut_pruned = prune(aut,generateIdRelation(numb_states),dw_sim,true);
       aut_pruned.RemoveUnreachableStates();   // There are potentially still a lot of states which do not reach any final state.
       std::cout << "\nThe pruned automaton is:";
       printAut(aut_pruned,stateDict);
*/

/*
    // Test 2 - Pruning with P(up simulation parameterized with dw simulation, id).
    string filename = "./test automata/small automata/testing upward simulation/1.txt";
    string autStr2 = VATA::Util::ReadFile(filename);

    //Create the dictionary for translating state names to internal state numbers.
    stateDict stateDict;
    tuple<Automaton,unsigned int,unsigned int> parsing_tuple = parseFromString(autStr2,stateDict,false,false);
    Automaton aut = std::get<0>(parsing_tuple);
    const unsigned int numb_states = std::get<1>(parsing_tuple);   // What about the unreachable states?
    //aut.RemoveUselessStates();  //Warning: there is the assumption that final states are never useless, so this never deletes them

    printAut(aut,stateDict,true);

    vector<bool> column(numb_states);
    vector<vector<bool> > dw_sim(numb_states,column);
    vector<vector<bool> > up_sim_strict(numb_states,column);
    dw_sim = (dw_simulation(aut,2,dw_sim,numb_states)).first;

    up_sim_strict = up_simulation_strict(aut,2,dw_sim,true,up_sim_strict,numb_states);
    std::cout << "\nThe computed simulation is: ";
    printW(up_sim_strict,numb_states,stateDict);
    Automaton aut_pruned = prune(aut,up_sim_strict,generateIdRelation(numb_states),false);
    //aut_pruned.RemoveUnreachableStates();   // There are potentially still a lot of states which do not reach any final state.
    std::cout << "\nThe pruned automaton is:";
    printAut(aut_pruned,stateDict,true);
*/


