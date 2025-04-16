
struct answer { 
    std::string text;
    int dialogLink;
};


struct Dialog {
    std::string question;
    std::vector<answer> variants;

    Dialog(const std::string& msg) : question(msg) {
        std::cout << "Я: " << question << std::endl;
    }
};


struct NarrativeText {
    std::string text;
    int displayTime; 
};

std::vector<Dialog> dialogs;
std::vector<NarrativeText> narratives;

void initContentData() {
    
    narratives.push_back({ "Вы просыпаетесь в темной комнате...", 2000 });
    narratives.push_back({ "Где-то вдалеке слышны странные звуки.", 2000 });

    
    Dialog d("Вы слышите голос: 'Ты меня слышишь?'");

    answer v1;
    v1.text = "Да, кто ты?";
    v1.dialogLink = 1;
    d.variants.push_back(v1);

    answer v2;
    v2.text = "Нет, это галлюцинация";
    v2.dialogLink = 2;
    d.variants.push_back(v2);

    dialogs.push_back(d);

    
    narratives.push_back({ "Вы пытаетесь встать, но чувствуете слабость.", 2000 });

    
    Dialog d1("Голос продолжает: 'КАК КАКАТ!'");
    answer v3;
    v3.text = "Что происходит?";
    v3.dialogLink = 3;
    d1.variants.push_back(v3);

    dialogs.push_back(d1);
}


void renderContent(HDC hdc) {
    MenuDrawer();

    int yPos = 200;

    for (const auto& narrative : narratives) {
        TextOutA(hdc, 100, yPos, narrative.text.c_str(), (int)narrative.text.length());
        yPos += 20;
    }

    if (!dialogs.empty()) {
        yPos += 20;
        const Dialog& d = dialogs[0];
        TextOutA(hdc, 100, yPos, d.question.c_str(), (int)d.question.length());
        yPos += 20;

        for (size_t i = 0; i < d.variants.size(); ++i) {
            std::string variantText = std::to_string(i + 1) + ": " + d.variants[i].text;
            TextOutA(hdc, 100, yPos, variantText.c_str(), (int)variantText.length());
            yPos += 20;
        }
    }
}