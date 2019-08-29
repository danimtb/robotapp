FROM conanio/gcc6-armv7hf

RUN pip install conan --upgrade
RUN pip install conan_package_tools
RUN conan user
RUN python build.py