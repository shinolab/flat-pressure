#!/usr/bin/env python3

import argparse
import contextlib
import os
import platform
import re
import shutil
import subprocess
import sys
import tarfile
from glob import glob
from typing import List, Optional

import requests
from packaging import version


def fetch_submodule():
    if shutil.which("git") is not None:
        with working_dir(os.path.dirname(os.path.abspath(__file__))):
            subprocess.run(
                ["git", "submodule", "update", "--init", "--recursive"]
            ).check_returncode()
    else:
        err("git is not installed. Skip fetching submodules.")


def generate_wrapper():
    if shutil.which("cargo") is not None:
        with working_dir(os.path.dirname(os.path.abspath(__file__))):
            with working_dir("tools/wrapper-generator"):
                subprocess.run(
                    [
                        "cargo",
                        "run",
                    ]
                ).check_returncode()
    else:
        err("cargo is not installed. Skip generating wrapper.")


def err(msg: str):
    print("\033[91mERR \033[0m: " + msg)


def warn(msg: str):
    print("\033[93mWARN\033[0m: " + msg)


def info(msg: str):
    print("\033[92mINFO\033[0m: " + msg)


def rm_f(path):
    try:
        os.remove(path)
    except FileNotFoundError:
        pass


def onexc(func, path, exeption):
    import stat

    if not os.access(path, os.W_OK):
        os.chmod(path, stat.S_IWUSR)
        func(path)
    else:
        raise


def rmtree_f(path):
    try:
        if version.parse(platform.python_version()) >= version.parse("3.12"):
            shutil.rmtree(path, onexc=onexc)
        else:
            shutil.rmtree(path, onerror=onexc)
    except FileNotFoundError:
        pass


@contextlib.contextmanager
def working_dir(path):
    cwd = os.getcwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(cwd)


class Config:
    _platform: str
    release: bool
    no_examples: bool
    cmake_extra: Optional[List[str]]

    def __init__(self, args):
        self._platform = platform.system()

        if not self.is_windows() and not self.is_macos() and not self.is_linux():
            err(f'platform "{platform.system()}" is not supported.')
            sys.exit(-1)

        self.release = hasattr(args, "release") and args.release
        self.no_examples = hasattr(args, "no_examples") and args.no_examples
        self.cmake_extra = (
            args.cmake_extra.split(" ")
            if hasattr(args, "cmake_extra") and args.cmake_extra is not None
            else None
        )

    def is_windows(self):
        return self._platform == "Windows"

    def is_macos(self):
        return self._platform == "Darwin"

    def is_linux(self):
        return self._platform == "Linux"

    def exe_ext(self):
        return ".exe" if self.is_windows() else ""

    def is_cuda_available(self):
        return shutil.which("nvcc") is not None

    def is_pcap_available(self):
        if not self.is_windows():
            return True
        wpcap_exists = os.path.isfile(
            "C:\\Windows\\System32\\wpcap.dll"
        ) and os.path.isfile("C:\\Windows\\System32\\Npcap\\wpcap.dll")
        packet_exists = os.path.isfile(
            "C:\\Windows\\System32\\Packet.dll"
        ) and os.path.isfile("C:\\Windows\\System32\\Npcap\\Packet.dll")

        return wpcap_exists and packet_exists


def should_update_lib(config: Config, version: str) -> bool:
    if config.is_windows():
        if not os.path.isfile("lib/autd3capi.lib"):
            return True
    else:
        if not os.path.isfile("lib/libautd3capi.a"):
            return True

    if not os.path.isfile("VERSION"):
        return True

    with open("VERSION", "r") as f:
        old_version = f.read().strip()

    return old_version != version


def copy_lib(config: Config):
    with open("CMakeLists.txt", "r") as f:
        content = f.read()
        version = re.search(r"project\(autd3 VERSION (.*)\)", content).group(1)
        version = ".".join(version.split(".")[:3])

    if not should_update_lib(config, version):
        return

    if config.is_windows():
        url = f"https://github.com/shinolab/autd3-capi/releases/download/v{version}/autd3-v{version}-win-x64-static.zip"
        with open("tmp.zip", mode="wb") as f:
            f.write(requests.get(url).content)
        shutil.unpack_archive("tmp.zip", ".")
        rm_f("tmp.zip")
    elif config.is_macos():
        url = f"https://github.com/shinolab/autd3-capi/releases/download/v{version}/autd3-v{version}-macos-universal-static.tar.gz"
        with open("tmp.tar.gz", mode="wb") as f:
            f.write(requests.get(url).content)
        with tarfile.open("tmp.tar.gz", "r:gz") as tar:
            tar.extractall()
        rm_f("tmp.tar.gz")
    elif config.is_linux():
        url = f"https://github.com/shinolab/autd3-capi/releases/download/v{version}/autd3-v{version}-linux-x64-static.tar.gz"
        with open("tmp.tar.gz", mode="wb") as f:
            f.write(requests.get(url).content)
        with tarfile.open("tmp.tar.gz", "r:gz") as tar:
            tar.extractall()
        rm_f("tmp.tar.gz")
    rmtree_f("bin")

    with open("VERSION", mode="w") as f:
        f.write(version)


