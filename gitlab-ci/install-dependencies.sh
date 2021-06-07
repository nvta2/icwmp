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

# install uspd
cd /opt/dev
rm -rf uspd
exec_cmd git clone -b devel https://dev.iopsys.eu/iopsys/uspd.git
cd /opt/dev/uspd
exec_cmd ./gitlab-ci/install-dependencies.sh
exec_cmd ./gitlab-ci/setup.sh
exec_cmd make
exec_cmd cp uspd /usr/sbin/uspd