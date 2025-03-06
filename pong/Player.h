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
	Red, Orange, Yellow, Green, Blue, Indigo, Violet
};

enum Inventory
{
	gold, emotions, Stardust, Potion, 
};

class Player
{
public:
	int Weapon;
	int Trap;
	int Color;
	int Hourglass;
	int Inventory;



	Player() : Trap(), Weapon(), Color(), Hourglass(), Inventory() {

		std::cin >> this->Trap;
		std::cin >> this->Weapon;
		std::cin >> this->Color;
		std::cin >> this->Hourglass;
		std::cin >> this->Inventory;
	}

	Player(int a) : Weapon(a) {}

	void print() {

		std::cout << this->Weapon;
	}

	void drawElement() {
		using FuncPtr = void(*)();
		std::array<FuncPtr, 4> elementFunctions = {
			[]() { std::cout << "Sword\n"; }, // Fire
			[]() { std::cout << "Shield\n"; }, // Earth
			[]() { std::cout << "Bow\n"; }, // Air
			[]() { std::cout << "Book\n"; }  // Water
		};

		if (this->Weapon >= 0 && this->Weapon < elementFunctions.size()) {
			elementFunctions[this->Weapon]();
		}
	}

	void drawTrap() {
		using FuncPtr = void(*)();
		std::array<FuncPtr, 3> trapFunctions = {
			[]() { std::cout << "BlackHole\n"; }, // BlackHole
			[]() { std::cout << "Quasar\n"; },    // Quasar
			[]() { std::cout << "Nebula\n"; }      // Nebula Туманность
		};

		if (this->Trap >= 0 && this->Trap < trapFunctions.size()) {
			trapFunctions[this->Trap]();
		}
	}

	void drawColor() {
		using FuncPtr = void(*)();
		std::array<FuncPtr, 7> colorFunctions = {
			[]() { std::cout << "Red\n"; },
			[]() { std::cout << "Orange\n"; },
			[]() { std::cout << "Yellow\n"; },
			[]() { std::cout << "Green\n"; },
			[]() { std::cout << "Blue\n"; },
			[]() { std::cout << "Indigo\n"; },
			[]() { std::cout << "Violet\n"; }
		};

		if (this->Color >= 0 && this->Color < colorFunctions.size()) {
			colorFunctions[this->Color]();
		}
	}

	void drawTimeWorld() {
		using FuncPtr = void(*)();
		std::array<FuncPtr, 4> timeFunctions = {
			[]() { std::cout << "The 1st epoch\n"; },
			[]() { std::cout << "The 2nd epoch\n"; },
			[]() { std::cout << "The 3rd epoch\n"; },
			[]() { std::cout << "The 4th epoch\n"; },
		};

		if (this->Hourglass >= 0 && this->Hourglass < timeFunctions.size()) {
			timeFunctions[this->Hourglass]();
		}
	}

	void drawInventory() {
		using FuncPtr = void(*)();
		std::array<FuncPtr, 4> inventoryFunctions = {
			[]() { std::cout << "golden sparkles\n"; },
			[]() { std::cout << "Skilset emotions\n"; },
			[]() { std::cout << "Stardust baff emotions\n"; },
			[]() { std::cout << "Potion(atk,hp,time)\n"; },
		};

		if (this->Inventory >= 0 && this->Inventory < inventoryFunctions.size()) {
			inventoryFunctions[this->Inventory]();
		}
	}

};


void drawInstruments(Player& USER) {
	USER.drawElement();
	USER.drawTrap();
	USER.drawColor();
	USER.drawTimeWorld();
	USER.drawInventory();
}