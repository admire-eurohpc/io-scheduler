# Copyright 2013-2023 Lawrence Livermore National Security, LLC and other
# Spack Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)

# ----------------------------------------------------------------------------
# If you submit this package back to Spack as a pull request,
# please first remove this boilerplate and all FIXME comments.
#
# This is a template package file for Spack.  We've put "FIXME"
# next to all the things you'll want to change. Once you've handled
# them, you can save this file and test your package like this:
#
#     spack install redis-plus-plus
#
# You can edit this file again by typing:
#
#     spack edit redis-plus-plus
#
# See the Spack documentation for more information on packaging.
# ----------------------------------------------------------------------------

from spack.package import *


class RedisPlusPlus(CMakePackage):
    """A C++ client library for Redis"""

    homepage = 'https://github.com/sewenew/redis-plus-plus'
    url = 'https://github.com/sewenew/redis-plus-plus/archive/refs/tags/1.3.3.zip'
    git = 'https://github.com/sewenew/redis-plus-plus'

    version('1.3.3', sha256='0bb1b5e459817d8edc53ecf6e893d82c0ea4b459')

    depends_on('cmake@3.2:', type='build')

    depends_on('hiredis@1.2.0:', when='@1.3.3:')
