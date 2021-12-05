#!/bin/sh

#build image with tag

echo Enter tag

# shellcheck disable=SC2162
read tag

echo Building docker image frontendinterface:"$tag"
echo Running docker container from image frontendinterface:"$tag"

docker run --rm -it -v "$HOME"/Developer/nitcbase/B18_Code/Files:/opt/B18_Code/Files -v "$HOME"/Developer/nitcbase/B18_Code/Disk:/opt/B18_Code/Disk frontendinterface:"$tag"