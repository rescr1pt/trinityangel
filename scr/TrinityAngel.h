/*
 * Copyright (C) 2010 TrinityAngel rescr1pt@ya.ru   
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version. You can see more in official site GNU.
 */

#ifndef TRINITYANGEL_H
#define TRINITYANGEL_H

#include <fstream>
#include <vector>
#include "WorldServer.h"

typedef std::vector <WorldServer*> WorldServerVec;

/* this class represents a set of different functions for starting and 
controlling the basic procedures of the program. 
Used in ::main()*/
class TrinityAngel
{
    public:
        TrinityAngel();
        ~TrinityAngel();
        void Log(WorldServer* pWorldServer, std::wstring wstr, ...);
        void Start();

    private:
        void clearSpaces(std::wstring& wstr);
        bool isNumber(std::wstring number) const;

        bool LoadMainSettings();
        bool LoadWorldServerList();
        void UpdateWorldsStatus();

        std::wstring GetFieldFromConf(std::wstring field);
        std::wstring GetLogFileName() const { return m_logFile; }
        unsigned int GetUpdateInterval() const { return m_updateInterval; }
        unsigned int GetMaxRunAttempts() const { return m_maxRunAttempts; }
        unsigned int GetMaxCrashesCount() const { return m_maxCrashesCount; }  

        std::wstring m_logFile;
        unsigned int m_updateInterval;
        unsigned int m_maxRunAttempts;
        unsigned int m_maxCrashesCount;

        std::wifstream m_wfin;
        std::wofstream m_wfout;
        WorldServerVec vWorldServer;
};

#endif
