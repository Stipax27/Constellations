void renderContent(Constellation& c);
void selectVariant(int variantIndex);
void handleInput();
void initContentData();

// Объявите прототипы
void ProcessSound(const char* soundPath);
void StopSound(const char* soundPath);

// Реализация StopSound
void StopSound(const char* soundPath) {
    if (soundPath && strlen(soundPath) > 0) {
        std::string command = "stop " + std::string(soundPath);
        mciSendString(command.c_str(), NULL, 0, NULL);
    }
}

struct answer {
    std::string text;
    int dialogLink;
    std::string soundPath;
    float delayBeforeNext; // Задержка перед следующим диалогом

    answer() : text(""), dialogLink(-1), soundPath(""), delayBeforeNext(0) {}
    answer(const std::string& t, int link, const std::string& sound = "", float delay = 0)
        : text(t), dialogLink(link), soundPath(sound), delayBeforeNext(delay) {
    }
};

struct Dialog {
    std::string question;
    std::vector<answer> variants;
    std::string soundPath;
    float displayDelay; // Задержка перед показом этого диалога

    Dialog(const std::string& msg, float delay = 0, const std::string& sound = "")
        : question(msg), displayDelay(delay), soundPath(sound) {
        std::cout << "Диалог: " << question << " (задержка: " << delay << "мс)" << std::endl;
    }
};

struct NarrativeText {
    std::string text;
    int displayTime;
    std::string soundPath;

    NarrativeText() : text(""), displayTime(0), soundPath("") {}
    NarrativeText(const std::string& t, int time, const std::string& sound = "")
        : text(t), displayTime(time), soundPath(sound) {
    }
};

std::vector<Dialog> dialogs;
std::vector<NarrativeText> narratives;
std::vector<Dialog> currentDialogs;

bool initDialogs = false;
static double soundStartTime = 0.0;
static bool isSoundPlaying = false;
static std::string currentSound = "";
static double dialogShowTime = 0.0; // Время когда должен показаться следующий диалог
static bool waitingForDelay = false; // Флаг ожидания задержки

