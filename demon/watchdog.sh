#!/bin/bash

 
 



while [ 0 -eq 0 ] 
do

 sleep 5


 VER="$( cat /var/www/html/config/gxh_version )" 


   
 razem="$(ps ax | grep -v grep | grep /gxh | wc -l)"
 IS_USB_ERROR="$(  tail -n 10 /var/log/syslog | grep usb_serial_generic_read_bulk_callback | wc -l )"

 
 echo $razem
 echo " | "
 echo $IS_USB_ERROR
 

 if [ $razem -eq 0 ]
 then

    echo "Program is not running "
    /var/www/html/demon/gxhd stop
    sleep 2
   /var/www/html/demon/gxhd start
    
 else
    echo "Program is running"
 fi


 if [ $IS_USB_ERROR -eq 0 ]
 then
    echo "porty aktywne..."     
 else
    echo "porty nieaktywne!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    sleep 3m 
    reboot
 fi

 



currentTime=`date +"%H%M"`
if [ $currentTime -eq "0610" ]; then
   sleep 6m 
   cp /var/www/html/$VER/app_home_log_1 /var/www/html/$VER/app_home_log_2
   cp /var/www/html/$VER/app_home_log /var/www/html/$VER/app_home_log_1
   echo '' > /var/www/html/$VER/app_home_log  
   reboot
fi

 

sleep 1
done


echo "End checking...."















