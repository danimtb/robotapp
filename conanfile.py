from conans import ConanFile, CMake


class RobotAppConan(ConanFile):
    name = "robotapp"
    version = "0.1"
    license = "MIT"
    description = "Robot application"
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "CMakeLists.txt", "app*", "LICENSE"
    generators = "cmake"
    requires = "gopigo/2.4.2", "Seasocks/1.3.2"
    default_options = "gopigo:shared=True", "Seasocks:shared=True"
    keep_imports = True

    def imports(self):
        self.copy("*")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*", src="bin", dst="bin")
        self.copy("*", src="lib", dst="bin", symlinks=True)

    def deploy(self):
        self.copy("robotapp*", src="bin", dst="bin")
        self.copy_deps("*", src="lib", dst="bin")