def cpp_build(args):
    config = Config(args)

    copy_lib(config)

    if not config.no_examples:
        info("Building examples...")
        with working_dir("examples"):
            os.makedirs("build", exist_ok=True)
            with working_dir("build"):
                command = ["cmake", "..", "-DAUTD_LOCAL_TEST=ON"]
                if config.cmake_extra is not None:
                    for cmd in config.cmake_extra:
                        command.append(cmd)
                subprocess.run(command).check_returncode()
                command = ["cmake", "--build", "."]
                if config.release:
                    command.append("--config")
                    command.append("Release")
                subprocess.run(command).check_returncode()


def cpp_test(args):
    args.no_examples = True
    cpp_build(args)

    config = Config(args)
    with working_dir("tests"):
        os.makedirs("build", exist_ok=True)
        with working_dir("build"):
            command = ["cmake", ".."]
            if config.cmake_extra is not None:
                for cmd in config.cmake_extra:
                    command.append(cmd)
            if config.is_cuda_available():
                command.append("-DRUN_BACKEND_CUDA=ON")
            subprocess.run(command).check_returncode()
            command = ["cmake", "--build", ".", "--parallel", "8"]
            if config.release:
                command.append("--config")
                command.append("Release")
            subprocess.run(command).check_returncode()

            target_dir = "."
            if config.is_windows():
                target_dir = "Release" if config.release else "Debug"
            subprocess.run(
                [f"{target_dir}/test_autd3{config.exe_ext()}"]
            ).check_returncode()


def check_if_all_native_methods_called():
    defined_methods = set()
    pattern = re.compile(".* (AUTD.*)\\(.*")
    for file in glob("include/autd3/native_methods/autd3capi*.h"):
        with open(file, "r") as f:
            for line in f.readlines():
                result = pattern.match(line)
                if result:
                    defined_methods.add(result.group(1))
    defined_methods.remove("AUTD3_API __attribute__(")

    used_methods = set()
    pattern = re.compile(".*(AUTD.*?)\\(.*")
    for file in glob("include/**/*.hpp", recursive=True) + glob(
        "tests/**/*.cpp", recursive=True
    ):
        with open(file, "r") as f:
            for line in f.readlines():
                result = pattern.match(line)
                if result:
                    used_methods.add(result.group(1))

    unused_methods = defined_methods.difference(used_methods)
    if len(unused_methods) > 0:
        err("Following native methods are defined but not used.")
        for method in unused_methods:
            print(f"\t{method}")
        sys.exit(-1)


def cpp_cov(args):
    check_if_all_native_methods_called()

    config = Config(args)
    if not config.is_linux():
        err("Coverage is only supported on Linux.")
        return

    args.no_examples = True
    cpp_build(args)

    with working_dir("tests"):
        os.makedirs("build", exist_ok=True)
        with working_dir("build"):
            command = ["cmake", "..", "-DCOVERAGE=ON"]
            if config.cmake_extra is not None:
                for cmd in config.cmake_extra:
                    command.append(cmd)
            if config.is_cuda_available():
                command.append("-DRUN_BACKEND_CUDA=ON")
            subprocess.run(command).check_returncode()
            command = ["cmake", "--build", ".", "--parallel", "8"]
            if config.release:
                command.append("--config")
                command.append("Release")
            subprocess.run(command).check_returncode()

            target_dir = "."
            if config.is_windows():
                target_dir = "Release" if config.release else "Debug"
            subprocess.run(
                [f"{target_dir}/test_autd3{config.exe_ext()}"]
            ).check_returncode()

            with working_dir("CMakeFiles/test_autd3.dir"):
                subprocess.run(
                    ["lcov", "-d", ".", "-c", "-o", "coverage.raw.info"]
                ).check_returncode()
                command = [
                    "lcov",
                ]
                command.extend(
                    [
                        "-a",
                        "coverage.raw.info",
                        "-o",
                        "coverage.combined.info",
                    ]
                )
                subprocess.run(command).check_returncode()
                command = [
                    "lcov",
                    "-r",
                    "coverage.combined.info",
                    "*/_deps/*",
                    "*/usr/*",
                    "*/tests/*",
                    "*/gain/holo/backend_cuda.hpp",
                    "-o",
                    "coverage.info",
                ]
                subprocess.run(command).check_returncode()
                if args.html:
                    command = [
                        "genhtml",
                        "-o",
                        "html",
                        "--num-spaces",
                        "4",
                        "coverage.info",
                    ]
                    subprocess.run(command).check_returncode()


