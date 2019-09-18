#!/bin/bash

URL_TGZ="https://art-yalla.jfrog-lab.com/artifactory/conan-repo/_/robotapp/0.1/_/0/package/2ee0aea63e74b4d1932fd84a96052f38be8f7900/0/conan_package.tgz"
APP_PATH="bin/robotapp"

parent_folder=$(pwd)
check_folder=$(pwd)/check
execute_folder=$(pwd)/execute
app_md5=""


run_check_directory()
{
    if [ -d "$check_folder" ]; then
        rm -r "$check_folder"
    fi
    sleep 1
    mkdir "$check_folder"
    sleep 1
    cd "$check_folder" || exit
    sleep 1
    wget "$URL_TGZ"
    tar -xzf conan_package.tgz
    rm conan_package.tgz
    cd "$parent_folder" || exit
}

read_binary_content ()
{
    folder=$1
    md5=$(md5sum "$folder/$APP_PATH")
    app_md5="${md5%% *}"
}


export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$execute_folder/bin
run_check_directory
cd "$parent_folder" || exit
while true
do
    if [ -d "$execute_folder" ]; then
        rm -r "$execute_folder"
    fi
    cp -R "$check_folder" "$execute_folder"
    cd "$execute_folder" || exit
    $APP_PATH &
    app_pid=$!
    sleep 5
    read_binary_content "$execute_folder"
    deploy_content="$app_md5"
    check=1

    while [ $check -gt 0 ]
    do
        sleep 5
        run_check_directory
        cd "$parent_folder" || exit
        read_binary_content "$check_folder"
        new_deploy_content="$app_md5"

        echo "$new_deploy_content"
        echo "$deploy_content"

        if [ "$deploy_content" != "$new_deploy_content" ]; then
            echo "KILL"
            kill -TERM $app_pid
            check=0
        fi
    done
done