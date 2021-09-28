#!/bin/sh
echo Running docker container from image xfs

disk_volume_path="$HOME/nitcbase/B18_Code/Files:/opt/B18_Code/Files"

files_volume_path="$HOME/nitcbase/B18_Code/Disk:/opt/B18_Code/Disk"

docker run --rm -it -v "$disk_volume_path" -v "$files_volume_path" xfs