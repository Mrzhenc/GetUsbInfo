#!/bin/bash

if [ "x$1" = "x" ]
then 
	exit -1
fi

id=$1
vid=`echo ${id} |cut -d":" -f 1`
pid=`echo ${id} |cut -d":" -f 2`
id_line="Vendor=${vid} ProdID=${pid}"
line_num=`nl -b a /sys/kernel/debug/usb/devices |grep "${id_line}" |awk '{print $1}' |head -n 1`
manuf_line_num=`expr ${line_num} + 1`
product_line_num=`expr ${manuf_line_num} + 1`
manufacture=`sed -n "${manuf_line_num} p" /sys/kernel/debug/usb/devices |grep "Manufacturer" |cut -d":" -f 2 |cut -d"=" -f 2-`
if test "x${manufacture}" = "x";then
	product_line_num=`expr ${product_line_num} - 1`
fi
product=`sed -n "${product_line_num} p" /sys/kernel/debug/usb/devices |grep "Product" |cut -d":" -f 2 |cut -d"=" -f 2-`
echo $manufacture" "$product
