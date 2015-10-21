
#include "combined_preorder.hh"


float combined_preorder(const Automaton& aut, vector<vector<bool> >& W_combined,
                        const unsigned int la_dw, const unsigned int la_up,
                        const unsigned int numb_states, const unsigned int numb_symbols,
                        const vector<typerank>& ranks,
                        timespec* timeout_start, unsigned int timeout)
{

    vector<vector<bool> > W_dw, W_up, W_composed;
    initializeW(aut, W_composed, numb_states, false, false);

    float numb_ref_dw =
            dw_simulation(aut, la_dw, W_dw, numb_states, ranks, timeout_start, timeout);

    float numb_ref_up =
            up_simulation(aut, la_up, W_dw, false, W_up, numb_states, numb_symbols, ranks, timeout_start, timeout);

    for (unsigned int i=0; i<numb_states; i++)
        for (unsigned int j=0; j<numb_states; j++)
        {
            if (!W_dw.at(i).at(j))
                continue;

            for (unsigned int k=0; k<numb_states; k++)
                if (W_up.at(k).at(j))
                    W_composed.at(i).at(k) = true;

        }

    W_combined = W_composed;

//    for (unsigned int y=0; y<numb_states; y++)
//        for (unsigned int z=0; z<numb_states; z++)
//        {
//            if (!W_dw.at(y).at(z))
//                continue;

//            for (unsigned int x=0; x<numb_states; x++)
//            {
//                if (!W_composed.at(x).at(y)) continue;

//                if (W_up.at(x).at(z) && !W_composed.at(x).at(z))
//                    W_combined.at(x).at(y) = false;
//            }
//        }

    for (unsigned int q=0; q<numb_states; q++)
       for (unsigned int r=0; r<numb_states && W_composed.at(q).at(r); r++)
          for (unsigned int s=0; s<numb_states && W_dw.at(r).at(s); s++)
             if (!W_composed.at(q).at(s))
             {
                 W_combined.at(q).at(r) = false;
                 continue;
             }

//    for (unsigned int x=0; x<numb_states; x++)
//        for (unsigned int y=0; y<numb_states; y++)
//        {
//            if (!W_composed.at(x).at(y)) continue;

//            for (unsigned int z=0; z<numb_states; z++)
//                if (W_dw.at(y).at(z))
//                    if (!W_composed.at(x).at(z))
//                        W_combined.at(x).at(y) = false;

//        }

  //  printW(aut, W_combined,numb_states);

    return numb_ref_dw + numb_ref_up;
}
