
int start;

bool isMoveActive = false; 

float finalS;

char whatKeyPressed() {
	if (GetAsyncKeyState('W') & 0x0001) 
		return 'W';
	if (GetAsyncKeyState('A') & 0x0001)
		return 'A';
	if (GetAsyncKeyState('S') & 0x0001)
		return 'S';
	if (GetAsyncKeyState('D') & 0x0001)
		return 'D';
	if (GetAsyncKeyState('1')) {
		return '1';
	}
	return '\0';  // Если ничего не нажато, возвращаем пустой символ
}

point3d player_dodge_ofs = { 0,0,0 };
point3d starfield_angles = { 0,0,0 };

void fightMove(point3d& p) {

	
	//player_dodge_ofs ; 

	float T;
	float moveTimer = 4;
	float currentV;
	float currentS;
	float currentA;
	float F = -1.5;
	bool isMoveStateBack; 

	static char SYM[2] = {0};

	char key = whatKeyPressed();

	if (!isMoveActive && key != '\0') // Проверка срабатывает если кнопка нажимается хотя бы один раз
	{
	//*SYM = { whatKeyPressed() };
		//if (!A || !S || !D) {
				//W = true;
		if (!isMoveActive)
		{
			//SYM[0] = 'A';
			SYM[0] = key;
			SYM[1] = '\0';


			start = currentTime;
			isMoveActive = true;
		}
		//}
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
				
				//move(p, 0, currentS, 0);
			}
			else {
				currentA = F * (T / 1000 - moveTimer / 2);
				//F = 1;
				//currentA = -currentA;
				//move(p, 0, 2 * finalS - currentS, 0);
			}
			//
			char temp[64] = {0};
			//_ultoa_s(moveTimer - T / 1000, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
			//_ultoa_s(currentA, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
			//_ultoa_s(moveTimer - T / 1000, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
			//TextOutA(window.context, window.width / 2, window.height / 2., (LPCSTR)temp, strlen(temp));
			/*SaveCurrentState();*/
		}
		else {
			isMoveActive = false;
		}



		
		/*SaveCurrentState();*/
	}
	
	move(p, player_dodge_ofs);
}

bool isCopyInit = false;

void morph(point3d& p, Constellation &con, Constellation& mor)
{
	//for (int i = 0; i < 1; i++) {
		//con.starsCords.push_back({1,1,1});
		//con.starsHealth.push_back(1);
	//}

	char key = whatKeyPressed();

	if (whatKeyPressed() == '1') {
		TextOutA(window.context, window.width / 2 + 20, window.height / 2 + 50., "Работаем!", 10);

		if (con.starsCords.size() == mor.starsCords.size()) {
			for (int i = 0; i < con.starsCords.size(); i++)
			{
				con.starsCords[i] = lerp(con.starsCords[i], mor.starsCords[i], 0.003);
			}
		}

		if (con.starsCords.size() > mor.starsCords.size()) {

			std::vector<point3d> surplus(con.starsCords.size() - mor.starsCords.size(), con.starsCords[con.starsCords.size()]);


			for (int i = 0; i < mor.starsCords.size() ; i++)
			{
				con.starsCords[i] = lerp(con.starsCords[i], mor.starsCords[i], 0.003);
			}
		}

		//point3d missing = con.startStarsCords[con.starsCords.size() - 1];
		
		if (con.starsCords.size() < mor.starsCords.size()) {

			//std::vector<point3d> missing(mor.starsCords.size() - con.starsCords.size(), con.starsCords[con.starsCords.size()-1]);


			for (int i = 0; i < con.starsCords.size() - 1; i++)
			{
				con.starsCords[i] = lerp(con.starsCords[i], mor.starsCords[i], 0.003);
			}
			
			int j = 0;
			//for (int i = con.startStarsCords.size(); i < mor.starsCords.size(); i++, j++)
			{
				//missing[j];
				//mor.starsCords[i];
				//con.starsHealth.push_back(1);
				//lerp(con.starsCords[i], mor.starsCords[i], 0.003);
				//lerp(missing[j], mor.starsCords[i], 0.003);

				//con.starsCords.push_back(con.starsCords[con.starsCords.size() - 1]);
				//con.starsCords.resize(con.startStarsCords.size() + j);
				//con.starsCords.push_back(lerp(missing, mor.starsCords[1], 0.003));
			}
		}
	}
}


