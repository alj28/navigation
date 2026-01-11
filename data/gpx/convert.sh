#! /bin/bash

#   Procedure:
#       -   convert .gpx to the .nmea
#       -   import generated .nmea file to the nmea.org and export back to .nmea file
#

gpsbabel -i gpx -f route_1.txt -x track,faketime=f20100705200000+10 -o nmea -F route_1.nmea



