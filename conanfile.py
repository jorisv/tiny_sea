from conans import ConanFile, tools, CMake


class TinySeaConan(ConanFile):
    name = "tiny_sea"
    version = "0.1.0"
    author = "Joris Vaillant (joris.vaillant@gmail.com)"
    license = "GPL-3.0-only"
    url = "https://github.com/jorisv/tiny_sea"
    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto",
    }
    description = (
        "TinySea provide functions to compute "
        "sailing boat routing based on multiple criteria"
    )
    settings = "os", "compiler", "build_type", "arch"
    requires = ("eigen/[^3.3]",)
    options = {"shared": [True, False], "build_tests": [True, False]}
    default_options = {
        "shared": False,
        "build_tests": True,
        "gtest:shared": True,
        "gtest:build_gmock": False,
    }
    generators = "cmake", "virtualrunenv"

    def configure(self):
        tools.check_min_cppstd(self, "17")

    def build_requirements(self):
        if self.options.build_tests:
            self.build_requires("gtest/[^1.10]", force_host_context=True)

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure(defs={"TINY_SEA_BUILD_TESTS": self.options.build_tests})
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()
        if tools.get_env("CONAN_RUN_TESTS", True):
            cmake.test()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["tiny_sea"]
