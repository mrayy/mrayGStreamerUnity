#!/bin/bash

cd $(dirname $(realpath $0))
sudo docker-compose -f docker/docker-compose.yml run mray-android $@
