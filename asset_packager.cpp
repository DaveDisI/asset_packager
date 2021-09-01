#include "utilities.h"

#include <windows.h>
#include <stdio.h>

static bool stringsAreEqual(s8* s1, s8* s2){
    s8* a = s1;
    s8* b = s2;
    while(*a != '\0'){
        if(*a != *b){
            return false;
        }
        a++;
        b++;
    }

    return *a == *b;
}

static void copyDirectoryFiles(s8* rootDir, s8* folder, u32* assetFileSize, HANDLE assetFile, HANDLE headerFile){
    WIN32_FIND_DATA ffd;
    s8 str[256];
    u32 ctr = 0;
    concatenateCharacterStrings(str, rootDir, &ctr);
    ctr--;
    concatenateCharacterStrings(str, folder, &ctr);
    concatenateCharacterStrings(str, "\\*", &ctr);
    HANDLE dir = FindFirstFile(str, &ffd);
    if(!dir){
        printf("NO %s DIRECTORY FOUND\n", folder);
    }

    int fl = FindNextFile(dir, &ffd);
    while(fl){
        if(!stringsAreEqual(ffd.cFileName, "..") && !stringsAreEqual(ffd.cFileName, ".")) {
            printf("%s\n", ffd.cFileName);
            u8* fileData = (u8*)malloc(ffd.nFileSizeLow);
            memset(fileData, 0, ffd.nFileSizeLow);
            s8 fileName[64];
            s8 debugString[64];
            s8 filePath[256];
            u32 ctr2 = 0;
            DWORD bytesRead;
            DWORD bytesWritten;
            concatenateCharacterStrings(fileName, ffd.cFileName, &ctr2);
            fileName[ctr2 - 4] = '_';
            ctr2 = 0;
            concatenateCharacterStrings(filePath, rootDir, &ctr2);
            ctr2--;
            concatenateCharacterStrings(filePath, folder, &ctr2);
            concatenateCharacterStrings(filePath, "\\", &ctr2);
            concatenateCharacterStrings(filePath, ffd.cFileName, &ctr2);
            HANDLE file = CreateFile(filePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
            ReadFile(file, fileData, ffd.nFileSizeLow, &bytesRead, 0);
            WriteFile(assetFile, fileData, ffd.nFileSizeLow, &bytesWritten, 0);
            
            ctr2 = 0;
            concatenateCharacterStrings(filePath, "#define ", &ctr2);
            concatenateCharacterStrings(filePath, fileName, &ctr2);
            concatenateCharacterStrings(filePath, "_offset ", &ctr2);
            createDebugString(debugString, "%i", *assetFileSize);
            concatenateCharacterStrings(filePath, debugString, &ctr2);
            concatenateCharacterStrings(filePath, "\n", &ctr2);
            WriteFile(headerFile, filePath, ctr2, &bytesWritten, 0);
            ctr2 = 0;
            concatenateCharacterStrings(filePath, "#define ", &ctr2);
            concatenateCharacterStrings(filePath, fileName, &ctr2);
            concatenateCharacterStrings(filePath, "_size ", &ctr2);
            createDebugString(debugString, "%i", ffd.nFileSizeLow);
            concatenateCharacterStrings(filePath, debugString, &ctr2);
            concatenateCharacterStrings(filePath, "\n", &ctr2);
            WriteFile(headerFile, filePath, ctr2, &bytesWritten, 0);

            *assetFileSize += ffd.nFileSizeLow;
            CloseHandle(file);
            free(fileData);
        }   
        fl = FindNextFile(dir, &ffd);
    }
}

int main(int argc, char** argv){
    if(argc < 4){
        printf("asset_packager <assets directory> <output header file> <output data file>");
        return 1;
    }

    HANDLE headerFile = CreateFile(argv[2], GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    HANDLE assetFile = CreateFile(argv[3], GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    DWORD bytesWritten = 0;
    WriteFile(headerFile, "#pragma once\n", sizeof("#pragma once\n") - 1, &bytesWritten, 0);

    WIN32_FIND_DATA ffd;
    HANDLE directory = FindFirstFile(argv[1], &ffd);
    if(!directory){
        printf("NO FIRST FILE ERROR\n");
        return 1;
    }
    
    if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        int res = FindNextFile(directory, &ffd);
        u32 bytesWritten = 0;
        
        while(res){
            if(stringsAreEqual(ffd.cFileName, "fonts")){
                printf("FONTS\n");
                copyDirectoryFiles(argv[1], "fonts", &bytesWritten, assetFile, headerFile);
            }else if(stringsAreEqual(ffd.cFileName, "images")){
                printf("%s\n", ffd.cFileName);
            }else if(stringsAreEqual(ffd.cFileName, "shaders")){
                printf("%s\n", ffd.cFileName);
            }else if(stringsAreEqual(ffd.cFileName, "sounds")){
                copyDirectoryFiles(argv[1], "sounds", &bytesWritten, assetFile, headerFile);
            }
            res = FindNextFile(directory, &ffd);
        }
    }else{
        printf("NOT DIRECTORY ERROR\n");
        return 1;
    }

    CloseHandle(headerFile);
    CloseHandle(assetFile);

    printf("Done\n");
    return 0;
}