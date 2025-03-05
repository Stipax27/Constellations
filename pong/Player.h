enum Elements
{
	Fire, Earth, Air, Water
};

enum Trups
{
	BlackHole, Quasar, Nebula
};

enum TimeWorld
{
	Hourglass
};

enum Colors
{
	Red, orange, yellow, green, blue, indigo, violet
};

class Player
{
public:
	int Weapon;
	int Trap;



	Player() : Trap(), Weapon() {
		std::cin >> this->Trap;
		std::cin >> this->Weapon;

	}

	Player(int a) : Weapon(a) {}

	void print() {

		std::cout << this->Weapon;
	}

	void Elements() {
		point3d p = { 0.0f, 0.0f, 0.0f };
		switch (this->Weapon)
		{
		case Fire:
			 drawer::drawPoint(p);
			break;

		case Earth:
			std::cout << "EARTH\n";
			break;

		case Air:
			std::cout << "AIR\n";
			break;

		case Water:
			std::cout << "WATER\n";
			break;
		}
	}


	void Trups() {
		switch (this->Trap)
		{
		case 0:
			std::cout << "BlackHole\n";
			break;

		case 1:
			std::cout << "Quasar\n";
			break;

		case 2:
			std::cout << "Nebula\n";
			break;
		}
	}

};


void drowInstruments(point3d& p, Player& USER)
{
	USER.Elements();
	USER.Trups();
}