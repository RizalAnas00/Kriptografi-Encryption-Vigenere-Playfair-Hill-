#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "VigenereCipher.h"
#include "PlayfairCipher.h"
#include "HillCipher.h"

// Utility functions for string conversion
std::string wstring_to_string(const std::wstring& wstr) {
    return std::string(wstr.begin(), wstr.end());
}

std::wstring string_to_wstring(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

// IDs for the controls
#define IDC_INPUT_TEXT_ENCRYPT 101
#define IDC_INPUT_KEY_ENCRYPT 102
#define IDC_BUTTON_UPLOAD_ENCRYPT 103
#define IDC_BUTTON_SUBMIT_ENCRYPT 104
#define IDC_OUTPUT_BOX_ENCRYPT 105

#define IDC_INPUT_TEXT_DECRYPT 106
#define IDC_BUTTON_UPLOAD_DECRYPT 107
#define IDC_BUTTON_SUBMIT_DECRYPT 108
#define IDC_OUTPUT_BOX_DECRYPT 109

#define IDC_COMBO_ENCRYPT_METHOD 110 // Combo box for encryption method
#define IDC_COMBO_DECRYPT_METHOD 111 // Combo box for decryption method

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Function to read file content
std::wstring ReadFileContent(const wchar_t* filePath) {
    std::wifstream fileStream(filePath);
    if (!fileStream) return L"Error opening file";

    std::wstringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}

// Entry point of the application
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wcex;
    HWND hWnd;
    MSG msg;

    // Window class structure
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"MainWindowClass";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    // Register the window class
    if (!RegisterClassEx(&wcex)) return 1;

    // Create the main window
    hWnd = CreateWindow(L"MainWindowClass", L"Encryption and Decryption App",
        WS_OVERLAPPEDWINDOW | WS_VSCROLL, CW_USEDEFAULT, 0, 600, 700, NULL, NULL, hInstance, NULL);
    if (!hWnd) return 1;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hInputEncrypt, hKeyEncrypt, hButtonUploadEncrypt, hButtonSubmitEncrypt, hOutputEncrypt;
    static HWND hInputDecrypt, hKeyDecrypt, hButtonUploadDecrypt, hButtonSubmitDecrypt, hOutputDecrypt;
    static wchar_t filePathEncrypt[MAX_PATH] = L"No file selected";
    static wchar_t filePathDecrypt[MAX_PATH] = L"No file selected";
    wchar_t inputTextEncrypt[1024];
    wchar_t inputTextDecrypt[1024];

    switch (message) {
    case WM_CREATE: {
        // Encryption section
        CreateWindow(L"STATIC", L"Encryption", WS_CHILD | WS_VISIBLE | SS_CENTER, 20, 10, 250, 25, hWnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Message:", WS_CHILD | WS_VISIBLE, 20, 50, 80, 25, hWnd, NULL, NULL, NULL);
        hInputEncrypt = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_HSCROLL | WS_VSCROLL,
            100, 50, 400, 50, hWnd, (HMENU)IDC_INPUT_TEXT_ENCRYPT, NULL, NULL);

        CreateWindow(L"STATIC", L"Key:", WS_CHILD | WS_VISIBLE, 20, 110, 80, 25, hWnd, NULL, NULL, NULL);
        hKeyEncrypt = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 110, 400, 25, hWnd, (HMENU)IDC_INPUT_KEY_ENCRYPT, NULL, NULL);

        hButtonUploadEncrypt = CreateWindow(L"BUTTON", L"Upload File", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            100, 150, 100, 30, hWnd, (HMENU)IDC_BUTTON_UPLOAD_ENCRYPT, NULL, NULL);

        hButtonSubmitEncrypt = CreateWindow(L"BUTTON", L"Submit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            210, 150, 100, 30, hWnd, (HMENU)IDC_BUTTON_SUBMIT_ENCRYPT, NULL, NULL);

        hOutputEncrypt = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
            100, 190, 400, 100, hWnd, (HMENU)IDC_OUTPUT_BOX_ENCRYPT, NULL, NULL);

        // Combo box for selecting encryption method
        CreateWindow(L"STATIC", L"Select Encryption Method:", WS_CHILD | WS_VISIBLE, 20, 300, 200, 25, hWnd, NULL, NULL, NULL);
        HWND hComboBoxEncrypt = CreateWindow(L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
            20, 330, 200, 100, hWnd, (HMENU)IDC_COMBO_ENCRYPT_METHOD, NULL, NULL);

        // Add items to the combo box
        SendMessage(hComboBoxEncrypt, CB_ADDSTRING, 0, (LPARAM)L"Vigenere Cipher");
        SendMessage(hComboBoxEncrypt, CB_ADDSTRING, 0, (LPARAM)L"Playfair Cipher");
        SendMessage(hComboBoxEncrypt, CB_ADDSTRING, 0, (LPARAM)L"Hill Cipher");
        SendMessage(hComboBoxEncrypt, CB_SETCURSEL, 0, 0); // Default to Vigenere

        // Decryption section
        CreateWindow(L"STATIC", L"Decryption", WS_CHILD | WS_VISIBLE | SS_CENTER, 20, 370, 250, 25, hWnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Message:", WS_CHILD | WS_VISIBLE, 20, 410, 80, 25, hWnd, NULL, NULL, NULL);
        hInputDecrypt = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_HSCROLL | WS_VSCROLL,
            100, 410, 400, 50, hWnd, (HMENU)IDC_INPUT_TEXT_DECRYPT, NULL, NULL);

        CreateWindow(L"STATIC", L"Key:", WS_CHILD | WS_VISIBLE, 20, 470, 80, 25, hWnd, NULL, NULL, NULL);
        hKeyDecrypt = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 470, 400, 25, hWnd, (HMENU)IDC_INPUT_KEY_ENCRYPT, NULL, NULL);

        hButtonUploadDecrypt = CreateWindow(L"BUTTON", L"Upload File", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            100, 510, 100, 30, hWnd, (HMENU)IDC_BUTTON_UPLOAD_DECRYPT, NULL, NULL);

        hButtonSubmitDecrypt = CreateWindow(L"BUTTON", L"Submit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            210, 510, 100, 30, hWnd, (HMENU)IDC_BUTTON_SUBMIT_DECRYPT, NULL, NULL);

        hOutputDecrypt = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
            100, 550, 400, 100, hWnd, (HMENU)IDC_OUTPUT_BOX_DECRYPT, NULL, NULL);

        // Combo box for selecting decryption method
        CreateWindow(L"STATIC", L"Select Decryption Method:", WS_CHILD | WS_VISIBLE, 20, 650, 200, 25, hWnd, NULL, NULL, NULL);
        HWND hComboBoxDecrypt = CreateWindow(L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
            20, 680, 200, 100, hWnd, (HMENU)IDC_COMBO_DECRYPT_METHOD, NULL, NULL);

        // Add items to the combo box
        SendMessage(hComboBoxDecrypt, CB_ADDSTRING, 0, (LPARAM)L"Vigenere Cipher");
        SendMessage(hComboBoxDecrypt, CB_ADDSTRING, 0, (LPARAM)L"Playfair Cipher");
        SendMessage(hComboBoxDecrypt, CB_ADDSTRING, 0, (LPARAM)L"Hill Cipher");
        SendMessage(hComboBoxDecrypt, CB_SETCURSEL, 0, 0); // Default to Vigenere
    }
    break;

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case IDC_BUTTON_UPLOAD_ENCRYPT: {
            OPENFILENAME ofn;
            wchar_t szFile[260];
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFile = szFile;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn)) {
                wcscpy_s(filePathEncrypt, ofn.lpstrFile);
                std::wstring content = ReadFileContent(filePathEncrypt);
                SetWindowText(hInputEncrypt, content.c_str());
            }
        }
        break;

        case IDC_BUTTON_SUBMIT_ENCRYPT: {
            wchar_t keyText[1024];
            GetWindowText(hInputEncrypt, inputTextEncrypt, sizeof(inputTextEncrypt) / sizeof(inputTextEncrypt[0]));
            GetWindowText(hKeyEncrypt, keyText, sizeof(keyText) / sizeof(keyText[0]));

            // Debug output
            OutputDebugString(inputTextEncrypt); // Menampilkan input
            OutputDebugString(keyText); // Menampilkan kunci

            // Get the selected method from the combo box
            int selectedIndex = SendMessage(GetDlgItem(hWnd, IDC_COMBO_ENCRYPT_METHOD), CB_GETCURSEL, 0, 0);
            int method = selectedIndex; // 0 for Vigenere, 1 for Playfair, 2 for Hill

            // Error handling for key length
            size_t keyLength = wcslen(keyText);
            if ((method != 2 && keyLength < 12) || (method == 2 && keyLength != 16)) {
                // Jika panjang key kurang dari 12 karakter untuk Vigenere/Playfair atau 16 karakter untuk Hill
                MessageBox(hWnd, L"Key must be at least 12 characters for Vigenere/Playfair or 16 characters for Hill cipher.", L"Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            std::string encryptedText;
            switch (method) {
            case 0: // Vigenere Cipher
                encryptedText = VigenereCipher::Encrypt(wstring_to_string(inputTextEncrypt), wstring_to_string(keyText));
                break;
            case 1: // Playfair Cipher
                encryptedText = PlayfairCipher::Encrypt(wstring_to_string(inputTextEncrypt), wstring_to_string(keyText));
                break;
            case 2: // Hill Cipher
                encryptedText = HillCipher::Encrypt(wstring_to_string(inputTextEncrypt), wstring_to_string(keyText));
                break;
            }

            SetWindowText(hOutputEncrypt, string_to_wstring(encryptedText).c_str());
        }
                                      break;


        case IDC_BUTTON_UPLOAD_DECRYPT: {
            OPENFILENAME ofn;
            wchar_t szFile[260];
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFile = szFile;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn)) {
                wcscpy_s(filePathDecrypt, ofn.lpstrFile);
                std::wstring content = ReadFileContent(filePathDecrypt);
                SetWindowText(hInputDecrypt, content.c_str());
            }
        }
        break;

        case IDC_BUTTON_SUBMIT_DECRYPT: {
            wchar_t keyTextDecrypt[1024];
            GetWindowText(hInputDecrypt, inputTextDecrypt, sizeof(inputTextDecrypt) / sizeof(inputTextDecrypt[0]));
            GetWindowText(hKeyDecrypt, keyTextDecrypt, sizeof(keyTextDecrypt) / sizeof(keyTextDecrypt[0]));

            // Debug output untuk memastikan input
            OutputDebugString(inputTextDecrypt); // Tampilkan input teks dekripsi
            OutputDebugString(keyTextDecrypt); // Tampilkan kunci dekripsi

            // Ambil metode yang dipilih dari combo box
            int selectedDecryptIndex = SendMessage(GetDlgItem(hWnd, IDC_COMBO_DECRYPT_METHOD), CB_GETCURSEL, 0, 0);
            int decryptMethod = selectedDecryptIndex; // 0 untuk Vigenere, 1 untuk Playfair, 2 untuk Hill

            std::string decryptedText;
            switch (decryptMethod) {
            case 0: // Vigenere Cipher
                decryptedText = VigenereCipher::Decrypt(wstring_to_string(inputTextDecrypt), wstring_to_string(keyTextDecrypt));
                break;
            case 1: // Playfair Cipher
                decryptedText = PlayfairCipher::Decrypt(wstring_to_string(inputTextDecrypt), wstring_to_string(keyTextDecrypt));
                break;
            case 2: // Hill Cipher
                // Pastikan kunci adalah string dan bukan matriks
                decryptedText = HillCipher::Decrypt(wstring_to_string(inputTextDecrypt), wstring_to_string(keyTextDecrypt));
                break;
            }

            // Tampilkan teks yang sudah didekripsi di output box
            SetWindowText(hOutputDecrypt, string_to_wstring(decryptedText).c_str());
        }
                                      break;
        }
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