def cpp_run(args):
    args.no_examples = False
    cpp_build(args)

    config = Config(args)
    if config.is_windows():
        target_dir = "Release" if args.release else "Debug"
    else:
        target_dir = "."

    subprocess.run(
        [f"examples/build/{target_dir}/{args.target}{config.exe_ext()}"]
    ).check_returncode()


def cpp_clear(_):
    with working_dir("."):
        rmtree_f("lib")
        rmtree_f("bin")
        rmtree_f("examples/build")
        rmtree_f("tests/build")


def util_update_ver(args):
    version = args.version

    with working_dir("."):
        with open("CMakeLists.txt", "r") as f:
            content = f.read()
            content = re.sub(
                r"^project\(autd3 VERSION (.*)\)",
                f"project(autd3 VERSION {version})",
                content,
                flags=re.MULTILINE,
            )
        with open("CMakeLists.txt", "w") as f:
            f.write(content)

        with open("include/autd3.hpp", "r") as f:
            content = f.read()
            content = re.sub(
                r'^static inline std::string version = "(.*)";',
                f'static inline std::string version = "{version}";',
                content,
                flags=re.MULTILINE,
            )
        with open("include/autd3.hpp", "w") as f:
            f.write(content)

        with open("examples/CMakeLists.txt", "r") as f:
            content = f.read()
            content = re.sub(
                r"v(.*)/autd3-v(\d*\.\d*\.\d*)",
                f"v{version}/autd3-v{version}",
                content,
                flags=re.MULTILINE,
            )
        with open("examples/CMakeLists.txt", "w") as f:
            f.write(content)


def util_gen_wrapper(_):
    fetch_submodule()
    generate_wrapper()


def command_help(args):
    print(parser.parse_args([args.command, "--help"]))


if __name__ == "__main__":
    with working_dir(os.path.dirname(os.path.abspath(__file__))):
        parser = argparse.ArgumentParser(description="autd3 library build script")
        subparsers = parser.add_subparsers()

        # build
        parser_cpp_build = subparsers.add_parser("build", help="see `build -h`")
        parser_cpp_build.add_argument(
            "--release", action="store_true", help="release build"
        )
        parser_cpp_build.add_argument(
            "--no-examples", action="store_true", help="skip building examples"
        )
        parser_cpp_build.add_argument("--cmake-extra", help="cmake extra args")
        parser_cpp_build.set_defaults(handler=cpp_build)

        # test
        parser_cpp_test = subparsers.add_parser("test", help="see `test -h`")
        parser_cpp_test.add_argument(
            "--release", action="store_true", help="release build"
        )
        parser_cpp_test.add_argument("--cmake-extra", help="cmake extra args")
        parser_cpp_test.set_defaults(handler=cpp_test)

        # cov
        parser_cpp_cov = subparsers.add_parser("cov", help="see `cov -h`")
        parser_cpp_cov.add_argument(
            "--release", action="store_true", help="release build"
        )
        parser_cpp_cov.add_argument(
            "--html", action="store_true", help="generate html report", default=False
        )
        parser_cpp_cov.add_argument("--cmake-extra", help="cmake extra args")
        parser_cpp_cov.set_defaults(handler=cpp_cov)

        # run
        parser_cpp_run = subparsers.add_parser("run", help="see `run -h`")
        parser_cpp_run.add_argument("target", help="binary target")
        parser_cpp_run.add_argument(
            "--release", action="store_true", help="release build"
        )
        parser_cpp_run.set_defaults(handler=cpp_run)

        # clear
        parser_cpp_clear = subparsers.add_parser("clear", help="see `clear -h`")
        parser_cpp_clear.set_defaults(handler=cpp_clear)

        # util
        parser_util = subparsers.add_parser("util", help="see `util -h`")
        subparsers_util = parser_util.add_subparsers()

        # util update version
        parser_util_upver = subparsers_util.add_parser(
            "upver", help="see `util upver -h`"
        )
        parser_util_upver.add_argument("version", help="version")
        parser_util_upver.set_defaults(handler=util_update_ver)

        # util update version
        parser_util_gen_wrap = subparsers_util.add_parser(
            "gen_wrap", help="see `util gen_wrap -h`"
        )
        parser_util_gen_wrap.set_defaults(handler=util_gen_wrapper)

        # help
        parser_help = subparsers.add_parser("help", help="see `help -h`")
        parser_help.add_argument("command", help="command name which help is shown")
        parser_help.set_defaults(handler=command_help)

        args = parser.parse_args()
        if hasattr(args, "handler"):
            args.handler(args)
        else:
            parser.print_help()
