
struct answer { 
    std::string text;
    int dialogLink;
};


struct Dialog {
    std::string question;
    std::vector <answer> variants;

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
std::vector<Dialog> currentDialogs;

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

    Dialog d2("Голос продолжае'");

    answer v4;
    v4.text = "Что происходит?";
    v4.dialogLink = 3;
    d2.variants.push_back(v4);

    dialogs.push_back(d2);

    currentDialogs.push_back(dialogs[0]);
}

void renderContent() {
    
    MenuDrawer();

    SIZE textSize;

    int textX = 100;
    int textY = 100; 

    for (const auto& narrative : narratives) {
        TextOutA(window.context, textX, textY,
            narrative.text.c_str(), (int)narrative.text.length());
        textY += 30;
    }

    
    if (!currentDialogs.empty()) {
        textY += 20;

        const Dialog& d = currentDialogs.back(); 
        TextOutA(window.context, 100, textY,
            d.question.c_str(), (int)d.question.length());
        textY += 30;

        for (size_t i = 0; i < d.variants.size(); ++i) {
            std::string variantText = std::to_string(i + 1) + ": " + d.variants[i].text;
            TextOutA(window.context, textX + 20, textY,
                variantText.c_str(), (int)variantText.length());
            textY += 30;
        }
    }


}

void selectVariant(int variantIndex) {
    if (!currentDialogs.empty()) {
        const Dialog& currentDialog = currentDialogs.back();
        if (variantIndex < currentDialog.variants.size()) {
            int nextDialogIndex = currentDialog.variants[variantIndex].dialogLink;
            if (nextDialogIndex < dialogs.size()) {
                currentDialogs.push_back(dialogs[nextDialogIndex]); 
            }
        }
    }
   renderContent();
}


void handleInput()
{
    if (GetAsyncKeyState('1') & 0x0001) {
        selectVariant(0);
    }
    else if(GetAsyncKeyState('2') & 0x0001)
    {
        selectVariant(1);
    }
    
}


