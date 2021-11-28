#!/bin/bash

# For Stop and Copy Type Container Migration between 2 VM's on same PM (diskless migration)

#if [ "$#" -ne 2 ]; then
#  echo "wrong parameter list\n"
#  echo " ./stop_and_copy.sh pid_container_to_migrate"
#  exit 1
#fi
dest_ip=192.168.122.11
source_ip=192.168.122.139
img_dir_src=/dev/shm/alak_mongodb/diskless/
img_dir_dest=/dev/shm/alak_mongodb/diskless/
cont_name=cont2  # add contianer name being migrated
root_pwd_dest="" # add dest VM root password
root_pwd_src="" # add src VM root password

echo started migration

# Start page server
sshpass -p $root_pwd_dest ssh root@$dest_ip -C "criu page-server --images-dir /dev/shm/alak_mongodb/diskless/ --port 8888 --auto-dedup" &


# checkpoint aka stop and copy over page server 
# this copies only user memory image files i.e pagemap.img and pages.img
sshpass -p $root_pwd_src ssh root@$source_ip -C "runc checkpoint --image-path /dev/shm/alak_mongodb/diskless/ --work-path /dev/shm/alak_mongodb/diskless/ --page-server $dest_ip:8888 --tcp-established $cont_name"

# copy the remaining files (need to setup root pwd for this)
sshpass -p $root_pwd_src ssh root@$source_ip -C "sshpass -p root1234 scp /dev/shm/alak_mongodb/diskless/*.img /dev/shm/alak_mongodb/diskless/*.json root@$dest_ip:/dev/shm/alak_mongodb/diskless/."

# Restore on destination
sshpass -p $root_pwd_dest ssh root@$dest_ip -C "cd /home/purvi/my_container && runc --debug --log log.txt restore --image-path $img_dir_dest $cont_name && echo OK_restore"


exit 0
