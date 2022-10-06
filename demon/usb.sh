#!/bin/bash

IS_USB_ERROR="$(  tail -n 20 /var/log/syslog | grep usb_serial_generic_read_bulk_callback | wc -l )"

echo $IS_USB_ERROR
