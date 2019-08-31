from conan.packager import ConanMultiPackager
import os, re


def get_value_from_recipe(search_string):
    with open("conanfile.py", "r") as conanfile:
        contents = conanfile.read()
        result = re.search(search_string, contents)
    return result

def get_name_from_recipe():
    return get_value_from_recipe(r'''name\s*=\s*["'](\S*)["']''').groups()[0]

def get_version_from_recipe():
    return get_value_from_recipe(r'''version\s*=\s*["'](\S*)["']''').groups()[0]


if __name__ == "__main__":
    name = get_name_from_recipe()
    version = get_version_from_recipe()
    reference = "{0}/{1}".format(name, version)
    conan_username = "conan"
    conan_channel = "stable"
    upload_remote = "https://art-yalla.jfrog-lab.com/artifactory/api/conan/conan-repo"

    builder = ConanMultiPackager(
        username=conan_username,
        channel=conan_channel,
        reference=reference,
        upload=upload_remote,
        remotes=upload_remote
        )
    
    compiler_version = os.environ.get('CONAN_GCC_VERSIONS')
    builder.add(settings={"arch": "armv7hf", "build_type": "Debug", "compiler.libcxx": "libstdc++11", 
                          "compiler.version": compiler_version}, options={}, env_vars={}, build_requires={})
    builder.add(settings={"arch": "armv7hf", "build_type": "Release", "compiler.libcxx": "libstdc++11", 
                          "compiler.version": compiler_version}, options={}, env_vars={}, build_requires={})
    builder.add(settings={"arch": "armv7hf", "build_type": "Debug", "compiler.libcxx": "libstdc++", 
                          "compiler.version": compiler_version}, options={}, env_vars={}, build_requires={})
    builder.add(settings={"arch": "armv7hf", "build_type": "Release", "compiler.libcxx": "libstdc++",
                          "compiler.version": compiler_version}, options={}, env_vars={}, build_requires={})
    builder.run()