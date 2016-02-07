
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Main file, which calls a user interface procedure.                          *
 * 																					*
 ************************************************************************************/

#include "../include/main.hh"
#include <boost/lambda/lambda.hpp>
#include <iterator>
#include <algorithm>

bool dbg = false;
/* After being declared in its header, a global variable must be defined in a different file.
 * Any files that then make use of the value of this variable must do an include of the first header. */
seconds TIMEOUT = 30*60;

void printHelp(char* argv[])
{
    std::cout << "\n"
              << "Please use one of the following: \n "
              << "\t - '" << argv[0] << " minimize [ options ] la-dw la-up i_dir1 i_dir2 ...' "
                    << "to apply Heavy(la-dw,la-up) to each automaton (with timbuk syntax) in the dirs provided. \n"
                    << "\t Options are as follows: \n"
                      << "\t\t - [-output=o_dir]: " << "if provided, each automaton from a file named ta.timbuk will "
                      << "be minimized and saved in the output_dir as a file named "
                      << "ta_minimized_with_Heavy(x,y).timbuk (default: no output). \n"
                      << "\t\t - [-max_numb_tests=n]: " << "to limit to n the number of automata to process in each i_dir. \n"
                      << "\t\t - [-timeout=t]: " << "to limit to t seconds each application of the Heavy algorithm (default: " << std::to_string(TIMEOUT) << "). \n"
                      << "\t\t - [-BranchPR=d]: " << "to set to d the depth of the pre-refinement performed before each computation of the lookahead downward simulation. (advanced) \n"
               << "\t - '" << argv[0] << " minimize_and_complement [ options ] la-dw la-up i_dir1 i_dir2 ...' "
                    << "to apply Heavy(la-dw,la-up) to each automaton (as described in the 'minimize' option) in the input dirs provided, "
                    << "and complement them using libvata's algorithm. For most automata, the minimization "
                    << "speeds up significantly the complement operation. \n"
                    << "\t Options are as follows: \n"
                      << "\t\t - [-output=o_dir]: " << "if provided, each automaton from a file named ta.timbuk will "
                      << "be minimized and complemented and saved in the output_dir as a file named "
                      << "ta_minimized_with_Heavy(x,y)_and_complemented.timbuk (default: no output). \n"
                      << "\t\t - [-max_numb_tests=n]: " << "to limit to n the number of automata to process in each i_dir. \n"
                      << "\t\t - [-timeout=t]: " << "to limit to t seconds each application of the Heavy algorithm (default: " << std::to_string(TIMEOUT) << "). \n"
                      << "\t\t - [-BranchPR=d]: " << "to set to d the depth of the pre-refinement performed before each computation of the lookahead downward simulation. (advanced) "
               << "\n" << "\n" ;
}

bool isOpt(string arg)
{
    bool result = arg.find('-') == 0;

    return result;
}

string getOptName(string arg)
{
    string optName = arg.substr(1, arg.find_first_of('=') - 1);
    return optName;
}

string getOptValue(string arg)
{
    string optValue = arg.substr(arg.find_first_of('=')+1);
    return optValue;
}

vector<string> mapSanitizeDirStrs(vector<string> dirs)
{
    for (unsigned int i=0; i<dirs.size(); i++)
        dirs[i] = sanitizeDirStr(dirs[i]);

    return dirs;
}

void set_HGA_strategy(string opt_value)
{
    if (opt_value == "none")
        setOPT(TYPE_OF_HISTORY_OF_GOOD_ATKS, NONE);
    else if (opt_value == "local")
        setOPT(TYPE_OF_HISTORY_OF_GOOD_ATKS, LOCAL);
    else if (opt_value == "semi-global")
        setOPT(TYPE_OF_HISTORY_OF_GOOD_ATKS, SEMI_GLOBAL);
    else if (opt_value == "global")
        setOPT(TYPE_OF_HISTORY_OF_GOOD_ATKS, GLOBAL);
    else
        exit_with_error("Error: wrong value provided for option 'HGA_strategy': '" + opt_value + "'. \n");
}

