/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2012  Jiri Simacek <isimacek@fit.vutbr.cz>
 *
 *  Description:
 *    Source file for an explicit downward inclusion.
 *
 *****************************************************************************/

// Standard library headers
#include <set>
#include <algorithm>

// VATA headers
#include <vata/vata.hh>
#include <vata/explicit_tree_aut.hh>
#include <vata/util/antichain1c.hh>
#include <vata/util/antichain2c_v2.hh>

#include "explicit_tree_incl_down.hh"
#include "util/cache.hh"
#include "util/cached_binary_op.hh"
#include "util/caching_allocator.hh"

// TODO: a lot in common with src/explicit_tree_incl_up.cc

namespace
{	// anonymous namespace

using StateDiscontBinaryRelation = VATA::ExplicitTreeAutCore::StateDiscontBinaryRelation;

template <class T1, class T2>
bool checkIntersection(const T1& s1, const T2& s2)
{
	auto i1 = s1.begin(), i2 = s2.begin();

	while (i1 != s1.end() && i2 != s2.end())
	{
		if (*i1 < *i2) ++i1;
		else if (*i2 < *i1) ++i2;
		else return true;
	}

	return false;
}

template <class T1, class T2>
void intersectionByLookup(T1& d, const T2& s)
{
	for (auto i = d.begin(); i != d.end(); )
	{
		if (s.count(*i) == 0)
			i = d.erase(i);
		else
			++i;
	}
}

typedef VATA::ExplicitTreeAutCore::StateType SmallerType;
typedef std::vector<VATA::ExplicitTreeAutCore::StateType> StateSet;

typedef size_t SymbolType;

typedef typename VATA::Util::Cache<StateSet> BiggerTypeCache;

typedef typename BiggerTypeCache::TPtr BiggerType;

typedef typename VATA::Util::Antichain1C<SmallerType> Antichain1C;
typedef typename VATA::Util::Antichain2Cv2<SmallerType, BiggerType> Antichain2C;

typedef std::pair<SmallerType, BiggerType> SmallerBiggerPair;
typedef std::pair<SmallerType, Antichain2C::TList::iterator> SmallerBiggerPairAC;

typedef VATA::ExplicitTreeAutCore::StateTuple StateTuple;

typedef VATA::ExplicitDownwardInclusion::DoubleIndexedTupleList DoubleIndexedTupleList;

struct less
{
	bool operator()(
		const SmallerBiggerPair&        p1,
		const SmallerBiggerPair&        p2) const
	{

		if (p1.second->size() < p2.second->size()) return true;
		if (p1.second->size() > p2.second->size()) return false;
		if (p1.first < p2.first) return true;
		if (p1.first > p2.first) return false;
		return p1.second.get() < p2.second.get();
	}
};

typedef std::set<SmallerBiggerPair, less> OrderedType;

class ChoiceFunction
{
	std::vector<size_t> data_;
	size_t arity_;

public:

	ChoiceFunction() : data_(), arity_() {}

	void swap(ChoiceFunction& rhs)
	{
		std::swap(data_, rhs.data_);
		std::swap(arity_, rhs.arity_);
	}

	void init(size_t size, size_t arity)
	{
		data_ = std::vector<size_t>(size, 0);
		arity_ = arity;
	}

	bool next()
	{
		// move to the next choice function
		size_t index = 0;

		while (++data_[index] == arity_)
		{
			data_[index] = 0; // reset this counter

			++index;                // move to the next counter

			if (data_.size() == index)
			{
				// if we drop out from the n-tuple
				return false;
			}
		}

		return true;
	}

	const size_t& operator[](size_t index)
	{
		assert(index < data_.size());

		return data_[index];
	}

	size_t size() const
	{
		return data_.size();
	}

	const size_t& arity() const
	{
		return arity_;
	}
};

struct ExpandStackFrame
{
	ExpandStackFrame* parent;
	size_t retAddr;
	SmallerType p_S;
	BiggerType P_B;
	size_t a;
	std::vector<const StateTuple*>::const_iterator tupleSetIter;
	std::vector<const StateTuple*>::const_iterator tupleSetIter2;
	size_t i;
	std::vector<size_t>::const_iterator sIter;
	Antichain2C::TList::iterator worksetIter;
	std::vector<const StateTuple*> W;
	ChoiceFunction choiceFunction;
	Antichain2C childrenCache;

	ExpandStackFrame() :
		parent(),
		retAddr(),
		p_S(),
		P_B(),
		a(),
		tupleSetIter(),
		tupleSetIter2(),
		i(),
		sIter(),
		worksetIter(),
		W(),
		choiceFunction(),
		childrenCache()
	{ }

private:

	ExpandStackFrame(const ExpandStackFrame&);
	ExpandStackFrame& operator=(const ExpandStackFrame&);
};

class ExpandCallEmulator
{
	VATA::Util::CachingAllocator<ExpandStackFrame> allocator_;
	ExpandStackFrame* ptr_;

public:

	ExpandCallEmulator() : allocator_(), ptr_() { }

