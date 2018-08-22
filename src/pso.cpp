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
#include <future>
#include <iostream>
#include "overloads.h"
#include "pso.h"
#include "threadpool.h"


Particle pso(Graph &G, double Cmin, double Cmax, unsigned int max_cycles,
	 unsigned int swarm_size, double social_factor, double cognitive_factor,
	 bool use_mst, bool randomize, bool verbose, unsigned int optima,
	 unsigned int threads)
{
	if (verbose)
		std::cerr << "Starting PSO.\nGenerating random particles...\n";

	// Set evaluation function penalization, Cmax, Cmin, and if it should
	// use MST
	Particle::penalty(Cmax);
	Particle::Cmin(Cmin);
	Particle::Cmax(Cmax);
	Particle::use_mst(use_mst);

	// Generate and randomize swarm
	std::vector<Particle> swarm(swarm_size, Particle(G));
	for (Particle &p : swarm) {
		p.randomize();
		p.eval();
	}

	// Select best particle so far, maybe we already found a good one!
	Particle best = Particle::best(swarm);

	if (verbose)
		std::cerr << "Random particles generated.\n"
			  << "Starting optimization...\n";

	// Create a threadpool to process particles
	ThreadPool T(threads);

	// Iterate max_cycles, if optima is set, iterate until optima is found
	for (unsigned int i = 0; optima || (i < max_cycles); i++) {
		if (optima && best.best_reward() == optima) {
			std::cerr << "Iterations: " << i << '\n';
			break;
		}
		if (verbose && (max_cycles <= 100 || i % (max_cycles / 100) == 0)
		    && i != max_cycles)
			std::cerr << "Optimizing "
				  << int(100.0 * i / max_cycles)
				  << "%...\n";

		// Create a job "queue" to process particles
		std::vector< std::future<void> > jobs;
		for (auto &p : swarm) {
			jobs.emplace_back(T.enqueue([&] {
				if (!randomize) {
					p.update_speed(best, social_factor, cognitive_factor);
					p.update_position();
				} else {
					p.randomize();
				}
				p.eval();
			}));
		}

		// Wait for all particles to be ready
		for (auto &job : jobs)
			job.wait();

		// Update best particle
		best = Particle::best(swarm);
	}

	if (verbose)
		std::cerr << "Optimizing: 100%!\n";

	// Return best particle
	return best;
}

