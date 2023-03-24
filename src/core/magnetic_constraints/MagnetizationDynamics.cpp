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
#include "MagnetizationDynamics.hpp"

#include "Observable_stat.hpp"
#include "Particle.hpp"
#include "config.hpp"

#include <utils/Vector.hpp>

#include <cmath>
#include <iostream>
namespace MagneticConstraints {

ParticleForce MagnetizationDynamics::force(const Particle &p,
                                              const Utils::Vector3d &, double t) {
#if defined(ROTATION) && defined(DIPOLES)
  return {{}, p.calc_dip()}; ;
#else
  return {};
#endif
}

DipoleMotion MagnetizationDynamics::dipole_boost(const Particle &p,
                                              const Utils::Vector3d &, double t) {
#ifdef DIPOLES
  return dm_ ;
#else
  return {};
#endif
}

void MagnetizationDynamics::add_energy(const Particle &p,
                                          const Utils::Vector3d &, double t,
                                          Observable_stat &obs_energy) const {
#ifdef DIPOLES
  obs_energy.dipolar[0] += 0.;
#endif
}

} // namespace MagneticConstraints
