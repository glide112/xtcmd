# xtcmd
Command line tool for Avaya Scopia XT
============================================================
### To start:

First you need to compile "xtcmd" which is a command line tool to connect between XT. "xtcmd" can run standalone. If you want to use a list of commands save in a file to feed xtcmd, you will need either a shell script or a batch file but even though you are on Windows, I would recommend using cygwin and run expect.
Below are the samples.

============================================================
### To compile command
Below is an example of how to compile. For Linux, Mac and Windows, compile can be done using gcc.

$ gcc -o xtcmd xtcmd.c 

============================================================
### Using a script to read commands from a file (Linux and Mac):
cmd.sh : for Linux and Mac

cmd.sh
<br>ーーーーーーーーーーーーーーーーーーーーーーーー<br>
＃!/usr/bin/expect

set fp [open ./test.txt]        <----"./test.txt is a list of commands"

spawn ./xtcmd 192.168.99.17     <----"ex 192.168.99.17 is the ip address of XT"

while {[gets $fp line] >= 0} {

  send "$line\r"

  expect ""

  interact timeout 1 return     <----"if you need to have larger delay(sec) between the commands"<br>
}

send "quit\r"
<br>ーーーーーーーーーーーーーーーーーーーーーーーー<br>

============================================================
### Using a script to read commands from a file (Windows):

You can write a vbscript to read from a file but reccomend using expect from crygwin console.<br>
*xtcmd-Win.zip: compiled on x64 Winodows using gcc 5.0.3

============================================================
### Usage
Please go through the API document provided by Avaya to understand how AT commands communicate.

standalone usage (Linux and Mac):<br>
$ ./xtcmd ADDRESS -d<br>

file reading usage (Linux and Mac):<br>
$ chmod 755 cmd.sh<br>
$ ./cmd.sh<br>

============================================================

written by Yosuke Sawamura
yosuke.sawamura@gmail.com
