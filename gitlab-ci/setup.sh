#!/bin/bash

echo "preparation script"
pwd

cp ./gitlab-ci/iopsys-supervisord.conf /etc/supervisor/conf.d/
cp -rf ./test/files/* /

