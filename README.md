point3d -> Constellation
HealthSystem 
Entity <-  HealthSystem, Constellation, <- point3d

void morph <- Entity(HealthSystem), Constellation <- point3d
void updateMorph <- Entity <- Constellation <- point3d
void fightMove <- Entity <- point3d

PressedKey в Navigation.h (???) в 5 <-ZodiacSign
Weapon.h -> initWeapon -> Sword, shield, bow, staff-> 1,2,3,4 PressedKey
