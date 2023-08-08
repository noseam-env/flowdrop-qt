/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "console_fix.h"
#include <QtCore/qsystemdetection.h>

#if defined(Q_OS_WINDOWS)

#include <windows.h>
#include <tlhelp32.h>

void terminateProcess(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess != NULL) {
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    }
}

void terminalAllSubProcesses(DWORD parentProcessId) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &processEntry)) {
        do {
            if (processEntry.th32ParentProcessID == parentProcessId) {
                terminateProcess(processEntry.th32ProcessID);
            }
        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
}

void consoleFix() {
    DWORD currentProcessId = GetCurrentProcessId();
    terminalAllSubProcesses(currentProcessId);
}

#else

#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void terminateProcess(pid_t processId) {
    kill(processId, SIGTERM);
}

void terminalAllSubProcesses(pid_t parentProcessId) {
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        terminateProcess(pid);
    }
}

void consoleFix() {
    pid_t currentProcessId = getpid();
    terminalAllSubProcesses(currentProcessId);
}

#endif
