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
#     spack install scord
#
# You can edit this file again by typing:
#
#     spack edit scord
#
# See the Spack documentation for more information on packaging.
# ----------------------------------------------------------------------------

from spack.package import *


class Scord(CMakePackage):
    """A parallel data stager for malleable applications."""

    homepage = "https://storage.bsc.es/gitlab/eu/admire/io-scheduler"
    url = "https://storage.bsc.es/gitlab/eu/admire/io-scheduler/-/archive/v0.2.1/io-scheduler-v0.2.1.tar.bz2"
    git = "https://storage.bsc.es/gitlab/eu/admire/io-scheduler.git"

    maintainers("alberto-miranda")

    # available versions
    version("latest", branch="main")
    version("0.2.0", sha256="61e0e2a10858e6a7027244f7b4609b64e03e8ef78ec080ef5536cacf7623ab42")

    # build variants
    variant('build_type',
            default='Release',
            description='CMake build type',
            values=('Debug', 'Release', 'RelWithDebInfo', 'ASan'))

    variant('tests',
            default=False,
            description='Build and run tests for scord')

    variant("ofi",
            default=True,
            when="@0.2.0:",
            description="Use OFI libfabric as transport library")

    variant("ucx",
            default=False,
            when="@0.2.0:",
            description="Use UCX as transport library")


    # general dependencies
    depends_on("cmake@3.19", type='build')

    # specific dependencies
    # v0.2.0+
    depends_on("argobots@1.1", when='@0.2.0:')
    depends_on("mochi-margo@0.9.8", when='@0.2.0:')
    depends_on("mochi-thallium@0.10.1", when='@0.2.0:')
    depends_on("boost@1.71 +program_options", when='@0.2.0:')
    depends_on("agios@latest", when='@0.2.0:')
    depends_on("redis-plus-plus@1.3.3:", when='@0.2.0:')

    with when("@0.2.0: +ofi"):
        depends_on("libfabric@1.14.0 fabrics=sockets,tcp,rxm")
        depends_on("mercury@2.1.0 +ofi")

    with when("@0.2.0: +ucx"):
        depends_on("ucx@1.12.0")
        depends_on("mercury@2.1.0 +ucx")

    def cmake_args(self):
        """Setup scord CMake arguments"""
        cmake_args = [
            self.define_from_variant('SCORD_BUILD_TESTS', 'tests')
        ]
        return cmake_args

    def check(self):
        """Run tests"""
        with working_dir(self.build_directory):
            make("test", parallel=False)
