

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

//void initDialogData()
//{
//	{
//		Dialog Dialog();
//		d.question = "blablablablablablablablablablabla";
//		answer v;
//
//		v.text = "var1";
//		v.dialogLink = 1;
//		d.variants.push_back(v);
//		v.text = "var2";
//		v.dialogLink = 2;
//		d.variants.push_back(v);
//		v.text = "var3";
//		v.dialogLink = 3;
//		d.variants.push_back(v);
//		dialog.push_back(d);
//	}
//
//
//	{
//		Dialog s;
//		s.question = "Meow meow meow meow meow meow";
//		answer v;
//
//		v.text = "var1";
//		v.dialogLink = 1222;
//		s.variants.push_back(v);
//		v.text = "var2";
//		v.dialogLink = 1110;
//		s.variants.push_back(v);
//		dialog.push_back(s);
//	}
//
//
//	int a = 0;
//}
