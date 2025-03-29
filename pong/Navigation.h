

void fightMove(point3d& p) {
	float moveStep = -1;

	if (GetAsyncKeyState('W'))
	{
		move(p, 0, moveStep, 0);
	};
	if (GetAsyncKeyState('A'))
	{
		move(p, moveStep, 0, 0);
	};
	if (GetAsyncKeyState('S'))
	{
		move(p, 0, -moveStep, 0);

	};
	if (GetAsyncKeyState('D'))
	{
		move(p, -moveStep, 0, 0);

	};
}

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