void initContentData() {
    if (initDialogs) return;
    initDialogs = true;

    // Очищаем векторы для безопасности
    dialogs.clear();
    narratives.clear();
    currentDialogs.clear();

    // Начальные нарративные тексты с озвучкой
    narratives.push_back(NarrativeText("Aspid: \"Who... am I?\n And where?\" ", 0, "..\\dx11minimal\\Resourses\\Sounds\\1.1Hero.wav"));
    narratives.push_back(NarrativeText("He instinctively tries to perform at least some action, \n as if wanting to wake up from a cold oblivion. \n A pleasant pulsation begins to spread through his being.", 0, "..\\dx11minimal\\Resourses\\Sounds\\3.1.wav"));
    narratives.push_back(NarrativeText("A pair of palms slid off his face,\n opening his eyes and allowing him to see the space around him that filled with dim light sources. \n His mind was trying to assess the situation around him,\n creating even more questions that he didn't know the answers to. \n It was if a shadow had enveloped his mind, leaving only fragmented thoughts...", 0, "..\\dx11minimal\\Resourses\\Sounds\\3.2.wav"));
    narratives.push_back(NarrativeText("At this most desperate moment, he suddenly felt someone's powerful presence. \n A bright creature appeared in front of him, starting to resonate with his mind.", 0, "..\\dx11minimal\\Resourses\\Sounds\\3.3.wav"));

    // Основная цепочка диалогов с озвучкой
    Dialog d0("Zenith: \"I can hear the questions in your tiny mind, and I have the answers. \n I am Zenith, the being who rules this world, and You are my creation, which  '\n'I have formed for a specific purpose. '\n' You have appeared in a place beyond your limited understanding. \n The boundless space around you is a canvas on which the world takes shape, and you are just a shell. \n But even such an insignificant creature has a reason to exist...  \nFor example, to fulfill the will of your creator.\"",
        0, "..\\dx11minimal\\Resourses\\Sounds\\2.1.wav");

    d0.variants.push_back(answer("What do you want from me?", 1, "..\\dx11minimal\\Resourses\\Sounds\\1.2Hero.wav", 4000));
    d0.variants.push_back(answer("I don't understand...", 2, "..\\dx11minimal\\Resourses\\Sounds\\1.3Hero.wav", 3000));
    dialogs.push_back(d0);

    // Ответ на первый вариант
    Dialog d1("Zenith: \"My power is immense and can be destructive if \nI interfere with the delicate weaves of this world's destinies. \n But you... You are just a faint echo of my power, \n which is now responsible for the future fate of this universe. \n So open your mind and listen to my words!\"",
        0, "..\\dx11minimal\\Resourses\\Sounds\\2.2.wav");
    d1.variants.push_back(answer("Continue", 3, "..\\dx11minimal\\Resourses\\Sounds\\continue.wav", 0));
    dialogs.push_back(d1);

    // Ответ на второй вариант
    Dialog d2("Zenith: \"You don't need to understand now. Just know that you were created for a great purpose, \n and in time all will become clear to you.\"",
        0, "..\\dx11minimal\\Resourses\\Sounds\\2.3.wav");
    d2.variants.push_back(answer("Continue", 3, "..\\dx11minimal\\Resourses\\Sounds\\continue.wav", 0));
    dialogs.push_back(d2);

    // Продолжение сюжета с озвучкой
    narratives.push_back(NarrativeText("The star cluster is approaching and is beginning to take on clearer outlines, \n resembling a man that dressed in shimmering white robes.  \nHe waves his hand and small lights begin to circle around, \n which line up to form the shapes of constellations.", 0, "..\\dx11minimal\\Resourses\\Sounds\\3.4.wav"));

    // Диалог 3 (index 3 в массиве)
    Dialog d3("Zenith: \"Do you see these bizarre connections in front of you? These are constellations. \n They represent a fundamental aspect of the natural order of things that maintain\n a delicate balance in this world. \n Among them, I will single out the oldest constellation: Aries. \n I learned that Aries was shrouded in a fog of corruption, \n a malevolent force that threatens to disrupt the harmony of existence.\"",
        0, "..\\dx11minimal\\Resourses\\Sounds\\2.4.wav");
    d3.variants.push_back(answer("What should I do?", 4, "..\\dx11minimal\\Resourses\\Sounds\\1.4Hero.wav", 2500));
    dialogs.push_back(d3);

    // Диалог 4 (index 4 в массиве)
    Dialog d4("Zenith: \"The domain of Aries is not far from us, and you must find and save it from the corruption that has consumed its being. \n He occupies a special place in the cosmic order \n and it is his absence that causes ripples in the fabric of reality.\"",
        0, "..\\dx11minimal\\Resourses\\Sounds\\2.5.wav");
    d4.variants.push_back(answer("But I'm powerless... How could I save Aries?", 5, "..\\dx11minimal\\Resourses\\Sounds\\1.5Hero.wav", 4000));
    dialogs.push_back(d4);

    // Диалог 5 (index 5 в массиве)
    Dialog d5("Zenith: \"I see the fire in your eyes, my creature. \n But you doubt your abilities. Let me explain. \n You are not just a shell that is devoid of essence. \n I have given you the ability to take shape and change to adapt to any situation. \n The shell in which you see yourself is just a basic form, \n something like a template on the basis of which you can build and shape anything out of yourself.\"",
        0, "..\\dx11minimal\\Resourses\\Sounds\\2.6.wav");
    d5.variants.push_back(answer("I understand... I'll try", 6, "..\\dx11minimal\\Resourses\\Sounds\\1.6Hero.wav", 3000));
    dialogs.push_back(d5);

    // Диалог 6 (index 6 в массиве)
    Dialog d6("Zenith: \"Your true strength lies not in your physical form, \n but in your ability to adapt, learn, and become what the situation requires of you. \n Your other ability is to purify constellations from corruption. \n You are my vessel, a flexible instrument with the ability to absorb filth and purify cosmic energy. \n By purifying Aries, you will draw out all the corruption from him \n and seal the negative energy inside yourself.\"",
        0, "..\\dx11minimal\\Resourses\\Sounds\\2.7.wav");
    d6.variants.push_back(answer("I accept this burden", 7, "..\\dx11minimal\\Resourses\\Sounds\\1.7Hero.wav", 4000));
    dialogs.push_back(d6);

    // Диалог 7 (index 7 в массиве)
    Dialog d7("Zenith: \"Beware, my creation, because the corruption you absorb will affect you as well.  \nYou must be prepared to face the consequences of your actions \n and remain strong in the face of the darkness you seek to purify.  \nIf you're ready to take on this assignment, I'll show you the way.\"",
        0, "..\\dx11minimal\\Resourses\\Sounds\\2.8.wav");
    d7.variants.push_back(answer("I'm ready", -1, "..\\dx11minimal\\Resourses\\Sounds\\1.8Hero.wav", 3000));
    dialogs.push_back(d7);

    // Проверяем что диалоги созданы корректно
    if (!dialogs.empty()) {
        currentDialogs.push_back(dialogs[0]);
    }

    std::cout << "Загружено диалогов: " << dialogs.size() << std::endl;
}

