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
#     spack install hiredis
#
# You can edit this file again by typing:
#
#     spack edit hiredis
#
# See the Spack documentation for more information on packaging.
# ----------------------------------------------------------------------------

from spack.package import *


class Hiredis(MakefilePackage):
    """A minimalistic C client library for the Redis database."""

    homepage = "https://github.com/redis/hiredis"
    url = "https://github.com/redis/hiredis/archive/refs/tags/v1.1.0.tar.gz"
    git = "https://github.com/redis/hiredis"

    version("1.1.0", sha256='fe6d21741ec7f3fc9df409d921f47dfc73a4d8ff64f4ac6f1d95f951bf7f53d6')

    depends_on("openssl")

    def edit(self, spec, prefix):
        env['PREFIX'] = prefix
        env['USE_SSL'] = '1'
