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
#include "BarnettField.hpp"

#include "Observable_stat.hpp"
#include "Particle.hpp"
#include "config.hpp"
#include "rotation.hpp"

#include <utils/Vector.hpp>
#include <cmath>

namespace Constraints {

ParticleForce BarnettField::force(const Particle &p,
                                             const Utils::Vector3d &, double t){


#if defined(ROTATION) && defined(DIPOLES)
  return {
      {},
      vector_product(p.calc_dip(),
                     -1 * convert_vector_body_to_space(p,p.omega()) / m_gamma)};
#else
  return {};
#endif
}

void BarnettField::add_energy(const Particle &p,
                                         const Utils::Vector3d &, double t,
                                         Observable_stat &obs_energy) const {
#ifdef DIPOLES
  obs_energy.dipolar[0] +=
      1 / m_gamma * convert_vector_body_to_space(p, p.omega()) * p.calc_dip();
#endif
}
void BarnettField::add_magnetic_losses(const Particle &,
                                                   const Utils::Vector3d &,
                                                   double,
                                                   Observable_stat &) const {}

} // namespace Constraints
