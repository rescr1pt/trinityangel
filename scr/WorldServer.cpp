/*
 * Copyright (C) 2010 TrinityAngel rescr1pt@ya.ru   
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version. You can see more in official site GNU.
 */

#include "WorldServer.h"

WorldServer::WorldServer()
{
    m_crashCount = 0;
    m_startAttemptCount = 0;
}

bool WorldServer::Enable()
{
    if(!IsAlive())
    {
        ZeroMemory(&m_startupInf, sizeof(STARTUPINFO));

        if(CreateProcess(m_path.c_str(), NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &m_startupInf, &m_processInf))
        {
           m_startAttemptCount = 0;
           return true;
        }
        else
            ++m_startAttemptCount;
    }
    return false;
}

bool WorldServer::Disable() 
{
    if(IsAlive())
    {
        TerminateProcess(m_processInf.hProcess, NO_ERROR);
        return true;
    }

    return false;
}

bool WorldServer::IsAlive() const
{
    DWORD exitCode;

    if(GetExitCodeProcess(m_processInf.hProcess, &exitCode) && exitCode == STILL_ACTIVE)
        return true;

    return false;
}