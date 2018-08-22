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

#include <iostream>
#include "graph.h"
#include "overloads.h"
#include "parse_opts.h"
#include "particle.h"
#include "pso.h"

int main(int const argc, char const **argv)
{
	if (parse_opts(argc, argv))
		return 0;

	if (VM.at("verbose").as<bool>())
		verbose_opts();

	double Cmin;
	double Cmax;
	unsigned int V;
	unsigned int E;
	unsigned int S0;

	std::cin >> Cmin;
	std::cin >> Cmax;
	std::cin >> V;
	std::cin >> E;
	std::cin >> S0; S0--;

	if (VM.at("verbose").as<bool>()) {
		std::cerr << "Cmin:\t"
			  << Cmin
			  << "\nCmax:\t"
			  << Cmax
			  << "\nV:\t"
			  << V
			  << "\nE:\t"
			  << E
			  << "\nS0:\t"
			  << S0
			  << "\n\n";
	}

	Graph G(V, S0);

	for (unsigned int i = 0; i < E; i++) {
		unsigned int id;
		unsigned int from;
		unsigned int to;
		double cost;
		unsigned int score;

		std::cin >> id >> from >> to >> cost >> score;
		G.add_edge(from - 1, to - 1, cost, score);
	}

	if (VM.at("verbose").as<bool>())
		std::cerr << "Analyzing graph...\n";
	G.analyze(VM.at("mst").as<bool>());
	if (VM.at("verbose").as<bool>()) {
		std::cerr << "Blacklisted nodes: \t";
		for (auto &b : G.blacklist())
			std::cerr << b << ' ';
		std::cerr << '\n';
	}

	Particle best = std::move(pso(G, Cmin, Cmax,
				      VM.at("max-cycles").as<unsigned int>(),
				      VM.at("swarm-size").as<unsigned int>(),
				      VM.at("social-factor").as<double>(),
				      VM.at("cognitive-factor").as<double>(),
				      VM.at("mst").as<bool>(),
				      VM.at("random").as<bool>(),
				      VM.at("verbose").as<bool>(),
				      VM.at("optima").as<unsigned int>()));

	std::cout << best << '\n';

	return 0;
}
