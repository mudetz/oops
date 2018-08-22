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

#ifndef __overloads_h__
#define __overloads_h__

#include <algorithm>
#include <functional>
#include <memory>
#include <ostream>
#include <vector>
#include "particle.h"

// Print particles
std::ostream &operator<<(std::ostream &, Particle const &);

// Multiply scalar and vector
template <typename T, typename Q>
std::vector<T> operator*(Q const, std::vector<T>);

// Add vectors
template <typename T>
std::vector<T> operator+(std::vector<T> const &, std::vector<T> const &);

// Subtract vectors
template <typename T>
std::vector<T> operator-(std::vector<T> const &, std::vector<T> const &);

// Add-assign vector
template <typename T>
std::vector<T> &operator+=(std::vector<T> &, std::vector<T> const &);

// Subtract-assign vector
template <typename T>
std::vector<T> &operator-=(std::vector<T> &, std::vector<T> const &);

// Hash a pair (for unordered_set)
struct pair_hash {
	size_t operator()(std::pair<unsigned int, unsigned int> const &) const;
};

/* */
template <typename T, typename Q>
std::vector<T> operator*(Q const c, std::vector <T> A)
{
	std::transform(A.begin(), A.end(), A.begin(),
		       std::bind1st(std::multiplies<T>(), c));
	return A;
}

template <typename T>
std::vector<T> operator+(std::vector<T> const &a, std::vector<T> const &b)
{
	assert(a.size() == b.size());

	std::vector<T> result;
	result.reserve(a.size());

	std::transform(a.begin(), a.end(), b.begin(),
		       std::back_inserter(result), std::plus<T>());
	return result;
}

template <typename T>
std::vector<T> operator-(std::vector<T> const &a, std::vector<T> const &b)
{
	assert(a.size() == b.size());

	std::vector<T> result;
	result.reserve(a.size());

	std::transform(a.begin(), a.end(), b.begin(),
		       std::back_inserter(result), std::minus<T>());
	return result;
}

template <typename T>
std::vector<T> &operator+=(std::vector<T> &a, std::vector<T> const &b)
{
	assert(a.size() == b.size());

	std::transform(b.begin(), b.end(), a.begin(), a.begin(),
		       std::plus<T>());
	return a;
}

template <typename T>
std::vector<T> &operator-=(std::vector<T> &a, std::vector<T> const &b)
{
	assert(a.size() == b.size());

	std::transform(b.begin(), b.end(), a.begin(), a.begin(),
		       std::minus<T>());
	return a;
}

#endif
