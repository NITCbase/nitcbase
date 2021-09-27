FROM ubuntu:bionic AS  build-base

RUN apt-get update && apt-get install -y build-essential cmake

COPY . /src/B18_Code/

WORKDIR /src/B18_Code/XFS_Interface_Code/

RUN cmake --clean-first . && make

# Use second docker image for the XFS-Executable and Volumes
FROM ubuntu:bionic

WORKDIR /opt/B18_Code/XFS-Interface/

COPY --from=build-base /src/B18_Code/XFS_Interface_Code/XFS-Interface ./

CMD [ "./XFS-Interface" ]


## Use base docker image to compile the XFS-Code
#FROM xfs-base:1.0 AS build
#
#COPY . /src/B18_Code/
#
#WORKDIR /src/B18_Code/XFS_Interface_Code/
#
#RUN cmake --clean-first . && make
#
## Use second docker image for the XFS-Executable and Volumes
#FROM ubuntu:bionic
#
#WORKDIR /opt/B18_Code/XFS-Interface/
#
#COPY --from=build /src/B18_Code/XFS_Interface_Code/XFS-Interface ./
#
#CMD [ "./XFS-Interface" ]