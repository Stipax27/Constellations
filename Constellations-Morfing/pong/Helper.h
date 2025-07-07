void Helpers() 
{
	std::string curentSignstring = zodiacSignToString(currentEnemyID);
	drawString(curentSignstring.c_str(), window.width / 1.1, window.height / 10., 1, true);
	
	curentSignstring = zodiacSignToString(player_sign);
	drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 7., 1, true);
	
	curentSignstring = "current weapon: " + weapon[(int)current_weapon].name;
	drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 10., 1, true);
	
	drawCurrentElement();
	
	drawString("Weapon selection:\nButton 1 - Sword \nButton 2 - Shield \nButton 3 - Bow ", (1700. / 2560)* window.width, (1100. / 1440)* window.height, .7f, false);
	drawString("Rewind time:\nbutton - E", (500. / 2560)* window.width, (1200. / 1440)* window.height, .7f, false);
	drawString("TUTORIAL:\nTo hit an enemy with a sword,\npress LMB and draw a line along the enemy star\nTo hit with a shield,\npress LMB and draw a line that will draw a circle that attacks stars\nTo hit with a bow,\npress LMB on the star and draw a vector in any direction from the star.", (120. / 2560)* window.width, (110. / 1440)* window.height, .7f, false);
	
	if (GetAsyncKeyState('T') & 0x8000)
	{
		gameState = gameState_::Fight;
	}
}
