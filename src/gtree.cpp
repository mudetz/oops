/*
 * OOPS - Particle Swarm Optimization for the Arc Orienteering Problem
 * Copyright (C) 2018	Manuel Weitzman
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include "gtree.h"

GTree::GTree(unsigned int id)
	: _id(id)
	, _sons()
{}

void GTree::add_edges(std::vector< std::pair<unsigned int, unsigned int> > &E)
{
	// Recursively create sons using arcs
	for (auto it = E.begin(); it != E.end(); ) {
		// If arc starts where we are, add destination
		if (it->first == _id) {
			_sons.emplace_back(it->second);
			it = E.erase(it);
		} else {
			it++;
		}
	}

	for (auto &son : _sons)
		son.add_edges(E);
}

void GTree::preorder(std::vector<unsigned int> &R, std::vector<double> const &P,
		     std::vector<bool> const &V) const
{
	// Filter and sort sons that will be visited
	std::vector<unsigned int> U;
	U.reserve(_sons.size());

	for (size_t i = 0; i < _sons.size(); i++)
		if (V.at(_sons.at(i)._id))
			U.push_back(i);

	auto cmp = [this, &P](auto const x, auto const y) {
		return P.at(_sons.at(x)._id) < P.at(_sons.at(y)._id);
	};
	std::sort(U.begin(), U.end(), cmp);

	// Visit self, then sons
	for (size_t i = 0; i < U.size(); i++) {
		R.push_back(_id);
		_sons.at(i).preorder(R, P, V);
	}
}
