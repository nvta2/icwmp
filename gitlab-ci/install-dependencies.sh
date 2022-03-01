#!/bin/bash

echo "install dependencies for unit-test script"
pwd

source ./gitlab-ci/shared.sh

# install required packages
exec_cmd apt update
exec_cmd apt install -y mongodb jq

# install genieacs
exec_cmd npm install -g genieacs@1.2.5
ln -sf /root/.nvm/versions/node/v14.16.1/bin/genieacs-cwmp /usr/sbin/genieacs-cwmp
ln -sf /root/.nvm/versions/node/v14.16.1/bin/genieacs-fs /usr/sbin/genieacs-fs  
ln -sf /root/.nvm/versions/node/v14.16.1/bin/genieacs-ui /usr/sbin/genieacs-ui
ln -sf /root/.nvm/versions/node/v14.16.1/bin/genieacs-nbi /usr/sbin/genieacs-nbi
mkdir -p /data/db

echo "Installing uspd"
install_uspd
