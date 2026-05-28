// GameState.cpp
#include "GameState.h"

// Деструктор уже в хедере, так как default

void GameState::Exit()
{
    if (m_MenuRoot)
    {
        m_MenuRoot->SetActive(false);
        m_MenuRoot = nullptr;
    }
    m_IsActive = false;
}