	void push(ExpandStackFrame& top)
	{
		ExpandStackFrame* newFrame = allocator_();

		newFrame->parent = ptr_;
		newFrame->retAddr = top.retAddr;
		newFrame->p_S = top.p_S;
		newFrame->P_B = top.P_B;
		newFrame->a = top.a;
		newFrame->tupleSetIter = top.tupleSetIter;
		newFrame->tupleSetIter2 = top.tupleSetIter2;
		newFrame->i = top.i;
		newFrame->sIter = top.sIter;
		newFrame->worksetIter = top.worksetIter;

		std::swap(newFrame->W, top.W);
		std::swap(newFrame->choiceFunction, top.choiceFunction);
		std::swap(newFrame->childrenCache, top.childrenCache);

		ptr_ = newFrame;
	}

	void pop(ExpandStackFrame& top)
	{
		top.retAddr = ptr_->retAddr;
		top.p_S = ptr_->p_S;
		top.P_B = ptr_->P_B;
		top.a = ptr_->a;
		top.tupleSetIter = ptr_->tupleSetIter;
		top.tupleSetIter2 = ptr_->tupleSetIter2;
		top.i = ptr_->i;
		top.sIter = ptr_->sIter;
		top.worksetIter = ptr_->worksetIter;

		std::swap(top.W, ptr_->W);
		std::swap(top.choiceFunction, ptr_->choiceFunction);
		std::swap(top.childrenCache, ptr_->childrenCache);

		allocator_.reclaim(ptr_);
		ptr_ = ptr_->parent;
	}

	bool empty() const
	{
		return (nullptr == ptr_);
	}

private:

