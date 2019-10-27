# heat-control-v2

Contains implementation of the HeatControl, made with platform IO
(mostly helpsto use VS and split implementation into multiple files, comparing to Ardiono IDE).

How to upload firware to the sep chip: 
1. Use OTA (if devide is online: /update - will open upload form)
2. Use USB to TTL converted. Ex: PL2303
 - sample converter: https://www.aliexpress.com/item/32757800387.html
 - driver download link (installs as COM5 by default): http://www.prolific.com.tw/US/ShowProduct.aspx?p_id=225&pcid=41
 When connecting, do not foeget connect Rx->Tx, Tx -> Rx on board and cable.

 This project to work requires upper version of ESP8266, than supplied with platform io.