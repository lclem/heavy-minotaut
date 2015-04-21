#ifndef STATISTICALRESULTS_H
#define STATISTICALRESULTS_H

#include "../src/Common/common.hh"
#include "common.hh"


/* Records in objects of this class are relative to tests that ended Successfully. - To be deleted later. */
class Success
{
    public:
        unsigned int total = 0;
        float initial_avg_q = 0;    // The average number of states in the automata prior to any reduction.
        float initial_avg_delta = 0;    // The average number of transitions in the automata prior to any reduction.
        float initial_avg_sigma = 0;
        float initial_avg_ranking = 0;
        float initial_avg_dens = 0;     // The average transition density in the automata prior to any reduction.
        unsigned int initial_greatest_q = 0;    // The average number of states in the automata prior to any reduction.
        unsigned int initial_greatest_delta = 0;    // The average number of transitions in the automata prior to any reduction.
        float initial_greatest_sigma = 0;
        float initial_greatest_ranking = 0;
        float initial_greatest_dens = 0;     // The average transition density in the automata prior to any reduction.
        vector<float> avg_sizes_relations;
        vector<float> avg_times_relations;
        vector<float> avg_times_quotientings;
        vector<float> avg_times_prunings;
        float avg_time_consCheck = 0;       // How long it took to check if the reduced aut was equivalent to the initial one.
        vector<float> avg_numbs_refinements;     // Number of matrix-refinements necessary to compute the relation. E.g., 1.5 means a loop and a half in the matrix.
        Success(unsigned int numb_relations, unsigned int numb_quotientings, unsigned int numb_prunings);
        void inc();
        void updateInitialAvg(unsigned int q, unsigned int delta, unsigned int sigma, float ranking, float dens);
        void checkInitialGreatest(unsigned int q, unsigned int delta, unsigned int sigma, float ranking, float dens);
        void updateAvgSizesRels(vector<unsigned int> cards);
        void updateAvgNumbsRefs(vector<float> numbs_refs);
        void updateAvgTimes(vector<float> times_relations, vector<float> times_quotientings, vector<float> times_prunings, float time_consCheck);
};

/* Objects of this class store general data of the experiments, as measures of the automata given from input. */
class MetaData
{
    public:
        unsigned int total = 0;
        float initial_avg_q = 0;        // The average number of states in the automata prior to any reduction.
        float initial_avg_delta = 0;    // The average number of transitions in the automata prior to any reduction.
        float initial_avg_sigma = 0;
        float initial_avg_ranking = 0;
        float initial_avg_dens = 0;     // The average transition density in the automata prior to any reduction.
        unsigned int initial_greatest_q = 0;    // The average number of states in the automata prior to any reduction.
        unsigned int initial_greatest_delta = 0;    // The average number of transitions in the automata prior to any reduction.
        float initial_greatest_sigma = 0;
        float initial_greatest_ranking = 0;
        float initial_greatest_dens = 0;     // The average transition density in the automata prior to any reduction.
        vector<float> initial_avg_transOverlap;
        vector<float> initial_greatest_transOverlap;
        MetaData();
        void inc();
        void updateInitialAvg(unsigned int q, unsigned int delta, unsigned int sigma, float avg_ranking, float dens, vector<float> transOverlap);
        void checkInitialGreatest(unsigned int q, unsigned int delta, unsigned int sigma, float ranking, float dens, vector<float> transOverlap);
};

/* An object of this class stores data relative to a specific kind of test. */
class TestData
{
    public:
        unsigned int total = 0;
        unsigned int numb_relations = 0;
        float VERY_LARGE_NUMB = 999999;     /* Must be defined as public and before the declarations that make use of it. */
        float greatest_q_reduction = VERY_LARGE_NUMB;
        float greatest_delta_reduction = VERY_LARGE_NUMB;
        float greatest_transDens_reduction = VERY_LARGE_NUMB;
        float avg_q_reduction = 0;              // on average, how many of the states (%) are still present in the automata after the reduction
        float avg_delta_reduction = 0;          // on average, how many of the transitions (%) are still present in the automata after the reduction
        float avg_transDens_reduction = 0;          // on average, how does the transitions density before the reduction compare to that after it.
        vector<float> avg_sizes_relations;
        vector<float> avg_times_relations;
        vector<float> avg_times_quotientings;
        vector<float> avg_times_prunings;
        float avg_time_consCheck = 0;       // How long it took to check if the reduced aut was equivalent to the initial one.
        vector<float> avg_numbs_refinements;     // Number of matrix-refinements necessary to compute the relation. E.g., 1.5 means a loop and a half in the matrix.
        vector<unsigned int> q_reduction_buckets;
        vector<unsigned int> delta_reduction_buckets;
        vector<unsigned int> transDens_reduction_buckets;

        TestData();
        TestData(unsigned int numb_relations, unsigned int numb_quotientings, unsigned int numb_prunings);
        void inc();
        void set(unsigned int numb_relations, unsigned int numb_quotientings, unsigned int numb_prunings);
        void updateAvgSizesRels(vector<unsigned int> cards);
        void updateAvgNumbsRefs(vector<float> numbs_refs);
        void updateAvgTimes(vector<float> times_relations, vector<float> times_quotientings, vector<float> times_prunings, float time_consCheck);
        void updateAvgTimes(vector<float> times_relations, vector<float> times_quotientings, vector<float> times_prunings);
        void checkGreatestReductions(float q_red, float delta_red, float transDens_red);
        void updateAvgReductions(float q_red, float delta_red, float transDens_red);
        void updateReductionBuckets(float q_red, float delta_red, float transDens_red);
        void printReductionBuckets(const string filename);
        bool isEmpty(const TestData& testData);
};


/* Records in objects of this class are relative to tests that ended Successfully and where automata reduction
 * efectively occured. */
class Reduction
{
    public:

        unsigned int total = 0;
        unsigned int VERY_LARGE_NUMB = 999999;
        float greatest_q_reduction = VERY_LARGE_NUMB;
        float greatest_delta_reduction = VERY_LARGE_NUMB;
        float greatest_transDens_reduction = VERY_LARGE_NUMB;
        float avg_q_reduction = 0;              // on average, how many of the states (%) are still present in the automata after the reduction
        float avg_delta_reduction = 0;          // on average, how many of the transitions (%) are still present in the automata after the reduction
        float avg_transDens_reduction = 0;

        void inc();
        void checkGreatest(float q_red, float delta_red);
        void checkGreatest(float q_red, float delta_red, float transDens_red);
        void updateAvg(float q_red, float delta_red);
        void updateAvg(float q_red, float delta_red, float transDens_red);
};

/* Records in objects of this class are relative to tests that were aborted due to a timeout. */
class Timeout
{
    public:

        unsigned int total = 0;
        unsigned int VERY_LARGE_NUMB = 999999;
        unsigned int smallest_q = VERY_LARGE_NUMB;
        unsigned int smallest_delta = VERY_LARGE_NUMB;
        float smallest_dens = VERY_LARGE_NUMB;

        void inc();
        void checkSmallest(unsigned int q, unsigned int delta, float dens);
};


class QueriesCounter
{
    public:
        unsigned int numb_inserts = 0;
        unsigned int numb_sucLookups = 0;
        unsigned int numb_unsucLookups = 0;

        void incNumbInserts();
        void incNumbSucLookups();
        void incNumbUnsucLookups();
};

extern MetaData emptyMetaData;
extern TestData emptyTestData;
extern Timeout emptyTimeout;


#endif // STATISTICALRESULTS_H
