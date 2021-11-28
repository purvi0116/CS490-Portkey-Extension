#!/bin/bash

# Pre copy  Container Migration between 2 VM's on same PM (diskless migration)

dest_ip=192.168.122.11
source_ip=192.168.122.139
pid_cont=$1
img_dir_src=/dev/shm/alak_mongodb/diskless/
img_dir_dest=/dev/shm/alak_mongodb/diskless/
cont_name=container1 # modify this to the contianer name being migrated
root_pwd_dest="" # add dest VM root password
root_pwd_src="" # add src VM root password
echo started migration

# Start page server
sshpass -p $root_pwd_dest ssh root@$dest_ip -C "criu page-server --images-dir /dev/shm/alak_mongodb/diskless/ --port 8888 --auto-dedup" &

# Pre dump 1 round
x=`(time sshpass -p $root_pwd_src ssh root@$source_ip -C "runc checkpoint --pre-dump --image-path /dev/shm/alak_mongodb/diskless/ --work-path /dev/shm/alak_mongodb/diskless/ --page-server $dest_ip:8888 --tcp-established $cont_name") 2>&1|grep real`
echo "predump time = $x seconds"

# criu page-server is "one shot" service, you need to run it for every migration, and it exits as soon as all pages are transferred
sshpass -p $root_pwd_dest ssh root@$dest_ip -C "criu page-server --images-dir /dev/shm/alak_mongodb/diskless/ --port 8888 --auto-dedup" &

# checkpoint aka stop and copy over page server
sshpass -p $root_pwd_src ssh root@$source_ip -C "runc checkpoint --image-path /dev/shm/alak_mongodb/diskless/ --work-path /dev/shm/alak_mongodb/diskless/ --page-server $dest_ip:8888 --tcp-established $cont_name"

# copy the remaining files
sshpass -p $root_pwd_src ssh root@$source_ip -C "sshpass -p root1234 scp /dev/shm/alak_mongodb/diskless/*.img /dev/shm/alak_mongodb/diskless/*.json root@$dest_ip:/dev/shm/alak_mongodb/diskless/."

# Restore on destination
sshpass -p $root_pwd_dest ssh root@$dest_ip -C "cd /home/purvi/my_container && runc --debug --log log.txt restore --image-path $img_dir_dest $cont_name && echo OK_restore"


exit 0
