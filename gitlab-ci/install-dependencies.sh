#!/bin/bash

echo "install dependencies for unit-test script"
pwd

source ./gitlab-ci/shared.sh

# install uspd
cd /opt/dev
rm -rf uspd
exec_cmd git clone -b devel https://dev.iopsys.eu/iopsys/uspd.git
cd /opt/dev/uspd
exec_cmd ./gitlab-ci/install-dependencies.sh
exec_cmd ./gitlab-ci/setup.sh
exec_cmd make
exec_cmd cp uspd /usr/sbin/uspd