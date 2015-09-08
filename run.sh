RASPID=`/sbin/ifconfig | grep "192." | cut -f 2 -d : | cut -f 1 -d " " | cut -f 4 -d . `

./bin/rpi-motion-detection $RASPID
