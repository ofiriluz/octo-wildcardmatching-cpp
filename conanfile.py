from conans import CMake, ConanFile, tools


class OctoWildcardMatchingCPPConan(ConanFile):
    name = "octo-wildcardmatching-cpp"
    version = "1.1.0"
    url = "https://github.com/ofiriluz/octo-wildcardmatching-cpp"
    author = "Ofir Iluz"
    generators = "cmake"
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("gtest/cci.20210126")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.install()
        if str(self.settings.os) != "Windows":
            cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
