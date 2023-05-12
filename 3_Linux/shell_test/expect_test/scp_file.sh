#!/bin/expect

set timeout 30

if { $argc < 2 } {
	puts "worng！ Set argv1 and argv2."
	puts "argv2 such as: 192.168.101.223:/home/ceitsa/file
}
set self_file [lindex $argv 0]
set obj_file [lindex $argv 1]

spawn scp -P 10022 $self_file ceitsa@$obj_file  

expect -re "(yes/no)?" { send "yes\r" }
expect -re "password:" { send "Sgw1234!\r" }
interact 

#sed -i 's/\r//' uninstall.sh	文件格式转换


#./scp_expect test223 192.168.101.222:/home/ceitsa/rules_set/