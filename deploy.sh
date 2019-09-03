#!/bin/bash

URL_TGZ="https://art-yalla.jfrog-lab.com/artifactory/conan-repo/_/robotapp/0.1/_/0/package/6a3e26c13846e76b140eb8d0801e0fb14640f3e1/0/conan_package.tgz"
APP_PATH="bin/robotapp"


run_check_directory()
{
    if [ -d "check" ]; then
        rm -r check
    fi
    sleep 1
    mkdir check
    sleep 1
    cd check
    sleep 1
    wget "$URL_TGZ"
    tar -xzf conan_package.tgz
    rm conan_package.tgz
}

read_binary_content ()
{
    folder=$1
    binary_content=$(<$folder/$APP_PATH)
    return binary_content
}


export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/execute/bin
run_check_directory
while [ 1 ]
do
    if [ -d "execute" ]; then
        rm -r execute
    fi
    cp -R $(pwd)/check $(pwd)/execute
    cd $(pwd)/execute
    $APP_PATH &
    app_pid=$!
    sleep 5
    deploy_content="$(read_binary_content execute)"
    echo "deploy content:"
    echo $deploy_content
    check=1

    while [ $check -gt 0 ]
    do
        sleep 5
        run_check_directory
        new_deploy_content="$(read_binary_content check)"
        echo "new deploy content:"
        echo $new_deploy_content

        if [ $deploy_content -ne $new_deploy_content ]; then
            kill -9 $app_pid
            check=0
        fi
    done
done