void morphWepon(std::vector <point3d>& starArray1, std::vector<std::vector<float>> starEdges1, std::vector <point3d>& starArray2, std::vector<std::vector<float>> starEdges2, std::vector <point3d>& morphArray, std::vector <std::vector <float>> Morp_indices, std::vector <float> Morp_health)
{
	morphArray.clear();
	Morp_indices.clear();
	Morp_health.clear();
	int sz1 = starArray1.size();
	int sz2 = starArray2.size();
	int sz3 = starEdges1.size();
	int sz4 = starEdges2.size();
	if (sz1 < sz2)

	{
		for (int i = 0; i < sz1;i++)
		{
			float morphSpeed = 0.01;
			morphArray.push_back(lerp(starArray1[i], starArray2[i], (0.5 + 0.5 * sin(currentTime * morphSpeed))));
		}
	}
	else
	{
		for (int i = 0; i < sz2;i++)
		{
			float morphSpeed = 0.01;
			morphArray.push_back(lerp(starArray1[i], starArray2[i], (0.5 + 0.5 * sin(currentTime * morphSpeed))));
		}
	}
	if (sz3 > sz4)
	{
		for (float i = 0; i < sz4;i++)
		{
			Morp_indices.push_back({ i, i + 1 });
		}
	}
	else
	{
		for (float i = 0; i < sz3;i++)
		{
			Morp_indices.push_back({ i, i + 1 });
		}
	}
	for (int i = 0; i < 15;i++)
	{
		Morp_health.push_back(1);
	}

	//drawСonstellation(morphArray, Morp_indices, Morp_health); Отключено
}









//

	/*
	if (isMoveActive) {

		T = timeGetTime() - start;

		if (T / 1000 < 3) {
			char temp[64] = { 0 };


			_ultoa_s(T / 1000, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
			TextOutA(window.context, window.width / 2, window.height / 2., (LPCSTR)temp, strlen(temp));
			TextOutA(window.context, window.width / 2 + 20, window.height / 2., SYM, 1);

		}
		else {
			isMoveActive = false;
		}
	}
	*/
	

////////////////////////////


	//if (GetAsyncKeyState('A'))
	//{
	//	move(p, currentV, 0, 0);
	//};

	//if (GetAsyncKeyState('S'))
	//{
	//	move(p, 0, -currentV, 0);

	//};

	//if (GetAsyncKeyState('D'))
	//{
	//	move(p, -currentV, 0, 0);

	//};










	//int time = timeGetTime() - startTime;
	//std::string str = "hi" + time;
	//char temp[7];


	//_ultoa_s(time / 1000, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt

	//TextOutA(window.context, window.width / 2, window.height / 2., (LPCSTR)temp, strlen(temp));

	//TextOutA(window.context, window.width / 2, window.height / 2., str.c_str(), str.size()); //Вывод текста














//void battleMove(point3d& b) {
//	char a[] = "qweqweqwe";
//	float moveStep = -100;
//
//		
//	if (GetAsyncKeyState('W'))
//	{
//		move(b, 0, moveStep, 0);
//	};
//	if (GetAsyncKeyState('A'))
//	{
//		move(b, moveStep, 0, 0);
//
//	};
//	if (GetAsyncKeyState('S'))
//	{
//		move(b, 0, -moveStep, 0);
//
//	};
//	if (GetAsyncKeyState('D'))
//	{
//		move(b, -moveStep, 0, 0);
//
//	};
//}
	//using namespace std::this_thread;     // sleep_for, sleep_until
	//using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
	//using std::chrono::system_clock;
//#include <chrono>
//#include <thread>
