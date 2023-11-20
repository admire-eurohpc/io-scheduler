# Copyright 2013-2023 Lawrence Livermore National Security, LLC and other
# Spack Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)
import re

from llnl.util import tty
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


def is_mercury_address(value):
    """a valid Mercury address"""
    return re.match(r"^(ofi|ucx)\+tcp://[0-9a-zA-Z.]+:[0-9]+$", value)


class Scord(CMakePackage):
    """A parallel data stager for malleable applications."""

    homepage = "https://storage.bsc.es/gitlab/eu/admire/io-scheduler"
    url = ("https://storage.bsc.es/gitlab/eu/admire/io-scheduler/-/archive/"
           "v0.3.2/io-scheduler-v0.3.2.tar.gz")
    git = "https://storage.bsc.es/gitlab/eu/admire/io-scheduler.git"

    maintainers("alberto-miranda")

    # available versions
    version("main", branch="main")
    version("0.2.0",
            sha256="61e0e2a10858e6a7027244f7b4609b64e03e8ef78ec080ef5536cacf7623ab42")
    version("0.2.1",
            sha256="e0a2e7fb835544eace291fc94ea689e504a84a6a6ef3d28c6a098d16cc4a7000")
    version("0.2.2",
            sha256="a336a96505158007fd64363e9c775ea8b24e9de984248724682fcb6e412f46fc")
    version("0.3.0",
            sha256="89dff9028bd2081205d0c029e63a8df1493f3f3bb03dfcddfc7f651c324d40a1")
    version("0.3.1",
            sha256="860bb115780082bab1ef99aa161e4bbb2907704ddea48526cabd7b814c2601b3")
    version("0.3.2",
            sha256="74c51915315e01d8479701d340331641f42c5f5cfae0c08bdea6c2f0b01da665")

    # build variants
    variant('build_type',
            default='Release',
            description='CMake build type',
            values=('Debug', 'Release', 'RelWithDebInfo', 'ASan'))

    variant("ofi",
            default=True,
            when="@0.2.0:",
            description="Use OFI libfabric as transport library")

    variant("ucx",
            default=False,
            when="@0.2.0:",
            description="Use UCX as transport library")

    variant("address",
            values=is_mercury_address,
            default="ofi+tcp://127.0.0.1:52000",
            when="@0.2.0:",
            description="Specify the Mercury address where the `scord` server "
                        "should listen for requests")

    # general dependencies
    depends_on("cmake@3.19", type='build', when='@:0.3.0')
    depends_on("cmake@3.20", type='build', when='@0.3.1:')

    # specific dependencies
    # v0.2.0+
    depends_on("argobots@1.1", when='@0.2.0:')
    depends_on("mochi-margo@0.9.8:", when='@0.2.0:')
    depends_on("mochi-thallium@0.10.1:", when='@0.2.0:')
    depends_on("boost@1.71 +program_options", when='@0.2.0:')
    depends_on("redis-plus-plus@1.3.3:", when='@0.2.0:')
    depends_on("cargo@0.3.2:", when='@0.3.1:')
    depends_on("slurm", when='@0.3.1:')

# slurm:
#   externals:
#    - spec: "slurm@23.02.6"
#      prefix: "opt/slurm-23.02.6/"

    with when("@0.2.0: +ofi"):
        depends_on("libfabric@1.14.0 fabrics=sockets,tcp,rxm")
        depends_on("mercury@2.1.0: +ofi")

    with when("@0.2.0: +ucx"):
        depends_on("ucx@1.12.0")
        depends_on("mercury@2.1.0: +ucx")

    def cmake_args(self):
        """Setup scord CMake arguments"""

        args = [
            self.define('SCORD_BUILD_TESTS', self.run_tests),
            self.define('SCORD_BUILD_EXAMPLES', self.run_tests)
        ]

        if "address" in self.spec.variants:
            protocol, tmp = self.spec.variants["address"].value.split("://")
            host, port = tmp.split(":")

            args.extend([
                self.define('SCORD_TRANSPORT_PROTOCOL', protocol),
                self.define('SCORD_BIND_ADDRESS', host),
                self.define('SCORD_BIND_PORT', port)
            ])

        return args

    def check(self):
        """Run tests"""
        with working_dir(self.build_directory):
            ctest("--output-on-failure", parallel=False)
