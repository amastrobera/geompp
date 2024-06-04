#!/bin/bash

# Function to display usage information
usage() {
    echo "Usage: $0 -image <Windows|Linux>"
    exit 1
}

# Parse the input parameter
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -image) image=$2; shift ;;
        *) echo "Unknown parameter passed: $1"; usage ;;
    esac
    shift
done


# Check if the image parameter is provided and valid
if [[ -z "${image}" ]]; then
    echo "Error: The -image parameter is required."
    usage
elif [[ "${image}" != "Windows" && "${image}" != "Linux" ]]; then
    echo "Error: Invalid value for -image. Accepted values are 'Windows' or 'Linux'."
    usage
fi


# Get the absolute path to the directory containing the script
script_dir=$(dirname "$(readlink -f "$0")")
local_vol="${scirpt_dir}/.."
image_vol="/home/developer/workspace/geompp"

# Run the docker command with the appropriate Dockerfile
if [[ "${image}" == "Windows" ]]; then
    echo "running windows"
    echo "    > tag ${image_tag}"
    echo "    > shared volume ${local_vol}"
    image_tag="geompp-win"
    image_name="geompp-win"
    docker run \
        --rm \
        --name ${image_name} \
        --publish 3232:3232 \
        --interactive \
        --tty \
        --env DISPLAY=$DISPLAY \
        --volume /tmp/.X11-unix:/tmp/.X11-unix \
        --entrypoint /bin/bash \
        --volume ${local_vol}:${image_vol} \
        ${image_tag}:latest

elif [[ "${image}" == "Linux" ]]; then
    echo "running linux"
    echo "    > tag ${image_tag}"
    echo "    > shared volume ${local_vol}"
    image_tag="geompp-lin"
    image_name="geompp-lin"
    docker run \
        --rm \
        --name ${image_name} \
        --publish 3232:3232 \
        --interactive \
        --tty \
        --env DISPLAY=$DISPLAY \
        --volume /tmp/.X11-unix:/tmp/.X11-unix \
        --entrypoint /bin/bash \
        --volume ${local_vol}:${image_vol} \
        ${image_tag}:latest
fi
