
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file containing definitions of all the exceptions used in this       *
 *  project.                                                                        *
 * 																					*
 ************************************************************************************/

#ifndef _EXCEPTIONS_HH_
#define _EXCEPTIONS_HH_

#include <exception>

using namespace std;


struct autHasNoLeafRules : public exception
{
};

struct wHasChanged : public exception
{  
};

struct finished : public exception
{
};

struct timeout_ : public exception
{
};

#endif
