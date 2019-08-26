#!/bin/bash

set -e
set -x

pip install conan --upgrade
pip install conan_package_tools

conan user
