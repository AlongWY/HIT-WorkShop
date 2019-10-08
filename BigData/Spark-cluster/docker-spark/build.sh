#!/bin/bash

set -e

TAG=latest

build() {
    NAME=$1
    IMAGE=alongwy/$NAME:$TAG
    cd $([ -z "$2" ] && echo "./$NAME" || echo "$2")
    echo '--------------------------' building $IMAGE in $(pwd)
    docker build -t $IMAGE .
    cd -
}

build spark-base
build spark-master
build spark-worker
#build submit
#build java-template template/java
#build python-template template/scala
#build python-template template/python
