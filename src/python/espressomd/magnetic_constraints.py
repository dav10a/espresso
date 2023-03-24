# Copyright (C) 2010-2022 The ESPResSo project
#
# This file is part of ESPResSo.
#
# ESPResSo is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ESPResSo is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
from .script_interface import ScriptObjectList, ScriptInterfaceHelper, script_interface_register
import numpy as np
import itertools


@script_interface_register
class MagneticConstraints(ScriptObjectList):

    """
    List of active constraints. Add a :class:`espressomd.constraints.Constraint`
    to make it active in the system, or remove it to make it inactive.

    """

    _so_name = "MagneticConstraints::MagneticConstraints"

    def add(self, *args, **kwargs):
        """
        Add a constraint to the list.

        Parameters
        ----------
        constraint: :class:`espressomd.MagneticConstraints.MagneticConstraints`
            Either a constraint object...
        \*\*kwargs : any
            ... or parameters to construct an
            :class:`espressomd.magnetic_constraints.ShapeBasedConstraint`

        Returns
        ----------
        constraint : :class:`espressomd.magnetic_constraints.MagneticConstraint`
            The added constraint

        """

        if len(args) == 1:
            if isinstance(args[0], MagneticConstraint):
                constraint = args[0]
                self.call_method("add", object=constraint)
            else:
                raise TypeError(
                    "Either a Constraint object or key-value pairs for the parameters of a ShapeBasedConstraint object need to be passed.")
        
        
        return constraint

    def remove(self, constraint):
        """
        Remove a constraint from the list.

        Parameters
        ----------
        constraint : :obj:`espressomd.constraints.Constraint`

        """

        self.call_method("remove", object=constraint)

    def clear(self):
        """
        Remove all constraints.

        """
        self.call_method("clear")


class MagneticConstraint(ScriptInterfaceHelper):

    """
    Base class for constraints. A constraint provides a force and
    an energy contribution for a single particle.

    """

    _so_name = "MagneticConstraints::MagneticConstraint"



@script_interface_register
class MagnetizationDynamics(MagneticConstraint):

    """
    Attributes
    ----------
    dm : (3,) array_like of :obj:`float`
        Dipole boost.

    """

    _so_name = "MagneticConstraints::MagnetizationDynamics"
