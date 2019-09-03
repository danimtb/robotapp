from conans import ConanFile, CMake


class RobotAppConan(ConanFile):
    name = "robotapp"
    version = "0.1"
    license = "MIT"
    description = "Robot application"
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "CMakeLists.txt", "app*", "LICENSE"
    generators = "cmake"
    requires = "gopigo/2.4.2"
    default_options = "gopigo:shared=True"

    def imports(self):
        self.copy("*.so", src="bin", dst="bin")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*", src="bin", dst="bin")

    def deploy(self):
        self.copy("robotapp*", src="bin", dst="bin")
        self.copy_deps("*.so", src="bin", dst="bin")
