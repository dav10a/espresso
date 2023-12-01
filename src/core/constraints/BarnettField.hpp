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
#ifndef CONSTRAINTS_BARNETTFIELD_HPP
#define CONSTRAINTS_BARNETTFIELD_HPP

#include "Constraint.hpp"
#include "Observable_stat.hpp"
#include "Particle.hpp"

#include <utils/Vector.hpp>

namespace Constraints {

class BarnettField : public Constraint {
public:
  BarnettField()
      :  m_gamma(0.) {}

  void set_gamma(double const &w) { m_gamma = w; }


  double const &gamma() const { return m_gamma; }

  void add_energy(const Particle &p, const Utils::Vector3d &, double,
                  Observable_stat &energy) const override;
                
  void add_magnetic_losses(const Particle &p, const Utils::Vector3d &, double,
                  Observable_stat &dH) const;

  ParticleForce force(const Particle &p, const Utils::Vector3d &,
                      double) override;

  bool fits_in_box(Utils::Vector3d const &) const override { return true; }

private:
  double m_gamma;
};

} /* namespace Constraints */

#endif
