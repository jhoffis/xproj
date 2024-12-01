@echo off
>  C:\script.txt echo open 192.168.0.84
>> C:\script.txt echo xbox
>> C:\script.txt echo xbox
>> C:\script.txt echo cd /e/appz/defaulttest
>> C:\script.txt echo binary
>> C:\script.txt echo put default.xbe
>> C:\script.txt echo bye
start /w %windir%\System32\ftp.exe -s:C:\script.txt
del C:\script.txt
rem cls
exit
