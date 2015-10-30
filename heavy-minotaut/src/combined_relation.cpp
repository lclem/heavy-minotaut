
#include "combined_relation.hh"


vector<vector<bool> > combined_relation(const AutData& aut, const unsigned int la_dw, const unsigned int la_up,
                                        const unsigned int numb_states, const unsigned int greatest_symbol,
                                        timespec* timeout_start, unsigned int timeout)
{

    vector<vector<bool> > W_dw, W_up, W_composed, W_combined;

    float numb_ref_dw;
    W_dw = dw_simulation(aut, la_dw, &numb_ref_dw, true, timeout_start, timeout);

    float numb_ref_up;
    W_up = up_simulation(aut, la_up, W_dw, false, numb_states, greatest_symbol, &numb_ref_up, timeout_start, timeout);

    W_composed = createBoolMatrix(numb_states,numb_states,false);

    /* Computing the composition of the two relations. */
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

    /* Computing the combined relation. */
    for (unsigned int q=0; q<numb_states; q++)
        for (unsigned int r=0; r<numb_states; r++)
        {
            if (!W_composed.at(q).at(r))
                continue;

            for (unsigned int s=0; s<numb_states; s++)
                if (W_dw.at(r).at(s) && !W_composed.at(q).at(s))
                {
                    W_combined.at(q).at(r) = false;
                    continue;
                }
        }

    return W_combined;

}








//#include "combined_relation.hh"


//vector<vector<bool> > combined_relation(/*const Automaton& aut*/ const AutData& autData,
//                        const unsigned int la_dw, const unsigned int la_up,
//                        const unsigned int numb_states, const unsigned int numb_symbols,
//                        //const vector<typerank>& ranks,
//                        float* refinements,
//                        timespec* timeout_start, unsigned int timeout)
//{

//    vector<vector<bool> > W_dw, W_up, W_composed, W_combined;

//    float numb_ref_dw;
//    W_dw = dw_simulation(autData, la_dw, /*W_dw, numb_states,*/ /*ranks,*/ &numb_ref_dw, timeout_start, timeout);

//    float numb_ref_up;
//    W_up = up_simulation(autData, la_up, W_dw, false, numb_states, numb_symbols, /*ranks,*/ &numb_ref_up, timeout_start, timeout);

//    if (refinements != NULL)
//        *refinements = numb_ref_dw + numb_ref_up;

//    W_composed = createBoolMatrix(numb_states,numb_states,false);
//    //initializeW(aut, W_composed, numb_states, false, false);

//// Composed (the paper way)
//    for (unsigned int i=0; i<numb_states; i++)
//        for (unsigned int j=0; j<numb_states; j++)
//        {
//            if (!W_dw.at(i).at(j))
//                continue;

//            for (unsigned int k=0; k<numb_states; k++)
//                if (W_up.at(k).at(j))
//                    W_composed.at(i).at(k) = true;

//        }

//    // Composed (My Way)
////    for (unsigned int i=0; i<numb_states; i++)
////        for (unsigned int j=0; j<numb_states; j++)
////            for (unsigned int k=0; k<numb_states; k++)
////                if (W_dw.at(i).at(k) && W_up.at(j).at(k))
////                    W_composed.at(i).at(j) = true;

//    W_combined = W_composed;

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

//    // Combined (my and Lukas way)
////    for (unsigned int q=0; q<numb_states; q++)
////       for (unsigned int r=0; r<numb_states; r++)
////       {
////           if (!W_composed.at(q).at(r))
////               continue;

////           for (unsigned int s=0; s<numb_states; s++)
////                if (W_dw.at(r).at(s) && !W_composed.at(q).at(s))
////                {
////                    W_combined.at(q).at(r) = false;
////                    continue;
////                }
////       }


////    for (unsigned int x=0; x<numb_states; x++)
////        for (unsigned int y=0; y<numb_states; y++)
////        {
////            if (!W_composed.at(x).at(y)) continue;

////            for (unsigned int z=0; z<numb_states; z++)
////                if (W_dw.at(y).at(z))
////                    if (!W_composed.at(x).at(z))
////                        W_combined.at(x).at(y) = false;

////        }

//  //  printW(aut, W_combined,numb_states);

//    printAut(getAut(autData));

//    outputText(" - D is: ");
//    printW(getAut(autData), W_dw);

//    outputText("\n - U is: ");
//    printW(getAut(autData), W_up);

//    outputText("\n - The composed relation is: ");
//    printW(getAut(autData), W_composed);

//    outputText("\n - The combined preorder is: ");
//    printW(getAut(autData), W_combined);

//    return W_combined;
//}
