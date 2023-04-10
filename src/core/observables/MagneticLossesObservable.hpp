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
#ifndef OBSERVABLES_MAGNETIC_LOSSES_HPP
#define OBSERVABLES_MAGNETIC_LOSSES_HPP

#include "Observable.hpp"
#include "magnetic_losses.hpp"

#include <cstddef>
#include <vector>

namespace Observables {

class MagneticLosses : public Observable {
public:
  std::vector<std::size_t> shape() const override { return {1}; }
  std::vector<double> operator()() const override {
    std::vector<double> res{1};
    res[0] = observable_compute_magnetic_losses();
    return res;
  }
};

} // Namespace Observables

#endif
