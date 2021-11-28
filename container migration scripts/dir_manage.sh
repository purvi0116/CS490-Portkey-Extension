#!/bin/bash

# Script to create the directory on source and destination VM where the dump files will be saved.
# Currently, for the stop and copy type migration

# VM IP's
source_ip=192.168.122.139
dest_ip=192.168.122.11
root_pwd="" # add the root password 

#Delete previous directories#
sshpass -p $root_pwd ssh root@$source_ip -C "rm -rf /dev/shm/alak_mongodb/diskless/;"
sshpass -p $root_pwd ssh root@$dest_ip -C "rm -rf /dev/shm/alak_mongodb/diskless/;"

#Create required directories
sshpass -p $root_pwd ssh root@$source_ip -C "mkdir -p /dev/shm/alak_mongodb/diskless/;"
sshpass -p $root_pwd ssh root@$dest_ip -C "mkdir -p /dev/shm/alak_mongodb/diskless/;"

echo "OK"
exit 0
##
