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
#     spack install agios
#
# You can edit this file again by typing:
#
#     spack edit agios
#
# See the Spack documentation for more information on packaging.
# ----------------------------------------------------------------------------

from spack.package import *


class Agios(CMakePackage):
    """AGIOS: an I/O request scheduling library at file level."""


    homepage = 'https://github.com/francielizanon/agios'
    url      = 'https://github.com/jeanbez/agios/archive/refs/tags/v1.0.tar.gz'
    git      = 'https://github.com/francielizanon/agios.git'

    version('latest', branch='development')
    version('1.0', sha256='e8383a6ab0180ae8ba9bb2deb1c65d90c00583c3d6e77c70c415de8a98534efd')

    # general dependencies
    depends_on('cmake@3.5', type='build')
    depends_on('libconfig')
