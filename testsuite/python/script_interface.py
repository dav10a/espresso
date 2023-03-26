#
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
#

import unittest as ut

import espressomd.shapes
import espressomd.constraints
import espressomd.interactions


class SphereWithProperties(espressomd.shapes.Sphere):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._mass = None

    @property
    def mass(self):
        return self._mass

    @mass.setter
    def mass(self, value):
        self._mass = value

    @mass.deleter
    def mass(self):
        self._mass = None


class ScriptInterface(ut.TestCase):

    def test_object_params(self):
        """Check that object parameters are assigned the correct python class"""
        c = espressomd.constraints.ShapeBasedConstraint()
        w = espressomd.shapes.Wall(normal=[-1, 0, 0])
        c.shape = w
        # Does the shape parameter return the correct class
        self.assertEqual(c.shape.__class__, espressomd.shapes.Wall)
        # Does the script object match
        self.assertEqual(c.shape, w)

        # Different shape
        c.shape = espressomd.shapes.Sphere(radius=1)
        # Test class
        self.assertEqual(c.shape.__class__, espressomd.shapes.Sphere)
        # Test parameter retrieval
        self.assertAlmostEqual(c.shape.radius, 1, places=8)

    def test_attributes_integration(self):
        # check the script interface distinguishes between core parameters,
        # python attributes stored in object dict, and python attributes
        # created with decorator @property
        obj = SphereWithProperties(radius=1)
        self.assertTrue(hasattr(obj, 'mass'))
        self.assertTrue(hasattr(obj, '_mass'))
        self.assertTrue(hasattr(obj, 'radius'))
        self.assertFalse(hasattr(obj, 'density'))
        self.assertIsNone(obj.mass)
        self.assertIn('_mass', obj.__dict__)
        obj.mass = 5.
        obj.density = 2.
        self.assertEqual(obj.mass, 5.)
        self.assertEqual(obj._mass, 5.)
        self.assertEqual(obj.density, 2.)
        del obj.mass
        self.assertIsNone(obj.mass)
        self.assertIn('_mass', obj.__dict__)
        self.assertIsNone(obj._mass)
        del obj.density
        self.assertNotIn('density', obj.__dict__)
        with self.assertRaisesRegex(AttributeError, "Object 'SphereWithProperties' has no attribute 'density'"):
            obj.density
        with self.assertRaisesRegex(RuntimeError, "Parameter 'radius' is read-only"):
            del obj.radius

    def test_autoparameter_exceptions(self):
        """Check AutoParameters framework"""
        constraint = espressomd.constraints.ShapeBasedConstraint()
        bond = espressomd.interactions.HarmonicBond(k=5., r_0=1., r_cut=2.)
        with self.assertRaisesRegex(RuntimeError, "Unknown parameter 'unknown_param'"):
            constraint.shape = espressomd.shapes.Wall(unknown_param=0)
        with self.assertRaisesRegex(RuntimeError, "Parameter 'k' is read-only"):
            bond.k = 1.
        with self.assertRaisesRegex(AttributeError, "Object 'HarmonicBond' has no attribute 'unknown'"):
            bond.unknown

    def test_variant_exceptions(self):
        """Check AutoParameters framework"""
        constraint = espressomd.constraints.ShapeBasedConstraint()
        # check conversion of unsupported types
        err_msg = "No conversion from type 'module' to 'Variant'"
        with self.assertRaisesRegex(TypeError, err_msg):
            espressomd.constraints.ShapeBasedConstraint(shape=ut)
        with self.assertRaisesRegex(TypeError, err_msg):
            espressomd.constraints.ShapeBasedConstraint(unknown=ut)
        with self.assertRaisesRegex(TypeError, err_msg):
            constraint.set_params(shape=ut)
        with self.assertRaisesRegex(TypeError, err_msg):
            constraint.call_method('unknown', unknown=ut)
        # check restrictions on the dict type
        with self.assertRaisesRegex(TypeError, r"No conversion from type 'dict_item\(\[\(float, int\)\]\)' to 'Variant\[std::unordered_map<int, Variant>\]'"):
            constraint.shape = {5.: 2}
        # check type mismatch
        error_msg = "Provided argument of type '{}' is not convertible to 'std::(__1::)?shared_ptr<ScriptInterface::Shapes::Shape>'"
        with self.assertRaisesRegex(RuntimeError, error_msg.format("int")):
            constraint.shape = 5
        with self.assertRaisesRegex(RuntimeError, error_msg.format("ScriptInterface::None")):
            constraint.shape = None
        with self.assertRaisesRegex(RuntimeError, error_msg.format("std::(__1::)?shared_ptr<ScriptInterface::ObjectHandle>")):
            constraint.shape = constraint

    def test_compare(self):
        """Check that script interface objects are equality comparable"""
        a = espressomd.constraints.ShapeBasedConstraint()
        b = espressomd.constraints.ShapeBasedConstraint()
        c = 5
        self.assertEqual(a, a)
        self.assertNotEqual(a, b)
        self.assertFalse(a == c)
        self.assertFalse(c == a)
        self.assertTrue(a != c)
        self.assertTrue(c != a)
        with self.assertRaises(NotImplementedError):
            a > a
        with self.assertRaises(NotImplementedError):
            a >= a
        with self.assertRaises(NotImplementedError):
            a < a
        with self.assertRaises(NotImplementedError):
            a <= a
        with self.assertRaises(NotImplementedError):
            a > c
        with self.assertRaises(NotImplementedError):
            a >= c
        with self.assertRaises(NotImplementedError):
            a < c
        with self.assertRaises(NotImplementedError):
            a <= c


if __name__ == "__main__":
    ut.main()
