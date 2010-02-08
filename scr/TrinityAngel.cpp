/*
 * Copyright (C) 2010 TrinityAngel rescr1pt@ya.ru   
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version. You can see more in official site GNU.
 */

#include <sstream>
#include <iostream>
#include "TrinityAngel.h"

#pragma warning (disable : 4996)

static wchar_t CONFIG_MAINSETTINGS_FIELD_DELIM = L'=';
static wchar_t CONFIG_WORLDSERVERS_FIELD_DELIM = L';';

TrinityAngel::TrinityAngel()
{
    // defaults
    m_logFile = L"TrinityAngel.log";
    m_wfin.open(L"TrinityAngel.conf");
    m_wfout.open(m_logFile.c_str());
    m_updateInterval = 2500;
    m_maxRunAttempts = 10;
    m_maxCrashesCount = 100;

}
TrinityAngel::~TrinityAngel()
{
    m_wfin.close();
    m_wfout.close();
}

bool TrinityAngel::isNumber(std::wstring number) const
{
    if(number.empty())
        return false;

    for (unsigned short i = 0; i < number.size(); ++i)
    {
        if (!iswdigit(number[i])) {
            return false;
        }
    }

    return true;
}

std::wstring TrinityAngel::GetFieldFromConf(std::wstring field)
{
    if(field.empty())
        return L"";

    std::wstringstream wss; 
    std::wstring line, token, f;

    while(std::getline(m_wfin, line))
    {
        // ignores
        if(field.find(L"#") != std::wstring::npos)
            continue;	

        clearSpaces(line);
        wss.str(line);
		
        // get field 1
        if(getline(wss, token, CONFIG_MAINSETTINGS_FIELD_DELIM)) 
            f = token;
        // get field 2
        if(getline(wss, token) && f == field)
            return token;

        wss.clear();
    }
	
    return L"";
}

void TrinityAngel::clearSpaces(std::wstring& wstr)
{
    std::wstring::size_type n = 0;

    while((n = wstr.find(L' ', n)) != wstr.npos)
        wstr.erase(n, 1);
}

void TrinityAngel::Log(WorldServer* pWorldServer, std::wstring wstr, ...)
{
    if(wstr.size() > 255)
        wstr = L"<Error of the size>";

    // time
    wchar_t date[128], time[128];
    _wstrdate(date);
    _wstrtime(time);

    std::wstringstream fullDateWss;
    fullDateWss << "[" << date << " " << time << "] ";

    // va list 
    wchar_t vaWstr[255];
    va_list ap;
    va_start(ap, wstr);
    _vsnwprintf(vaWstr, 255, wstr.c_str(), ap);
    va_end(ap);

    // format: [fullDate] vaWstr 
    std::wstring result = fullDateWss.str() + vaWstr;

    if(pWorldServer == NULL)
    {
        std::wcout << result << std::endl;
        m_wfout << result << std::endl;
        return;
    }

    // format: [fullDate] Title: vaWstr
    std::wstring objResult = fullDateWss.str() + pWorldServer->GetTitle() +  L": " + vaWstr;

    if(pWorldServer->GetFlag() >= 1)
        std::wcout << objResult << std::endl;
    if(pWorldServer->GetFlag() == 2)
        m_wfout << objResult << std::endl;
}

bool TrinityAngel::LoadMainSettings()
{
    // config open
    if(!m_wfin.is_open())
    {
        Log(NULL, L"config file is not exist!");
        return false; 
    }

    std::wstring wstr;

    // log file
    wstr = GetFieldFromConf(L"LogFile");
    if(wstr != L"")
    {
        m_logFile = wstr;
		m_wfout.open(m_logFile.c_str());
        Log(NULL, L"LogFile = %s", GetLogFileName().c_str());
    }
    else
        Log(NULL, L"LogFile = <error>, set by default <%s>", GetLogFileName().c_str());

    // update time interval
    wstr = GetFieldFromConf(L"UpdateInterval");
    if(isNumber(wstr))
    {
        m_updateInterval = _wtoi(wstr.c_str());
        Log(NULL, L"UpdateInterval = %u", GetUpdateInterval());
    }
    else
        Log(NULL, L"UpdateInterval = <error>, set by default <%u>", GetUpdateInterval());

    // max run attempts row
    wstr = GetFieldFromConf(L"MaxRunAttemptsRow");
    if(isNumber(wstr))
    {
        m_maxRunAttempts = _wtoi(wstr.c_str());
        Log(NULL, L"MaxRunAttempts = %u", GetMaxRunAttempts());
    }
    else
        Log(NULL, L"MaxRunAttempts = <error>, set by default <%u>", GetMaxRunAttempts());

    // max crashes count
    wstr = GetFieldFromConf(L"MaxCrashesCount");
    if(isNumber(wstr)) 
    {
        m_maxCrashesCount = _wtoi(wstr.c_str());
        Log(NULL, L"MaxCrashesCount = %u", GetMaxCrashesCount());
    }
    else
        Log(NULL, L"MaxCrashesCount = <error>, set by default <%u>", GetMaxCrashesCount());

    return true;
}

