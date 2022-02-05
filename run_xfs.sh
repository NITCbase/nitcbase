#!/bin/sh

echo Running docker container from image xfs

# shellcheck disable=SC2006
project_root=`realpath .`

disk_volume_path="$project_root/Disk:/opt/NITCbase/Disk"
files_volume_path="$project_root/Files:/opt/NITCbase/Files"

docker run --rm -it -v "$files_volume_path" -v "$disk_volume_path" xfs