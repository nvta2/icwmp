#!/bin/bash
if [ -d "/opt/dev/bbf" ]; then
	cd /opt/dev/bbf
	./gitlab-ci/setup.sh
	cd -
fi

echo "preparation script"
pwd

mkdir -p /var/run/icwmpd/
rm -rf /etc/supervisor/conf.d/*.conf
cp ./gitlab-ci/iopsys-supervisord.conf /etc/supervisor/conf.d/
cp -rf ./test/files/* /

# copy schema for validation test
cp -r ./schemas/ubus/*.json /usr/share/rpcd/schemas/