void selectVariant(int variantIndex) {
    if (currentDialogs.empty()) {
        std::cout << "Ошибка: currentDialogs пуст!" << std::endl;
        return;
    }

    Dialog& currentDialog = currentDialogs.back();
    if (variantIndex < 0 || variantIndex >= (int)currentDialog.variants.size()) {
        std::cout << "Ошибка: неверный variantIndex: " << variantIndex << std::endl;
        return;
    }

    const answer& selectedAnswer = currentDialog.variants[variantIndex];

    // Воспроизводим звук ответа ГГ
    if (!selectedAnswer.soundPath.empty()) {
        if (!currentSound.empty()) {
            StopSound(currentSound.c_str());
        }
        ProcessSound(selectedAnswer.soundPath.c_str());
        currentSound = selectedAnswer.soundPath;
        soundStartTime = currentTime;
        isSoundPlaying = true;
        std::cout << "Воспроизводим звук ответа ГГ: " << selectedAnswer.soundPath << std::endl;
    }

    int nextDialogIndex = selectedAnswer.dialogLink;
    std::cout << "Выбран вариант: " << selectedAnswer.text << " -> переход к диалогу " << nextDialogIndex << std::endl;

    if (nextDialogIndex == -1) {
        // Конец диалога
        currentDialogs.clear();
        isSoundPlaying = false;
        waitingForDelay = false;
        std::cout << "Диалог завершен" << std::endl;
    }
    else if (nextDialogIndex >= 0 && nextDialogIndex < (int)dialogs.size()) {
        // Устанавливаем задержку перед следующим диалогом
        if (selectedAnswer.delayBeforeNext > 0) {
            waitingForDelay = true;
            dialogShowTime = currentTime + selectedAnswer.delayBeforeNext;
            std::cout << "Установлена задержка: " << selectedAnswer.delayBeforeNext << "мс" << std::endl;
        }
        else {
            waitingForDelay = false;
        }

        // Добавляем следующий диалог
        currentDialogs.push_back(dialogs[nextDialogIndex]);
        std::cout << "Добавлен диалог " << nextDialogIndex << " в currentDialogs" << std::endl;
    }
    else {
        std::cout << "Ошибка: неверный nextDialogIndex: " << nextDialogIndex << " (всего диалогов: " << dialogs.size() << ")" << std::endl;
        return;
    }

    renderContent(*starSet[player_sign]);
}

