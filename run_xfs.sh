#!/bin/sh

echo Running docker container from image xfs

disk_volume_path="$HOME/NITCbase/Files:/opt/NITCbase/Files"

files_volume_path="$HOME/NITCbase/Disk:/opt/NITCbase/Disk"

docker run --rm -it -v "$files_volume_path" -v "$disk_volume_path" xfs