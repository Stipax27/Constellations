

struct answer {
	std::string text;
	int dialogLink;

};

struct Dialog {
	std::string question;
	std::vector <answer> variants;

	Dialog(const std::string& msg) : question(msg)
	{
		std::cout << "ß" << question << std::endl;
	}
};

std::vector<Dialog> dialog;

void initDialogData()
{
	{
		Dialog d("blabla");
		
		answer v1;
		v1.text = "var1";
		v1.dialogLink = 1;
		d.variants.push_back(v1);

		answer v2;
		v2.text = "var2";
		v2.dialogLink = 2;
		d.variants.push_back(v2);

		answer v3;
		v3.text = "...";
		v3.dialogLink = 3;
		d.variants.push_back(v3);

		dialog.push_back(d);

		Dialog d1("bfakjbfiajb00");

	}



	int a = 0;
}


