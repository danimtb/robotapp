from conans import ConanFile, CMake


class BlinkAppConan(ConanFile):
    name = "robotapp"
    version = "0.1"
    license = "MIT"
    description = "Robot application"
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "CMakeLists.txt", "main.cpp", "LICENSE"
    generators = "cmake"

    def requirements(self):
        if "arm" in self.settings.arch:
            self.requires("wiringpi/2.46@conan/stable")

    def build(self):
        cmake = CMake(self)
        if "arm" in self.settings.arch:
            cmake.definitions["WIRINGPI"] = True
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("blinkapp*", src="bin", dst="bin")

    def deploy(self):
        self.copy("blinkapp*", src="bin", dst="bin")
