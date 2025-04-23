
struct answer { 
    std::string text;
    int dialogLink;
};


struct Dialog {
    std::string question;
    std::vector <answer> variants;

    Dialog(const std::string& msg) : question(msg) {
        std::cout << "ß: " << question << std::endl;
    }
};


struct NarrativeText {
    std::string text;
    int displayTime; 
};

std::vector<Dialog> dialogs;
std::vector<NarrativeText> narratives;
std::vector<Dialog> currentDialogs;

bool initDialogs = false;

void initContentData() {
    
    if (initDialogs)
    {
        return;
    }

    initDialogs = true;

    narratives.push_back({ "You wake up in a dark room...", 2000 });
    narratives.push_back({ "Strange sounds can be heard somewhere in the distance.", 2000 });
    
    Dialog d("You hear a voice: 'Can you hear me?'");

    answer v1;
    v1.text = "Yes, who are you?";
    v1.dialogLink = 1;
    d.variants.push_back(v1);

    answer v2;
    v2.text = "No, it's a hallucination.";
    v2.dialogLink = 2;
    d.variants.push_back(v2);

    dialogs.push_back(d);

    
    narratives.push_back({ "You try to get up, but you feel weak.", 2000 });

    
    Dialog d1("The voice continues: 'WHO'S HERE!'");

    answer v3;
    v3.text = "What's happening?";
    v3.dialogLink = 3;
    d1.variants.push_back(v3);

    dialogs.push_back(d1);

    Dialog d2("The voice continues");

    answer v4;
    v4.text = "What";
    v4.dialogLink = 4;
    d2.variants.push_back(v4);

    dialogs.push_back(d2);

    currentDialogs.push_back(dialogs[0]);
}

void renderContent() {
    
   
    SIZE textSize;

    float textX = 100;
    float textY = 100; 

    

    for (const auto& narrative : narratives) 
    {
        
        drawString(narrative.text.c_str(), textX, textY, 1.f, false);

        textY += 30;
    }

    if (!currentDialogs.empty()) 
    {
        textY += 20;

        const Dialog& d = currentDialogs.back(); 
        drawString(d.question.c_str(), textX, textY, 1.f, false);
        textY += 30;

        for (size_t i = 0; i < d.variants.size(); ++i) 
        {
            std::string variantText = std::to_string(i + 1) + ": " + d.variants[i].text;
            drawString(variantText.c_str(), textX, textY, 1.f, false);
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


