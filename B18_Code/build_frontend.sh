#!/bin/sh

echo Building docker image frontend

docker build . -f Dockerfile_frontend -t frontend