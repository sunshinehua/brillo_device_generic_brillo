#!/usr/bin/python

# Copyright 2016, The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


"""Unit-test for make_efi_image."""

import os
import subprocess
import tempfile
import unittest

class ArgumentTest(unittest.TestCase):
  """Unit tests for various argument values."""

  def testOutputPathSpaces(self):
    with tempfile.NamedTemporaryFile(prefix='foo ') as efi_image, \
      tempfile.NamedTemporaryFile() as efi_app:
      efi_args = \
        [ 'make_efi_image', '524288',
          efi_image.name ,
          efi_app.name + ':bar/biz/' + efi_app.name.split('/')[-1]]

      self.assertEqual(subprocess.call(efi_args, stdout=open(os.devnull, 'wb'))
        , 0)

class DirectoryStructureTest(unittest.TestCase):
  """Unit tests for files output paths."""

  def _Paths(self, dirs):
    """Helper function to place files into a list of directories.

    This function takes a list of directories and places one file in each to
    verify they are copied into an EFI image.  Listing the same directory N
    times will result in N files being placed into it.

    """
    with tempfile.NamedTemporaryFile() as efi_image:
      efi_args = ['make_efi_image', '524288', efi_image.name]
      ifiles = []
      target_paths = []

      for d in dirs:
        ifiles.append(tempfile.NamedTemporaryFile())
        target_paths.append(d + ifiles[-1].name.split('/')[-1])
        efi_args.append(ifiles[-1].name + ':' + target_paths[-1])

      self.assertEqual(subprocess.call(efi_args, stdout=open(os.devnull, 'wb'))
        , 0)

      for target in target_paths:
        self.assertEqual(
          subprocess.call( ['mdir', '-i', efi_image.name, target],
            stdout=open(os.devnull, 'wb'))
          , 0)

  def testOneDir(self):
    self._Paths([''])
    self._Paths(['foo/'])

  def testSameDir(self):
    self._Paths(['', ''])
    self._Paths(['foo/bar/','foo/bar/'])

  def testManyDir(self):
    self._Paths(['', 'foo/', 'bar/'])
    self._Paths(['', 'foo/bar/', 'foo/bar/biz/'])

if __name__ == '__main__':
  unittest.main()