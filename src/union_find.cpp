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

#include "union_find.h"

// pair::first <- rank
// pair::second <- parent

UnionFind::UnionFind(unsigned int size)
	: _uf(size, std::make_pair(0, 0))
{
	for (unsigned int i = 0; i < size; i++)
		_uf[i].second = i;
}

unsigned int UnionFind::find(unsigned int x) const
{
	while (_uf[x].second != x)
		x = _uf[x].second;
	return x;
}

void UnionFind::unite(unsigned int x, unsigned int y)
{
	unsigned int x_root = find(x);
	unsigned int y_root = find(y);

	if (x_root == y_root) return;

	if (_uf[x_root].first < _uf[y_root].first) {
		_uf[x_root].second = y_root;
	} else if (_uf[x_root].first > _uf[y_root].first) {
		_uf[y_root].second = x_root;
	} else {
		_uf[y_root].second = x_root;
		_uf[x_root].first++;
	}
}
