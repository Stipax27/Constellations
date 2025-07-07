
int start;

bool isMoveActive = false; 

float finalS;

char PressedKey() {
	if (GetAsyncKeyState('W') & 0x0001) 
		return 'W';
	if (GetAsyncKeyState('A') & 0x0001)
		return 'A';
	if (GetAsyncKeyState('S') & 0x0001)
		return 'S';
	if (GetAsyncKeyState('D') & 0x0001)
		return 'D';
	if (GetAsyncKeyState('1'))
		return '1';
	return '\0';  // Если ничего не нажато, возвращаем пустой символ
}

point3d player_dodge_ofs = { 0,0,0 };
point3d starfield_angles = { 0,0,0 };
point3d milkyway_angles = { 0,0,100 };

std::vector<point3d> swordStars = { {0,0,0}, {0,1,0}, {1,1,0}, {1,0,0} };
std::vector<std::vector<float>> swordEdges = { {0,1}, {1,2}, {2,3} };
Constellation swordConstellation(swordStars, swordEdges);

std::vector<point3d> bowStars = { {0,0,0}, {1,1,0}, {1,-1,0} };
std::vector<std::vector<float>> bowEdges = { {0,1}, {0,2} };
Constellation bowConstellation(bowStars, bowEdges);

std::vector<point3d> shieldStars = { {0,0,0}, {1,0,0}, {1,1,0}, {0,1,0} };
std::vector<std::vector<float>> shieldEdges = { {0,1}, {1,2}, {2,3}, {3,0} };
Constellation shieldConstellation(shieldStars, shieldEdges);

std::vector<point3d> staffStars = { {0,0,0}, {0,2,0} };
std::vector<std::vector<float>> staffEdges = { {0,1} };
Constellation staffConstellation(staffStars, staffEdges);

void morph(Entity& entity, Constellation& target) {
	if (!entity.isMorphing) {
		entity.targetConstellation = &target;
		entity.isMorphing = true;
		entity.morphProgress = 0.0f;

		if (entity.constellation->starsCords.size() != target.starsCords.size()) {
			delete entity.healthSystem;
			entity.healthSystem = new HealthSystem(target.starsCords.size());
		}
	}
}

void updateMorph(Entity& entity) {
	if (!entity.isMorphing || !entity.targetConstellation) return;

	Constellation* current = entity.constellation;
	Constellation* target = entity.targetConstellation;

	size_t minStars = std::min(current->starsCords.size(), target->starsCords.size());
	for (int i = 0; i < minStars; i++) {
		current->starsCords[i] = point3d::lerp(
			current->starsCords[i],
			target->starsCords[i],
			entity.morphProgress
		);
	}

	if (current->starsCords.size() != target->starsCords.size() && entity.morphProgress >= 1.0f) {
		*current = *target;
	}

	entity.morphProgress += 0.003f;

	if (entity.morphProgress >= 1.0f) {
		entity.isMorphing = false;
		entity.targetConstellation = nullptr;
		current->constellationEdges = target->constellationEdges;
	}
}

void fightMove(point3d& p, Entity& playerEntity) {

	float T;
	float moveTimer = 4;
	float currentV;
	float currentS;
	float currentA;
	float F = -1.5;
	bool isMoveStateBack; 

	static char SYM[2] = {0};


	if (!isMoveActive && key != '\0') // Проверка срабатывает если кнопка нажимается хотя бы один раз
	{
		if (!isMoveActive)
		{
			SYM[0] = key;
			SYM[1] = '\0';

			start = currentTime;
			isMoveActive = true;
		}
	}

	if (isMoveActive) {
		T = currentTime - start;
		currentA = F * (moveTimer / 2 - T / 1000);
		currentV = currentA * T / 1000;
		
		currentS = currentV * T / 1000;
		isMoveStateBack = false;
		if (T/1000 < moveTimer) {
			if (T/1000 > (moveTimer / 2))
			{
				isMoveStateBack = true;
			}
			if (!isMoveStateBack) {
				finalS = currentS;

				switch (SYM[0]) {
				case 'W':
					player_dodge_ofs = { 0, currentS, 0 };
					break;
				case 'A':
					player_dodge_ofs = { currentS, 0, 0 };
					break;
				case 'S':
					player_dodge_ofs = { 0, -currentS, 0 };
					break;
				case 'D':
					player_dodge_ofs = { -currentS, 0, 0 };
					break;

				
				}
			}
			else {
				currentA = F * (T / 1000 - moveTimer / 2);
				
			}
			char temp[64] = {0};
		}
		else {
			isMoveActive = false;
		}

	}
	
	p.move(p, player_dodge_ofs);

	switch (PressedKey()) 
						{
						case '1': morph(playerEntity, swordConstellation); break;
						case '2': morph(playerEntity, bowConstellation); break;
						case '3': morph(playerEntity, shieldConstellation); break;
						case '4': morph(playerEntity, staffConstellation); break;
						case '5': morph(playerEntity, *playerEntity.baseConstellation); break;
	}

	updateMorph(playerEntity);
}