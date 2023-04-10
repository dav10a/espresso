/*
 * Copyright (C) 2010-2022 The ESPResSo project
 *
 * This file is part of ESPResSo.
 *
 * ESPResSo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ESPResSo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ShapeBasedConstraint.hpp"

#include "BoxGeometry.hpp"
#include "Observable_stat.hpp"
#include "communication.hpp"
#include "config.hpp"
#include "dpd.hpp"
#include "energy_inline.hpp"
#include "errorhandling.hpp"
#include "forces_inline.hpp"
#include "grid.hpp"
#include "nonbonded_interactions/nonbonded_interaction_data.hpp"
#include "thermostat.hpp"

#include <utils/Vector.hpp>

#include <boost/mpi/collectives.hpp>

#include <algorithm>
#include <functional>
#include <limits>
#include <numeric>

namespace Constraints {
Utils::Vector3d ShapeBasedConstraint::total_force() const {
  return all_reduce(comm_cart, m_local_force, std::plus<>());
}

double ShapeBasedConstraint::total_normal_force() const {
  return all_reduce(comm_cart, m_outer_normal_force, std::plus<double>());
}

double ShapeBasedConstraint::min_dist(const ParticleRange &particles) {
  double global_mindist = std::numeric_limits<double>::infinity();

  auto const local_mindist = std::accumulate(
      particles.begin(), particles.end(),
      std::numeric_limits<double>::infinity(),
      [this](double min, Particle const &p) {
        IA_parameters const &ia_params =
            *get_ia_param(p.type(), part_rep.type());
        if (checkIfInteraction(ia_params)) {
          double dist;
          Utils::Vector3d vec;
          m_shape->calculate_dist(folded_position(p.pos(), box_geo), dist, vec);
          return std::min(min, dist);
        }
        return min;
      });
  boost::mpi::reduce(comm_cart, local_mindist, global_mindist,
                     boost::mpi::minimum<double>(), 0);
  return global_mindist;
}

ParticleForce ShapeBasedConstraint::force(Particle const &p,
                                          Utils::Vector3d const &folded_pos,
                                          double) {
  ParticleForce pf{};
  IA_parameters const &ia_params = *get_ia_param(p.type(), part_rep.type());

  if (checkIfInteraction(ia_params)) {
    double dist = 0.;
    Utils::Vector3d dist_vec;
    m_shape->calculate_dist(folded_pos, dist, dist_vec);
    auto const coulomb_kernel = Coulomb::pair_force_kernel();

#ifdef DPD
    Utils::Vector3d dpd_force{};
#endif
    Utils::Vector3d outer_normal_vec{};

    if (dist > 0) {
      outer_normal_vec = -dist_vec / dist;
      pf = calc_non_bonded_pair_force(p, part_rep, ia_params, dist_vec, dist,
                                      coulomb_kernel.get_ptr());
#ifdef DPD
      if (thermo_switch & THERMO_DPD) {
        dpd_force =
            dpd_pair_force(p, part_rep, ia_params, dist_vec, dist, dist * dist);
        // Additional use of DPD here requires counter increase
        dpd.rng_increment();
      }
#endif
    } else if (m_penetrable && (dist <= 0)) {
      if ((!m_only_positive) && (dist < 0)) {
        pf = calc_non_bonded_pair_force(p, part_rep, ia_params, dist_vec, -dist,
                                        coulomb_kernel.get_ptr());
#ifdef DPD
        if (thermo_switch & THERMO_DPD) {
          dpd_force = dpd_pair_force(p, part_rep, ia_params, dist_vec, dist,
                                     dist * dist);
          // Additional use of DPD here requires counter increase
          dpd.rng_increment();
        }
#endif
      }
    } else {
      runtimeErrorMsg() << "Constraint violated by particle " << p.id()
                        << " dist " << dist;
    }

#ifdef ROTATION
    part_rep.torque() += calc_opposing_force(pf, dist_vec).torque;
#endif
#ifdef DPD
    pf.f += dpd_force;
#endif
    m_local_force -= pf.f;
    m_outer_normal_force -= outer_normal_vec * pf.f;
  }
  return pf;
}

void ShapeBasedConstraint::add_energy(const Particle &p,
                                      const Utils::Vector3d &folded_pos, double,
                                      Observable_stat &obs_energy) const {
  double energy = 0.0;

  IA_parameters const &ia_params = *get_ia_param(p.type(), part_rep.type());

  if (checkIfInteraction(ia_params)) {
    auto const coulomb_kernel = Coulomb::pair_energy_kernel();
    double dist = 0.0;
    Utils::Vector3d vec;
    m_shape->calculate_dist(folded_pos, dist, vec);
    if (dist > 0) {
      energy = calc_non_bonded_pair_energy(p, part_rep, ia_params, vec, dist,
                                           coulomb_kernel.get_ptr());
    } else if ((dist <= 0) && m_penetrable) {
      if (!m_only_positive && (dist < 0)) {
        energy = calc_non_bonded_pair_energy(p, part_rep, ia_params, vec, -dist,
                                             coulomb_kernel.get_ptr());
      }
    } else {
      runtimeErrorMsg() << "Constraint violated by particle " << p.id();
    }
  }
  if (part_rep.type() >= 0)
    obs_energy.add_non_bonded_contribution(p.type(), part_rep.type(), energy);
}

void ShapeBasedConstraint::add_magnetic_losses(const Particle &,
                                               const Utils::Vector3d &, double,
                                               Observable_stat &) const {}
} // namespace Constraints
