#!/bin/bash

sudo apt install curl

echo
echo "*************************************************************************"
echo "INSTALL DOCKER"
echo "*************************************************************************"
echo

curl -sSL "https://get.docker.com/" | sudo sh

echo
echo "*************************************************************************"
echo "INSTALL DOCKER-COMPOSE"
echo "*************************************************************************"
echo

sudo curl -L "https://github.com/docker/compose/releases/download/1.9.0/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose

echo
echo "*************************************************************************"
echo "INSTALLED VERSIONS :"
sudo docker --version
sudo docker-compose --version
echo "*************************************************************************"
echo
