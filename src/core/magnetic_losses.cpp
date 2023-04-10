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
/** \file
 *  Energy calculation.
 */

#include "EspressoSystemInterface.hpp"
#include "Observable_stat.hpp"
#include "communication.hpp"
#include "constraints.hpp"
#include "cuda_interface.hpp"
#include "energy_inline.hpp"
#include "event.hpp"
#include "forces.hpp"
#include "integrate.hpp"
#include "interactions.hpp"
#include "nonbonded_interactions/nonbonded_interaction_data.hpp"

#include "short_range_loop.hpp"

#include "electrostatics/coulomb.hpp"
#include "magnetostatics/dipoles.hpp"

#include <utils/Span.hpp>

#include <memory>

static std::shared_ptr<Observable_stat> calculate_magnetic_losses_local() {

  auto obs_magnetic_losses_ptr = std::make_shared<Observable_stat>(1);



  auto &obs_magnetic_losses = *obs_magnetic_losses_ptr;

  auto &espresso_system = EspressoSystemInterface::Instance();
  espresso_system.update();

  on_observable_calc();

  auto const local_parts = cell_structure.local_particles();


 


  Constraints::constraints.add_magnetic_losses(local_parts, get_sim_time(), obs_magnetic_losses);



  obs_magnetic_losses.mpi_reduce();
  return obs_magnetic_losses_ptr;
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
}

REGISTER_CALLBACK_MAIN_RANK(calculate_magnetic_losses_local)

std::shared_ptr<Observable_stat> calculate_magnetic_losses() {
  return mpi_call(Communication::Result::main_rank, calculate_magnetic_losses_local);
}



double observable_compute_magnetic_losses() {
  auto const obs_magnetic_losses = calculate_magnetic_losses();
  return obs_magnetic_losses->accumulate(0);
}



