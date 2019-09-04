#!/bin/bash

URL_TGZ="https://art-yalla.jfrog-lab.com/artifactory/conan-repo/_/robotapp/0.1/_/0/package/6a3e26c13846e76b140eb8d0801e0fb14640f3e1/0/conan_package.tgz"
APP_PATH="bin/robotapp"

parent_folder=$(pwd)
check_folder=$(pwd)/check
execute_folder=$(pwd)/execute


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
    echo "folder: $folder"
    binary_content=$(<"$folder/$APP_PATH")
    echo "$binary_content"  # return value
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
    deploy_content="$(read_binary_content "$execute_folder")"
    echo "deploy content: $deploy_content"
    check=1

    while [ $check -gt 0 ]
    do
        sleep 5
        run_check_directory
        cd "$parent_folder" || exit
        new_deploy_content="$(read_binary_content "$check_folder")"
        echo "new deploy content: $new_deploy_content"

        if [ "$deploy_content" -ne "$new_deploy_content" ]; then
            echo "KIIIIIIIIIIIIIIIIIIIILLLLL"
            kill -9 $app_pid
            check=0
        fi
        echo $check
    done
done