#!/usr/bin/expect

set timeout 30

if { $argc < 1 } {
	puts "worng！ argv0 is scp file."
}


set self_file [lindex $argv 0]
set obj_file [lindex $argv 1]

spawn scp $self_file root@192.168.101.150:/home

expect {
	"(yes/no" {
		send "yes\r" 
		expect "*assword:" {
			send "$password\r"
		}
	}
	
	"password:" { send "1\r" }
}


interact 

