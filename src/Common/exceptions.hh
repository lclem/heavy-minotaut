
#ifndef _EXCEPTIONS_HH_
#define _EXCEPTIONS_HH_

#include <exception>

using namespace std;

//extern static exception WHasChanged;

enum Suit { Diamonds = 1, Hearts, Clubs, Spades };

//extern static enum MY_ERRORS { ERR_NONE = 0, ERR_T1, ERR_T2 };

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