bool TrinityAngel::LoadWorldServerList()
{
    vWorldServer.clear();

    WorldServer* pWorldServer = NULL;
    std::wstringstream wss; 
    std::wstring line, token;

    while(getline(m_wfin, line))
    {
        // ignore
        if(line.find(L"#") != std::wstring::npos)
            continue;

        wss.str(line);

        bool isParsed = false;

        pWorldServer = new WorldServer();

        // get path
        if(std::getline(wss, token, CONFIG_WORLDSERVERS_FIELD_DELIM))
        {
            clearSpaces(token);
            pWorldServer->SetPath(token);
        }
        // get title
        if(getline(wss, token, CONFIG_WORLDSERVERS_FIELD_DELIM))
        {
            clearSpaces(token);
            pWorldServer->SetTitle(token);
        }
        // get flags and push back to vector
        if(getline(wss, token, CONFIG_WORLDSERVERS_FIELD_DELIM))
        {
            clearSpaces(token);
            if(isNumber(token))
            {
                pWorldServer->SetFlag(_wtoi(token.c_str()));
                vWorldServer.push_back(pWorldServer);
            }
            else
                isParsed = false;
        } 
        else
            isParsed = false;

        if (!isParsed) {
            delete pWorldServer;
            Log(NULL, L"WorldServer = <error>, can not parse the line <%s>", line.c_str());
        }

        wss.clear();
	}

	return true;
}

void TrinityAngel::UpdateWorldsStatus()
{
    for(WorldServerVec::iterator it = vWorldServer.begin(); it != vWorldServer.end(); ++it)
    {
        if(!(*it)->IsAlive() && (*it)->GetStartAttemptCount() < GetMaxRunAttempts()
            && (*it)->GetCrashCount() < GetMaxCrashesCount())
        {	
            (*it)->IncreaseCrashesCount();

            if((*it)->GetStartAttemptCount() == GetMaxRunAttempts()-1)
                Log(*it, L"last attempt to run...");
            else
                Log(*it, L"dies %u time. Restarting...", (*it)->GetCrashCount());

            if((*it)->Enable())
                Log(*it, L"is successfully created.");
            else
                Log(*it, L"error on creating.");		
        }
    }
}

void TrinityAngel::Start()
{
    // title
    std::wcout << " _______   _       _ _                                   _  \n";
    std::wcout << "|__   __| (_)     (_) |           /\\                    | | \n";
    std::wcout << "   | |_ __ _ _ __  _| |_ _   _   /  \\   _ __   __ _  ___| | \n";
    std::wcout << "   | | '__| | '_ \\| | __| | | | / /\\ \\ | '_ \\ / _` |/ _ \\ | \n";
    std::wcout << "   | | |  | | | | | | |_| |_| |/ ____ \\| | | | (_| |  __/ | \n";
    std::wcout << "   |_|_|  |_|_| |_|_|\\__|\\__, / /    \\_\\_| |_|\\__, |\\___|_| \n";
    std::wcout << "                          __/  /               __/ |        \n";
    std::wcout << "                         |____/               |___/        \n";
    Log(NULL, L"rescr1pt@ya.ru>\n");

    if(!LoadMainSettings())   
        return;

    if(!LoadWorldServerList())
        return;

    // first run
    for(WorldServerVec::iterator it = vWorldServer.begin(); it < vWorldServer.end(); ++it)
    {
        Log(*it, L"create from '%s'...", (*it)->GetPath().c_str());

        if((*it)->Enable())
            Log(*it, L"is successfully created."); 
        else
        {
            (*it)->SetStartAttemptCount(GetMaxRunAttempts());
            Log(*it, L"can't create from this path. Please, correct the file path.\n");
        }
    }

    // loop
    while(true)
    {
        if(vWorldServer.empty())
        {			
            Log(NULL, L"not configured the ways of WorldServers.");
            break;
        }
        else
            UpdateWorldsStatus();

        // temporary 
        Sleep(GetUpdateInterval());
    }
}