void set_HBA_strategy(string opt_value)
{
    if (opt_value == "none")
        setOPT(TYPE_OF_HISTORY_OF_BAD_ATKS, NONE);
    else if (opt_value == "local")
        setOPT(TYPE_OF_HISTORY_OF_BAD_ATKS, LOCAL);
    else if (opt_value == "semi-global")
        setOPT(TYPE_OF_HISTORY_OF_BAD_ATKS, SEMI_GLOBAL);
    else if (opt_value == "global")
        setOPT(TYPE_OF_HISTORY_OF_BAD_ATKS, GLOBAL);
    else
        exit_with_error("Error: wrong value provided for option 'HBA_strategy': '" + opt_value + "'. \n");
}

void setBranchPRdepth(string str)
{
    pr_depth = boost::lexical_cast<unsigned int>(str);
}

int parseMinimize(int argc, char* argv[], unsigned int pos)
{

    if (argc < 5)
    {
        printHelp(argv);
        return 0;
    }

    unsigned int max_numb_tests = 0;
    string output_dir = "";

    /* Parse opt args. */
    while (isOpt(argv[++pos]))
    {
        if (getOptName(argv[pos]) == "timeout")
            TIMEOUT = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "max_numb_tests")
            max_numb_tests = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "output")
            output_dir = sanitizeDirStr(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HGA_strategy")
            set_HGA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HBA_strategy")
            set_HBA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "BranchPR")
            setBranchPRdepth(getOptValue(argv[pos]));
        else
            exit_with_error("Error: Wrong option name given '" + getOptName(argv[pos]) + "'. \n");
    }
    /* At the end of the loop pos is left pointing at the first non-opt argument. */

    unsigned int la_dw = boost::lexical_cast<unsigned int>(argv[pos++]);
    unsigned int la_up = boost::lexical_cast<unsigned int>(argv[pos++]);

    vector<string> input_dirs = mapSanitizeDirStrs(convertCharPtArrayToStrVector(argv, pos, argc-1));

    string log_avg = "log_of_avg_values_of_heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up) + ")";
    /*
    log_avg += "using_hga=";
    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == NONE)
        log_avg += "none";
    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == SEMI_GLOBAL)
        log_avg += "semi-global";
    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL)
        log_avg += "local";
    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL)
        log_avg += "global";
    log_avg += "_and_hba=";
    if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == NONE)
        log_avg += "none";
    if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == SEMI_GLOBAL)
        log_avg += "semi-global";
    if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == LOCAL)
        log_avg += "local";
    if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL)
        log_avg += "global";
    log_avg += (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] != NONE || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] != NONE) ?
                 appendStrings("_on_", (areMapsOrdered ? "ordered" : "unordered"), "_maps")
              : "";
    log_avg += "_and_bpr=" + std::to_string(pr_depth);
    */
    log_avg += "_date:" + localTime() + ".txt";

    heavy_tester(la_dw, la_up, input_dirs, output_dir, max_numb_tests, log_avg);

    return 1;

}

int parseMinimizeAndComplement(int argc, char* argv[], unsigned int pos)
{

    if (argc < 5)
    {
        printHelp(argv);
        return 0;
    }

    unsigned int max_numb_tests = 0;
    string output_dir = "";

    /* Parse opt args. */
    while (isOpt(argv[++pos]))
    {
        if (getOptName(argv[pos]) == "timeout")
            TIMEOUT = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "max_numb_tests")
            max_numb_tests = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "output")
            output_dir = sanitizeDirStr(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HGA_strategy")
            set_HGA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HBA_strategy")
            set_HBA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "BranchPR")
            setBranchPRdepth(getOptValue(argv[pos]));
        else
            exit_with_error("Error: Wrong option name given '" + getOptName(argv[pos]) + "'. \n");
    }
    /* At the end of the loop pos is left pointing at the first non-opt argument. */

    unsigned int la_dw = boost::lexical_cast<unsigned int>(argv[pos++]);
    unsigned int la_up = boost::lexical_cast<unsigned int>(argv[pos++]);

    vector<string> input_dirs = mapSanitizeDirStrs(convertCharPtArrayToStrVector(argv, pos, argc-1));

    complement_after_heavy_tester(la_dw, la_up, input_dirs, output_dir, max_numb_tests);

    return 1;

}


int main(int argc, char* argv[])
{
    startOPT();

    if (argc > 1 && argv[1] == minimize_cmd)
    {
        return parseMinimize(argc, argv, 1);

    }

    if (argc > 1 && argv[1] == minimize_and_complement_cmd)
    {
        return parseMinimizeAndComplement(argc, argv, 1);

    }

    else
    {
        printHelp(argv);
        return 0;
    }

}
