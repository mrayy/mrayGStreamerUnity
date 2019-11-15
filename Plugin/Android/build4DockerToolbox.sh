#!/bin/bash

cd $(dirname $(realpath $0))
docker-compose -f docker/docker-compose-toolbox.yml run mray-android $@
