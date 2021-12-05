#!/bin/sh

#build image with tag

echo Enter tag

# shellcheck disable=SC2162
read tag

echo Building docker image xfs:"$tag"

docker build . -t xfs:"$tag"

echo Running docker container from image xfs:"$tag"

docker run --rm -it -v "$HOME"/Developer/nitcbase/B18_Code/Files:/opt/B18_Code/Files -v "$HOME"/Developer/nitcbase/B18_Code/Disk:/opt/B18_Code/Disk xfs:"$tag"