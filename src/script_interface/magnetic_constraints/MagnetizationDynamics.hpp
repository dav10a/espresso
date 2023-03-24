/*
 * Copyright (C) 2010-2022 The ESPResSo project
 * Copyright (C) 2002,2003,2004,2005,2006,2007,2008,2009,2010
 *   Max-Planck-Institute for Polymer Research, Theory Group
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

#ifndef SCRIPT_INTERFACE_MAGNETICCONSTRAINTS_MAGNETIZATIONDYNAMICS_HPP
#define SCRIPT_INTERFACE_MAGNETICCONSTRAINTS_MAGNETIZATIONDYNAMICS_HPP

#include "core/magnetic_constraints/MagneticConstraint.hpp"
#include "core/magnetic_constraints/MagnetizationDynamics.hpp"

#include "script_interface/ScriptInterface.hpp"
#include "script_interface/magnetic_constraints/MagneticConstraint.hpp"

#include <utils/Vector.hpp>

#include <memory>

namespace ScriptInterface {
namespace MagneticConstraints {

class MagnetizationDynamics : public MagneticConstraint {
public:
  MagnetizationDynamics()
      : m_constraint(
            std::make_shared<::MagneticConstraints::MagnetizationDynamics>()) {
    add_parameters({{"dm",
                     [this](Variant const &v) {
                       m_constraint->set_dm(get_value<Utils::Vector3d>(v));
                     },
                     [this]() { return m_constraint->dm(); }}});
   
  }

  std::shared_ptr<::MagneticConstraints::MagneticConstraint> constraint() override {
    return std::static_pointer_cast<::MagneticConstraints::MagneticConstraint>(m_constraint);
  }
  std::shared_ptr<const ::MagneticConstraints::MagneticConstraint> constraint() const override {
    return std::static_pointer_cast<::MagneticConstraints::MagneticConstraint>(m_constraint);
  }
  std::shared_ptr<::MagneticConstraints::MagnetizationDynamics>
  homogeneous_magnetic_field() const {
    return m_constraint;
  }

private:
  /* The actual constraint */
  std::shared_ptr<::MagneticConstraints::MagnetizationDynamics> m_constraint;
};

} /* namespace MagneticConstraints */
} /* namespace ScriptInterface */

#endif