	ExpandCallEmulator(const ExpandCallEmulator&);
	ExpandCallEmulator& operator=(const ExpandCallEmulator&);
};

#define EXPAND_CALL(ret)\
	retAddr = ret;\
	goto _call;

#define EXPAND_PUSH\
	callEmulator.push(top);\
	top.p_S = r_i;\
	top.P_B = S;\
	top.retAddr = retAddr;\
	top.worksetIter = workset.insert(r_i, S);

#define EXPAND_RETURN\
	switch (retAddr) {\
		case 0: goto _end;\
		case 1: goto _stdret;\
		case 2: goto _simret;\
	}

#define EXPAND_POP_RETURN\
	workset.remove(top.p_S, top.worksetIter);\
	retAddr = top.retAddr;\
	S = top.P_B;\
	r_i = top.p_S;\
	callEmulator.pop(top);\
	EXPAND_RETURN

inline bool expand(
	BiggerTypeCache&                                biggerTypeCache,
	VATA::Util::CachedBinaryOp<const StateSet*,
		const StateSet*, bool>&                       lteCache,
	Antichain2C&                                    nonincluded,
	const SmallerType&                              p_S,
	const BiggerType&                               P_B,
	const DoubleIndexedTupleList&                   smallerIndex,
	const DoubleIndexedTupleList&                   biggerIndex,
	const StateDiscontBinaryRelation::IndexType&    ind,
	const StateDiscontBinaryRelation::IndexType&    inv)
{
	auto noncachedLte = [&ind](const StateSet* x, const StateSet* y) -> bool
	{
		assert(x); assert(y);

		for (auto& s1 : *x)
		{
			assert(s1 < ind.size());

			if (!checkIntersection(ind.at(s1), *y))
				return false;
		}

		return true;
	};

	auto lte = [&noncachedLte, &lteCache](const BiggerType& x, const BiggerType& y) -> bool
	{
		assert(x); assert(y);

		return (x.get() == y.get())?(true):(lteCache.lookup(x.get(), y.get(), noncachedLte));
	};

	auto gte = [&lte](const BiggerType& x, const BiggerType& y) { return lte(y, x); };

	Antichain2C workset;

	ExpandStackFrame top;

	ExpandCallEmulator callEmulator;

	const std::vector<const StateTuple*>* smallerTupleSet = nullptr;

	std::unordered_set<const StateTuple*> tupleSet;

	Antichain1C post;

	StateSet tmp;

	SmallerType r_i = p_S;

	BiggerType S = P_B;

	size_t retAddr = 0;

	bool found = false; // return value of simulated calls
_call:
	if (smallerIndex.size() <= r_i)
	{
		found = true;

		EXPAND_RETURN
	}

	if (checkIntersection(ind.at(r_i), *S))
	{
		found = true;

		EXPAND_RETURN
	}

	assert(r_i < ind.size());

	if (workset.contains(ind.at(r_i), S, lte))
	{
		found = true;

		EXPAND_RETURN
	}

	assert(r_i < inv.size());

	if (nonincluded.contains(inv.at(r_i), S, gte))
	{
		found = false;

		EXPAND_RETURN
	}

	EXPAND_PUSH

	assert(r_i < smallerIndex.size());

	top.childrenCache.clear();

	for (top.a = 0; top.a < smallerIndex[top.p_S].size(); ++top.a)
	{
		smallerTupleSet = &smallerIndex[top.p_S][top.a];

		if (smallerTupleSet->empty())
			continue;

		if (/* arity */ smallerTupleSet->front()->size() == 0)
		{
			StateSet::const_iterator i;

			for (i = top.P_B->begin(); i != top.P_B->end(); ++i)
			{
				if (*i < biggerIndex.size() && biggerIndex[*i].size())
					break;
			}

			if (i == top.P_B->end())
			{
				found = false;

				EXPAND_POP_RETURN
			}

			continue;
		}

		top.W.clear();

		tupleSet.clear();

		for (auto& state : *top.P_B)
		{
			if (biggerIndex.size() <= state)
				continue;

			auto& biggerCluster = biggerIndex[state];

			if (biggerCluster.size() <= top.a)
				continue;

			for (auto& tuple : biggerCluster[top.a])
			{
				if (tupleSet.insert(tuple).second)
					top.W.push_back(tuple);
			}
		}

		if (top.W.empty())
		{
			found = false;

			EXPAND_POP_RETURN
		}

		for (top.tupleSetIter = smallerTupleSet->begin(); top.tupleSetIter != smallerTupleSet->end(); ++top.tupleSetIter)
		{
			for (top.tupleSetIter2 = top.W.begin(); top.tupleSetIter2 != top.W.end(); ++top.tupleSetIter2)
			{
				assert((**top.tupleSetIter).size() == (**top.tupleSetIter2).size());

				for (top.i = 0; top.i < (**top.tupleSetIter).size(); ++top.i)
				{
					r_i = (**top.tupleSetIter)[top.i];

					S = biggerTypeCache.lookup({ (**top.tupleSetIter2)[top.i] });

					EXPAND_CALL(2)
_simret:
					if (!found)
						break;
				}

				if (found)
					goto _nexttuple;
			}

			top.choiceFunction.init(top.W.size(), /* arity */ (**top.tupleSetIter).size());

			do
			{
				// we loop for each choice function
				found = false;

				for (top.i = 0; top.i < top.choiceFunction.arity(); ++top.i)
				{
					// for each position of the n-tuple
					post.clear();

					for (size_t j = 0; j < top.choiceFunction.size(); ++j)
					{
						if (top.choiceFunction[j] != top.i)
							continue;

						// in case the choice function for given vector is i
						r_i = (*top.W[j])[top.i];

						assert(r_i < ind.size());

						if (post.contains(ind.at(r_i)))
							continue;

						assert(r_i < inv.size());

						post.refine(inv.at(r_i));
						post.insert(r_i);
					}

					if (post.data().empty())
						continue;

					tmp = StateSet(post.data().begin(), post.data().end());

					std::sort(tmp.begin(), tmp.end());

					assert((**top.tupleSetIter).size() == top.choiceFunction.arity());

					r_i = (**top.tupleSetIter)[top.i];

					S = biggerTypeCache.lookup(tmp);

					if (top.childrenCache.contains(ind.at(r_i), S, lte))
						goto _nextchoice;

					EXPAND_CALL(1)
_stdret:
					if (found)
					{
						top.childrenCache.refine(inv.at(r_i), S, gte);
						top.childrenCache.insert(r_i, S);

						goto _nextchoice;
					}

					if (!nonincluded.contains(inv.at(r_i), S, gte))
					{
						nonincluded.refine(ind.at(r_i), S, lte);
						nonincluded.insert(r_i, S);
					}
				}

				EXPAND_POP_RETURN
_nextchoice:;
			} while (top.choiceFunction.next());
_nexttuple:
			assert(top.p_S < smallerIndex.size());
			assert(top.a < smallerIndex[top.p_S].size());

			smallerTupleSet = &smallerIndex[top.p_S][top.a];
		}
	}

	found = true;

	EXPAND_POP_RETURN
_end:
	assert(callEmulator.empty());

	return found;
}
} // namespace


bool VATA::ExplicitDownwardInclusion::checkInternal(
	const DoubleIndexedTupleList&                 smallerIndex,
	const ExplicitTreeAutCore::FinalStateSet&     smallerFinalStates,
	const DoubleIndexedTupleList&                 biggerIndex,
	const ExplicitTreeAutCore::FinalStateSet&     biggerFinalStates,
	const StateDiscontBinaryRelation::IndexType&  ind,
	const StateDiscontBinaryRelation::IndexType&  inv)
{
	Util::CachedBinaryOp<const StateSet*, const StateSet*, bool> lteCache;

	BiggerTypeCache biggerTypeCache(
		[&lteCache](const StateSet* v)
		{
			lteCache.invalidateFirst(v);
			lteCache.invalidateSecond(v);
		}
	);

	Antichain2C nonincluded;

	std::vector<size_t> v(biggerFinalStates.begin(), biggerFinalStates.end());

	std::sort(v.begin(), v.end());

	auto biggerF = biggerTypeCache.lookup(v);

	for (auto& f : smallerFinalStates)
	{
		if (!expand(biggerTypeCache, lteCache, nonincluded, f, biggerF, smallerIndex, biggerIndex, ind, inv))
			return false;
	}

	return true;
}
