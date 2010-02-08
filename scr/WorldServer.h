/*
 * Copyright (C) 2010 TrinityAngel rescr1pt@ya.ru   
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version. You can see more in official site GNU.
 */

#ifndef WORLDSERVER_H
#define WORLDSERVER_H

#include <string>
#include <windows.h>

class WorldServer
{
    public:
        WorldServer();

        bool Enable();
        bool Disable();
        bool IsAlive() const;

        unsigned int GetCrashCount() const { return m_crashCount; }
        void IncreaseCrashesCount(int count = 1) { m_crashCount += count; }
        unsigned int GetStartAttemptCount() const { return m_startAttemptCount; }
        void SetStartAttemptCount(int attemptCount) { m_startAttemptCount = attemptCount; }

        void SetPath(const std::wstring& path) { m_path = path; }
        const std::wstring& GetPath() const { return m_path; }
        void SetTitle(const std::wstring& title) { m_title = title; }
        const std::wstring& GetTitle() const { return m_title; }
        void SetFlag(int flag) { m_flag = flag; }
        int GetFlag() const { return m_flag; }
        void SetDumpPath(std::wstring path) { m_dumpPath = path; }
        const std::wstring& GetDumpPath() const { return m_dumpPath; }

    private:
        int m_crashCount;
        int m_startAttemptCount;

        PROCESS_INFORMATION m_processInf;
        STARTUPINFO m_startupInf;

        std::wstring m_path;
        std::wstring m_title;
        int m_flag;
        std::wstring m_dumpPath;
};

#endif