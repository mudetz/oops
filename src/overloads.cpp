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

#include "overloads.h"
#include "parse_opts.h"

std::ostream &operator<<(std::ostream &os, Particle const &p)
{
	os << "Cost:\t" << p.best_cost() << "\n";
	os << "Reward:\t" << p.best_reward() << "\n";
	os << "Route:\t";

	auto route = std::move(p.best_route());
	for (size_t i = 0; i < route.size(); i++)
		os << route[i] + 1 << (i < route.size() - 1 ? "->" : "");

	return os;
}

size_t pair_hash::operator()(std::pair<unsigned int, unsigned int> const &v) const
{
	std::hash<unsigned int> hasher;
	return hasher(v.first) ^ hasher(v.second);
}

