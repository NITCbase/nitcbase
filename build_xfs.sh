#!/bin/sh

echo Building docker image xfs

docker build . -f Dockerfile_xfs -t xfs