
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		A file that includes the exceptions header file.                            *
 * 																					*
 ************************************************************************************/

#include "exceptions.hh"

void check_timeout(const Automaton& aut, timespec timeout_start, unsigned int timeout)
{
    unsigned int timeout_temp = timeout==0 ? TIMEOUT : timeout;

    timespec nowTime;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &nowTime);

    if (nowTime.tv_sec - timeout_start.tv_sec > timeout_temp)
    {
        // outputText(std::to_string(nowTime.tv_sec - timeout_start.tv_sec) + " have passed -> TIMEOUT. "); Uncomment to help fine tune the timeout interruption.
        throw timeout_(aut);
    }

}
