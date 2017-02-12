#!/usr/bin/expect

set fp [open ./test.txt]

spawn ./xtcmd 192.168.99.17

while {[gets $fp line] >= 0} {

  send "$line\r"

  expect ""

  interact timeout 1 return
}

send "quit\r"
