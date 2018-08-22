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

#ifndef __union_find_h__
#define __union_find_h__

#include <utility>
#include <vector>

// pair::first <- rank
// pair::second <- parent

class UnionFind {
private:
	std::vector< std::pair<unsigned int, unsigned int> > _uf;
public:
	UnionFind(unsigned int);

	unsigned int find(unsigned int) const;
	void unite(unsigned int, unsigned int);
};

#endif
