/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2011  Jiri Simacek <isimacek@fit.vutbr.cz>
 *
 *  Description:
 *    Implemention of simulation computation for explicitly represented tree
 *    automata.
 *
 *****************************************************************************/

#ifndef _VATA_EXPLICIT_TREE_SIM_HH_
#define _VATA_EXPLICIT_TREE_SIM_HH_

#include "explicit_tree_transl.hh"

// template <class Index>
// VATA::AutBase::StateBinaryRelation VATA::ExplicitTreeAutCore::ComputeDownwardSimulation(
// 	size_t            size,
// 	const Index&      index) const
// {
// 	VATA::AutBase::StateBinaryRelation sim =
// 		this->TranslateDownward(index).computeSimulation(size);
//
// 	return sim;
// }


// template <class Index>
// VATA::AutBase::StateBinaryRelation VATA::ExplicitTreeAutCore::ComputeUpwardSimulation(
// 	size_t                   size,
// 	const Index&             index) const
// {
// 	std::vector<std::vector<size_t>> partition;
//
// 	AutBase::StateBinaryRelation relation;
//
// 	return TranslateUpward(
// 		*this, partition, relation, Util::Identity(size), index
// 	).computeSimulation(partition, relation, size);
// }

#endif
