void renderContent();
void selectVariant(int variantIndex);
void handleInput();
void initContentData();

struct answer {
    std::string text;
    int dialogLink;
};

struct Dialog {
    std::string question;
    std::vector<answer> variants;

    Dialog(const std::string& msg) : question(msg) {
        std::cout << "Диалог: " << question << std::endl;
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
    if (initDialogs) return;
    initDialogs = true;

    // Начальные нарративные тексты
    narratives.push_back({ "MC: \"Who... am I?\" "});
    narratives.push_back({ "MC: \"And where?\" "});
    narratives.push_back({ "He instinctively tries to perform at least some action, \n as if wanting to wake up from a cold oblivion. \n A pleasant pulsation begins to spread through his being." });
    narratives.push_back({ "A pair of palms slid off his face,\n opening his eyes and allowing him to see the space around him that filled with dim light sources. \n His mind was trying to assess the situation around him,\n creating even more questions that he didn't know the answers to. \n It was if a shadow had enveloped his mind, leaving only fragmented thoughts..."});
    narratives.push_back({ "At this most desperate moment, he suddenly felt someone's powerful presence. \n A bright creature appeared in front of him, starting to resonate with his mind." });

    // Основная цепочка диалогов
    Dialog d0("Zenith: \"I can hear the questions in your tiny mind, and I have the answers. \n I am Zenith, the being who rules this world, and You are my creation, which  '\n'I have formed for a specific purpose. '\n' You have appeared in a place beyond your limited understanding. \n The boundless space around you is a canvas on which the world takes shape, and you are just a shell. \n But even such an insignificant creature has a reason to exist...  \nFor example, to fulfill the will of your creator.\"");

    answer v0_1;
    v0_1.text = "What do you want from me?";
    v0_1.dialogLink = 1;
    d0.variants.push_back(v0_1);

    answer v0_2;
    v0_2.text = "I don't understand...";
    v0_2.dialogLink = 2;
    d0.variants.push_back(v0_2);

    dialogs.push_back(d0);

    // Ответ на первый вариант
    Dialog d1("Zenith: \"My power is immense and can be destructive if \nI interfere with the delicate weaves of this world's destinies. \n But you... You are just a faint echo of my power, \n which is now responsible for the future fate of this universe. \n So open your mind and listen to my words!\"");

    answer v1_1;
    v1_1.text = "Continue";
    v1_1.dialogLink = 3;
    d1.variants.push_back(v1_1);

    dialogs.push_back(d1);

    // Ответ на второй вариант
    Dialog d2("Zenith: \"You don't need to understand now. Just know that you were created for a great purpose, \n and in time all will become clear to you.\"");

    answer v2_1;
    v2_1.text = "Continue";
    v2_1.dialogLink = 3;
    d2.variants.push_back(v2_1);

    dialogs.push_back(d2);

    // Продолжение сюжета
    narratives.push_back({ "The star cluster is approaching and is beginning to take on clearer outlines, \n resembling a man that dressed in shimmering white robes.  \nHe waves his hand and small lights begin to circle around, \n which line up to form the shapes of constellations." });

    Dialog d3("Zenith: \"Do you see these bizarre connections in front of you? These are constellations. \n They represent a fundamental aspect of the natural order of things that maintain\n a delicate balance in this world. \n Among them, I will single out the oldest constellation: Aries. \n I learned that Aries was shrouded in a fog of corruption, \n a malevolent force that threatens to disrupt the harmony of existence.\"");

    answer v3_1;
    v3_1.text = "What should I do?";
    v3_1.dialogLink = 4;
    d3.variants.push_back(v3_1);

    dialogs.push_back(d3);

    Dialog d4("Zenith: \"The domain of Aries is not far from us, and you must find and save it from the corruption that has consumed its being. \n He occupies a special place in the cosmic order \n and it is his absence that causes ripples in the fabric of reality.\"");

    answer v4_1;
    v4_1.text = "But I'm powerless... How could I save Aries?";
    v4_1.dialogLink = 5;
    d4.variants.push_back(v4_1);

    dialogs.push_back(d4);

    Dialog d5("Zenith: \"I see the fire in your eyes, my creature. \n But you doubt your abilities. Let me explain. \n You are not just a shell that is devoid of essence. \n I have given you the ability to take shape and change to adapt to any situation. \n The shell in which you see yourself is just a basic form, \n something like a template on the basis of which you can build and shape anything out of yourself.\"");

    answer v5_1;
    v5_1.text = "I understand... I'll try";
    v5_1.dialogLink = 6;
    d5.variants.push_back(v5_1);

    dialogs.push_back(d5);

    Dialog d6("Zenith: \"Your true strength lies not in your physical form, \n but in your ability to adapt, learn, and become what the situation requires of you. \n Your other ability is to purify constellations from corruption. \n You are my vessel, a flexible instrument with the ability to absorb filth and purify cosmic energy. \n By purifying Aries, you will draw out all the corruption from him \n and seal the negative energy inside yourself.\"");

    answer v6_1;
    v6_1.text = "I accept this burden";
    v6_1.dialogLink = 7;
    d6.variants.push_back(v6_1);

    dialogs.push_back(d6);

    Dialog d7("Zenith: \"Beware, my creation, because the corruption you absorb will affect you as well.  \nYou must be prepared to face the consequences of your actions \n and remain strong in the face of the darkness you seek to purify.  \nIf you're ready to take on this assignment, I'll show you the way.\"");

    answer v7_1;
    v7_1.text = "I'm ready";
    v7_1.dialogLink = -1; // Конец диалога
    d7.variants.push_back(v7_1);

    dialogs.push_back(d7);

    // Завершающие нарративы
    /*narratives.push_back({ "START TUTORIAL" });
    narratives.push_back({ "Zenith: \"I'll show you how you should unleash your potential in battle. Let's get started.\""});
    narratives.push_back({ "AFTER TUTORIAL"});
    narratives.push_back({ "MC: \"Hmm, it turned out to be easier than I thought. This power is overwhelming...\"" });
    narratives.push_back({ "Our hero's thoughts are interrupted by Zenith's booming voice, '\n' directing his attention to the starry expanse. '\n' The faint light becomes his guide, paving the way further." });
    narratives.push_back({ "Zenith: \"Follow this path and it will lead you to the extinguished stars of Aries. '\n' Remember the abilities I have given you. '\n' And now... It's time for you to prove yourself.\"" });
    narratives.push_back({ "Zenith's silhouette disappears into space, leaving him no choice but to follow his instructions. '\n' The hero decides to follow the landmark, '\n' rapidly approaching the constellation of Aries."});
    narratives.push_back({ "Our hero moves through the Milky Way to Aries and quickly finds himself in his territory. '\n' The space around the hero seemed to have become colder, '\n' as if the atmosphere was becoming saturated with negativity and despair. '\n' Traces of heavy hoofprints were visible in the trampled grass, '\n' leaving behind a desolate trail of destruction." });*/

    currentDialogs.push_back(dialogs[0]);
}

void selectVariant(int variantIndex) {
    if (currentDialogs.empty()) return;

    const Dialog& currentDialog = currentDialogs.back();
    if (variantIndex < 0 || variantIndex >= currentDialog.variants.size()) return;

    int nextDialogIndex = currentDialog.variants[variantIndex].dialogLink;

    if (nextDialogIndex == -1) {
        // Конец диалога
        currentDialogs.clear();
    }
    else if (nextDialogIndex >= 0 && nextDialogIndex < dialogs.size()) {
        currentDialogs.push_back(dialogs[nextDialogIndex]);
    }

    renderContent();
}

static double lastClickTime = 0.0;

void renderContent() {
    float textX = window.width / 2;
    float textY = (500. / 1440) * window.height;
    static int currentNarrative = 0;
    static double lastClickTime = 0.0; 

    if (currentNarrative < narratives.size()) {
        drawString(narratives[currentNarrative].text.c_str(), textX, textY, 1.f, true);

        
        bool canClick = (currentTime - lastClickTime) >= 1000;

        if (drawClickableText("Continue", true, RGB(0, 191, 255), textX, (1050. / 1440) * window.height) && canClick & 0x0001) {
            lastClickTime = currentTime;
            currentNarrative++;
            if (currentNarrative >= narratives.size()) {
                currentDialogs.push_back(dialogs[0]);
            }
            return;
        }
    }
    else {
        if (!currentDialogs.empty()) {
            textY += (50. / 1440) * window.height;
            const Dialog& d = currentDialogs.back();
            drawString(d.question.c_str(), textX, textY, 1.f, true);
            textY += (500. / 1440) * window.height;

            for (size_t i = 0; i < d.variants.size(); ++i) {
                
                bool canClick = (currentTime - lastClickTime) >= 1000;

                if (drawClickableText(d.variants[i].text, true, RGB(0, 191, 255), textX, textY) && canClick & 0x0001) {
                    lastClickTime = currentTime; 
                    selectVariant(i);
                }
                textY += (50. / 1440) * window.height;
            }
        }
    }
}

void handleInput() {
    static double lastClickTime = 0.0; 

    bool canClick = (currentTime - lastClickTime) >= 1.f;

    if (drawClickableText("If you don't want to read \n Start Game", true, RGB(0, 191, 255), window.width / 2, (1200. / 1440) * window.height) && canClick) {
        lastClickTime = currentTime; 
        gameState = gameState_::selectEnemy;
        startTime = currentTime;
    }
}