void renderContent(Constellation& c) {
    float textX = window.width / 2;
    float textY = (200. / 1440) * window.height;
    static int currentNarrative = 0;
    static double lastClickTime = 0.0;
    static bool narrativeSoundPlayed = false;
    static bool dialogSoundPlayed = false;
    
    if (currentNarrative < (int)narratives.size()) {
        // Показываем нарративный текст
        const NarrativeText& narrative = narratives[currentNarrative];
        drawString(narrative.text.c_str(), textX, textY, 1.f, true);

        if (!narrativeSoundPlayed && !narrative.soundPath.empty()) {
            if (!currentSound.empty()) {
                StopSound(currentSound.c_str());
            }
            ProcessSound(narrative.soundPath.c_str());
            currentSound = narrative.soundPath;
            soundStartTime = currentTime;
            isSoundPlaying = true;
            narrativeSoundPlayed = true;
            std::cout << "Воспроизводим нарративный звук: " << narrative.soundPath << std::endl;
        }

        bool canClick = (currentTime - lastClickTime) >= 1000;
        bool canProceed = canClick && (!isSoundPlaying || (currentTime - soundStartTime) >= 1000);

        if (drawClickableText("Continue", true, RGB(0, 191, 255), textX, (1050. / 1440) * window.height) && canProceed) {
            lastClickTime = currentTime;
            narrativeSoundPlayed = false;
            isSoundPlaying = false;
            currentNarrative++;
            if (currentNarrative >= (int)narratives.size()) {
                if (!dialogs.empty()) {
                    currentDialogs.push_back(dialogs[0]);
                    dialogSoundPlayed = false;
                    waitingForDelay = false;
                    std::cout << "Начинаем диалоги с диалога 0" << std::endl;
                }
            }
            return;
        }
        c.StartShaking(0.5f,1000.f);
    }
    else {
        // Показываем диалоги
        if (!currentDialogs.empty()) {
            Dialog& d = currentDialogs.back();
            std::cout << "Показываем диалог: " << d.question.substr(0, 50) << "..." << std::endl;

            // Проверяем задержку перед показом диалога
            if (waitingForDelay) {
                if (currentTime >= dialogShowTime) {
                    waitingForDelay = false;
                    dialogSoundPlayed = false; // Сбрасываем флаг звука
                    std::cout << "Задержка завершена, показываем диалог" << std::endl;
                }
                else {
                    // Показываем сообщение о ожидании
                    float waitTime = (float)(dialogShowTime - currentTime);
                    std::string waitText = "..." + std::to_string((int)waitTime) + "ms";
                    //drawString(waitText.c_str(), textX, textY, 1.f, true);
                    return;
                }
            }

            // Показываем вопрос диалога
            textY += (50. / 1440) * window.height;
            drawString(d.question.c_str(), textX, textY, 1.f, true);

            // Воспроизводим звук диалога Зенита
            if (!dialogSoundPlayed && !d.soundPath.empty()) {
                if (!currentSound.empty()) {
                    StopSound(currentSound.c_str());
                }
                ProcessSound(d.soundPath.c_str());
                currentSound = d.soundPath;
                soundStartTime = currentTime;
                isSoundPlaying = true;
                dialogSoundPlayed = true;
                std::cout << "Воспроизводим звук диалога Зенита: " << d.soundPath << std::endl;
            }

            textY += (500. / 1440) * window.height;

            // Показываем варианты ответов
            for (size_t i = 0; i < d.variants.size(); ++i) {
                bool canClick = (currentTime - lastClickTime) >= 1000;
                bool canSelect = canClick && (!isSoundPlaying || (currentTime - soundStartTime) >= 1000);

                if (drawClickableText(d.variants[i].text, true, RGB(0, 191, 255), textX, textY) && canSelect) {
                    lastClickTime = currentTime;
                    dialogSoundPlayed = false;
                    selectVariant((int)i);
                }
                textY += (50. / 1440) * window.height;
            }
        }
        else {
            std::cout << "currentDialogs пуст!" << std::endl;
        }
        c.StartShaking(0.5f, 1000.f);
    }
}

void handleInput() {
    static double lastClickTime = 0.0;

    bool canClick = (currentTime - lastClickTime) >= 1.f;
    drawString("If you don't want to read \n Start Game Press P", window.width / 2, (1200. / 1440) * window.height, 1.f, true);
    if (GetAsyncKeyState('P') & 0x8000) {
        lastClickTime = currentTime;
        if (!currentSound.empty()) {
            StopSound(currentSound.c_str());
        }
        StartTransition(gameState_::selectEnemy, 1500.0f);
        mciSendString(TEXT("stop ..\\dx11minimal\\Resourses\\Sounds\\GG_C.mp3"), NULL, 0, NULL);
        mciSendString(TEXT("play ..\\dx11minimal\\Resourses\\Sounds\\FREEFLY.mp3"), NULL, 0, NULL);
        startTime = currentTime;
        isSoundPlaying = false;
        waitingForDelay = false;
    }
}