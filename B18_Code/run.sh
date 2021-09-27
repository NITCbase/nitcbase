#!/bin/sh
echo Running docker container from image xfs

docker run --rm -it -v "$HOME"/nitcbase/B18_Code/Files:/opt/B18_Code/Files -v "$HOME"/nitcbase/B18_Code/Disk:/opt/B18_Code/Disk xfs