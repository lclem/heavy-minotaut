
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Main file, which calls a user interface procedure.                          *
 * 																					*
 ************************************************************************************/

#include "../include/main.hh"

bool dbg = false;
/* After being declared in its header, a global variable must be defined in a different file.
 * Any files that then make use of the value of this variable must do an include of the first header. */
long int TIMEOUT = 10*60;    // seconds

int main(int argc, char* argv[])
{
    startOPT();

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
