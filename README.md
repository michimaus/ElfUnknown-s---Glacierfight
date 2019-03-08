# ElfUnknown-s---Glacierfight

Duican Mihnea - Ionut 314CA

I used two defined structs datatype in order to have stored all the information 
about the elves and about the map: "elf_stat" with one pointer - char for name
and six integers for positions X and Y, HP, gvloves, stamina and the number of 
oponents one elf has taken out and respectively "on_ice" with three integers for
hight, gloves and cheking if an elf in a specific cell.

The function "read_map" is used to read the bidimensional array for the glaciar 
and the information regarding the elves ( map and v_evf ). Another function 
"elf_on_ice" is used to check if an elf is in the radius of the glaciar and to 
establish its position and its gloves. Those variables are dynamically allocated.

After this the program is getting the comands and one specific function is
called for each input:

* "moving_elf" - moves the elf accross the glaciar, according with fasez U, L,
D, R and it is working in association with function "next_position" wich is going
to solve the cases of a duel, the casees when is there enaugh stamina or not
or the casees when an elf is about to fall of the glaciar.

* "snow_storm" - extract the needed values from the input number and using 
bitwise operations and an 8-bit mask and snowballs any elvese placed in the 
storm radius at that time.

* "melting" - reduces the size of the glaciar and the elves on the edge get wet
while the others get an stamina boost. Given the new size of the glaciar and in 
order to use as little memory as possible all the positions are moved one cell 
to the upper - left diagonal. The last two rows will have no use so they will get 
freed and the rest will get realocated.

* "elf_ranking" - bublesort the players by their status (wet / dry), by the
number of oponents they have taken out and by their name.
