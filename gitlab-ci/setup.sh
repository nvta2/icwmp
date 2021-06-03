#!/bin/bash

echo "preparation script"
pwd

cp ./gitlab-ci/iopsys-supervisord.conf /etc/supervisor/conf.d/
cp -rf ./test/files/* /

# copy schema for validation test
cp -r ./schemas/ubus/*.json /usr/share/rpcd/schemas/

