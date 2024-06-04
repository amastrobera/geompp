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

# Run the docker build command with the appropriate Dockerfile
if [[ "${image}" == "Windows" ]]; then
    echo "building windows"
    docker build -f "${script_dir}/Dockerfile.win" -t "geompp-win" ${script_dir}
elif [[ "${image}" == "Linux" ]]; then
    echo "building linux"
    docker build -f "${script_dir}/Dockerfile.lin" -t "geompp-lin" ${script_dir}
fi
