"""

Author: Arthur Wesley

"""

import shutil
import subprocess
from pathlib import Path

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


class MesonExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name=name, sources=[])
        self.sourcedir = Path(sourcedir).absolute()


class MesonBuild(build_ext):
    def build_extension(self, ext) -> None:

        # obtain the name of the output file and the output path
        ext_filename = Path(self.get_ext_filename(ext.name))
        ext_fullpath = Path(self.get_ext_fullpath(ext.name)).parent

        print("the extension's filepath is", ext_fullpath)

        # initialize the extension directory and temporary directory
        temp_dir = Path(self.build_temp)

        # create the temporary directory if it does not exist
        if not temp_dir.exists():
            # create the build directory
            subprocess.check_call(
                ["meson", "setup", temp_dir, "--buildtype", "debug" if self.debug else "release"]
            )

        # compile the code
        subprocess.check_call(
            ["meson", "compile", "-C", self.build_temp],
        )

        if not ext_fullpath.exists():
            ext_fullpath.mkdir(parents=True, exist_ok=True)

        # copy the executable
        shutil.copy(temp_dir/ext_filename, ext_fullpath/ext_filename)


def read_file(filename):
    """

    read the text of a file

    :param filename: name of the file to read
    :return: text of the file
    """

    with open(filename, encoding="utf-8") as file:
        return file.read()


setup(
    name="sente",
    python_requires=">=3.8.*",
    version=read_file("version.txt").strip(),
    author="Arthur Wesley",
    license="MIT",
    url="https://github.com/atw1020/sente",
    description="Sente: a Python 3 native library for the game of Go.",
    long_description=read_file("readme.md"),
    long_description_content_type="text/markdown",
    install_requires=["numpy>=1.7.0"],
    classifiers=["Programming Language :: Python",
                 "Programming Language :: Python :: 3",
                 "Programming Language :: Python :: 3.8",
                 "Programming Language :: Python :: 3.9",
                 "Programming Language :: Python :: 3.10",
                 "Operating System :: MacOS :: MacOS X",
                 "Operating System :: POSIX :: Linux",
                 "Operating System :: Microsoft :: Windows :: Windows 10"],
    author_email="arthur@electricfish.com",
    ext_modules=[MesonExtension("sente")],
    test_suite="tests",
    cmdclass={"build_ext": MesonBuild},
    zip_safe=False
)
