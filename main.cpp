#include <iostream>
#include <string>
#include <windows.h>
#include <winsvc.h>
#include <memory>

void PrintError(const std::wstring& functionName) 
{
    DWORD errorCode = GetLastError();
    if (errorCode == 0) return;

    LPWSTR messageBuffer = nullptr;
    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, nullptr);

    std::wcerr << L"[ERROR] " << functionName << L": " << messageBuffer << L" (Code: " << errorCode << L")" << std::endl;
    LocalFree(messageBuffer);
}

struct ServiceHandleDeleter 
{
    void operator()(SC_HANDLE handle) const {
        if (handle) {
            CloseServiceHandle(handle);
        }
    }
};

using unique_schandle = std::unique_ptr<void, ServiceHandleDeleter>;

int wmain(int argc, wchar_t* argv[]) 
{
    if (argc != 3) {
        std::wcerr << L"[INFO] Usage: " << argv[0] << L" <start|stop> <ServiceName>" << std::endl;
        return 1;
    }

    const std::wstring action = argv[1];
    const wchar_t* serviceName = argv[2];

    unique_schandle scmHandle(OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT));
    if (!scmHandle) {
        PrintError(L"OpenSCManager");
        return 1;
    }

    unique_schandle serviceHandle(OpenService(scmHandle.get(), serviceName, SERVICE_START | SERVICE_STOP | SERVICE_QUERY_STATUS));
    if (!serviceHandle) {
        PrintError(L"OpenService");
        return 1;
    }

    SERVICE_STATUS_PROCESS ssp;
    DWORD bytesNeeded;
    if (!QueryServiceStatusEx(serviceHandle.get(), SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(ssp), &bytesNeeded)) {
        PrintError(L"QueryServiceStatusEx");
        return 1;
    }

    if (_wcsicmp(action.c_str(), L"start") == 0) {
        if (ssp.dwCurrentState == SERVICE_RUNNING) {
            std::wcout << L"[INFO] Service '" << serviceName << L"' is already running." << std::endl;
            return 0;
        }

        if (!StartService(serviceHandle.get(), 0, nullptr)) {
            PrintError(L"StartService");
            return 1;
        }

        std::wcout << L"[INFO] Starting service '" << serviceName << L"'..." << std::endl;
        
        DWORD startTime = GetTickCount();
        const DWORD timeout = 30000;

        while (ssp.dwCurrentState != SERVICE_RUNNING) {
            Sleep(ssp.dwWaitHint > 0 ? ssp.dwWaitHint : 250);
            if (!QueryServiceStatusEx(serviceHandle.get(), SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(ssp), &bytesNeeded)) {
                PrintError(L"QueryServiceStatusEx");
                return 1;
            }
            if (GetTickCount() - startTime > timeout) {
                std::wcerr << L"[ERROR] Timeout waiting for service to start." << std::endl;
                return 1;
            }
        }
        std::wcout << L"[SUCCESS] Service '" << serviceName << L"' started successfully." << std::endl;

    } else if (_wcsicmp(action.c_str(), L"stop") == 0) {
        if (ssp.dwCurrentState == SERVICE_STOPPED) {
            std::wcout << L"[INFO] Service '" << serviceName << L"' is already stopped." << std::endl;
            return 0;
        }

        if (!ControlService(serviceHandle.get(), SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp)) {
            PrintError(L"ControlService");
            return 1;
        }

        std::wcout << L"[INFO] Stopping service '" << serviceName << L"'..." << std::endl;

        DWORD startTime = GetTickCount();
        const DWORD timeout = 30000;

        while (ssp.dwCurrentState != SERVICE_STOPPED) {
            Sleep(ssp.dwWaitHint > 0 ? ssp.dwWaitHint : 250);
            if (!QueryServiceStatusEx(serviceHandle.get(), SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(ssp), &bytesNeeded)) {
                PrintError(L"QueryServiceStatusEx");
                return 1;
            }
            if (GetTickCount() - startTime > timeout) {
                std::wcerr << L"[ERROR] Timeout waiting for service to stop." << std::endl;
                return 1;
            }
        }
        std::wcout << L"[SUCCESS] Service '" << serviceName << L"' stopped successfully." << std::endl;

    } else {
        std::wcerr << L"[ERROR] Invalid action. Use 'start' or 'stop'." << std::endl;
        return 1;
    }

    return 0;
}
