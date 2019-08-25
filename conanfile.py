from conans import ConanFile, CMake


class BlinkAppConan(ConanFile):
    name = "robotapp"
    version = "0.1"
    license = "MIT"
    description = "Robot application"
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "CMakeLists.txt", "app*", "wrapper*", "LICENSE"
    generators = "cmake"
    requires = "gopigo/2.4.2@conan/stable"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("robotapp*", src="bin", dst="bin")

    def deploy(self):
        self.copy("robotapp*", src="bin", dst="bin")
