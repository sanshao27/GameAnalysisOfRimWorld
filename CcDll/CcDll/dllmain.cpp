// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include "resource.h"
#include "CcDll.h"

HMODULE g_hModule;
RECT  WinRect;
HWND hList;

//文本控件
HWND g_TextGoodsAddr;
HWND g_TextGoodsNum;
HWND g_TextGoodsNaijiu;
HWND g_TextGoodsZuobiao;
HWND g_TextGoodsName;

CHAR* fastResearchMem;//快速建造指令指针
CHAR* peopleStateMem;// 改变人物数据内存指针

PStateData g_pStateOfPeo;//当前人物状态

void InitFastResearchData()
{
	fastResearchMem = (CHAR*)malloc(0x2A);
	/*
		0208153B - D9 45 10              - fld dword ptr [ebp+10]
		0208153E - 39 71 0C              - cmp [ecx+0C],esi
		02081541 - 0F86 3F000000         - jbe 02081586
		--------跳到下面这
		02081547 - 8D 4C B1 10           - lea ecx,[ecx+esi*4+10]
		0208154B - D9 19                 - fstp dword ptr [ecx]

		0040EB19      8B09				 - mov ecx,dword ptr ds:[ecx]
		0040EB1B      81C1 FFFF0000		 - add ecx,0xFFFF



		0208154D - 8B 48 30              - mov ecx,[eax+30]
		02081550 - 41                    - inc ecx
		02081551 - 89 48 30              - mov [eax+30],ecx
		02081554 - 8D 65 F4              - lea esp,[ebp-0C]
		02081557 - 5E                    - pop esi
		02081558 - 5F                    - pop edi
		02081559 - 5B                    - pop ebx
		0208155A - C9                    - leave 
		0208155B - C3                    - ret 

		0040EAEA      81F9 ????????    cmp ecx,0x12345678 大
		0040EAF0      77 10            ja short 0040EB02
		0040EAF2      81F9 ????????    cmp ecx,0x12345678 小
		0040EAF8      76 08            jbe short 0040EB02
		0040EAFA      8B09             mov ecx,dword ptr ds:[ecx]
		0040EAFC      81C1 FFFF0000    add ecx,0xFFFF
		0040EB02      90               nop
		0040EB03      8B48 30          mov ecx,dword ptr ds:[eax+0x30]
		0040EB06      41               inc ecx
		0040EB07      8948 30          mov dword ptr ds:[eax+0x30],ecx
		0040EB0A      8D65 F4          lea esp,dword ptr ss:[ebp-0xC]
		0040EB0D      5E               pop esi
		0040EB0E      5F               pop edi
		0040EB0F      5B               pop ebx
		0040EB10      C9               leave
		0040EB11      C3               retn
		81F978563412771081F97856341276088B0981C1FFFF0000908B4830418948308D65F45E5F5BC9C3
	*/
	char changeByte[] = { 0x81,0xF9,0x78,0x56,0x34,0x12,0x77,0x13,0x81,0xF9,0x78,0x56,0x34,0x12,0x76,0x0B,0x50,0x8B,0x01,0x05,0xFF,0xFF,0x00,0x00,0x89,0x01,0x58,0x8B,0x48,0x30,0x41,0x89,0x48,0x30,0x8D,0x65,0xF4,0x5E,0x5F,0x5B,0xC9,0xC3 };

	//char changeByte[] = { 0x8D ,0x4C ,0xB1 ,0x10 ,0xD9 ,0x19 ,0x8B,0x09 ,0x81,0xC1 ,0xFF,0xFF,0x00,0x00,0x8B ,0x48 ,0x30 ,0x41 ,0x89 ,0x48 ,0x30,0x8d,0x65,0xf4,0x5e,0x5f,0x5b,0xc9,0xc3 };
	memcpy(fastResearchMem, changeByte, 0x2A);
	DWORD dwOldAttrubet;
	VirtualProtect(fastResearchMem,
		0x2A,
		PAGE_EXECUTE_READWRITE,
		&dwOldAttrubet);
}

void InitPeopleStateData()
{
	peopleStateMem = (CHAR*)malloc(0x200);
	/*
			56                                            push    esi
			8B 70 10                                      mov     esi, [eax+10h]
			81 FE 00 00 10 00                             cmp     esi, 100000h
			0F 8E 5D 01 00 00                             jle     loc_147016D
			8B 76 0C                                      mov     esi, [esi+0Ch]
			81 FE 00 00 10 00                             cmp     esi, 100000h
			0F 8E 4E 01 00 00                             jle     loc_147016D
			8B 76 0C                                      mov     esi, [esi+0Ch]
			81 FE 00 00 10 00                             cmp     esi, 100000h
			0F 8E 3F 01 00 00                             jle     loc_147016D
			8B 76 08                                      mov     esi, [esi+8]
			81 FE 00 00 10 00                             cmp     esi, 100000h
			0F 8E 30 01 00 00                             jle     loc_147016D
			8B 76 08                                      mov     esi, [esi+8]
			81 FE 00 00 10 00                             cmp     esi, 100000h
			0F 8E 21 01 00 00                             jle     loc_147016D
			81 7E 0C 63 00 6F 00                          cmp     dword ptr [esi+0Ch], 6F0063h
			0F 84 1F 00 00 00                             jz      loc_1470078
			81 7E 0C 74 00 72 00                          cmp     dword ptr [esi+0Ch], 720074h
			0F 84 12 00 00 00                             jz      loc_1470078
			81 7E 0C 50 00 6C 00                          cmp     dword ptr [esi+0Ch], 6C0050h
			0F 84 05 00 00 00                             jz      loc_1470078
			E9 F5 00 00 00                                jmp     loc_147016D
										  ; ---------------------------------------------------------------------------
			
										  loc_1470078:                           
																				 
			83 3D 78 01 47 01 00                          cmp     ds:people_mood, 0
			0F 84 16 00 00 00                             jz      loc_147009B
			8B 70 10                                      mov     esi, [eax+10h]
			81 FE 00 00 10 00                             cmp     esi, 100000h
			0F 8E D9 00 00 00                             jle     loc_147016D
			C7 46 14 00 00 80 3F                          mov     dword ptr [esi+14h], 3F800000h
			
										  loc_147009B:                           
			83 3D 7C 01 47 01 00                          cmp     ds:people_food, 0
			0F 84 16 00 00 00                             jz      loc_14700BE
			8B 70 14                                      mov     esi, [eax+14h]
			81 FE 00 00 10 00                             cmp     esi, 100000h
			0F 8E B6 00 00 00                             jle     loc_147016D
			C7 46 14 00 00 80 3F                          mov     dword ptr [esi+14h], 3F800000h
			
										  loc_14700BE:                            
			83 3D 80 01 47 01 00                          cmp     ds:people_rest, 0
			0F 84 16 00 00 00                             jz      loc_14700E1
			8B 70 18                                      mov     esi, [eax+18h]
			81 FE 00 00 10 00                             cmp     esi, 100000h
			0F 8E 93 00 00 00                             jle     loc_147016D
			C7 46 14 00 00 80 3F                          mov     dword ptr [esi+14h], 3F800000h
			
										  loc_14700E1:
			83 3D 84 01 47 01 00                          cmp     ds:people_joy, 0
			0F 84 16 00 00 00                             jz      loc_1470104
			8B 70 1C                                      mov     esi, [eax+1Ch]
			81 FE 00 00 10 00                             cmp     esi, 100000h
			0F 8E 70 00 00 00                             jle     loc_147016D
			C7 46 14 00 00 80 3F                          mov     dword ptr [esi+14h], 3F800000h
			
										  loc_1470104:                      
			83 3D 88 01 47 01 00                          cmp     ds:people_beauty, 0
			0F 84 16 00 00 00                             jz      loc_1470127
			8B 70 20                                      mov     esi, [eax+20h]
			81 FE 00 00 10 00                             cmp     esi, 100000h
			0F 8E 4D 00 00 00                             jle     loc_147016D
			C7 46 14 00 00 80 3F                          mov     dword ptr [esi+14h], 3F800000h
			
										  loc_1470127:                          
			83 3D 8C 01 47 01 00                          cmp     ds:people_space, 0
			0F 84 16 00 00 00                             jz      loc_147014A
			8B 70 24                                      mov     esi, [eax+24h]
			81 FE 00 00 10 00                             cmp     esi, 100000h
			0F 8E 2A 00 00 00                             jle     loc_147016D
			C7 46 14 00 00 80 3F                          mov     dword ptr [esi+14h], 3F800000h
			
										  loc_147014A:                         
			83 3D 90 01 47 01 00                          cmp     ds:people_comfort, 0
			0F 84 16 00 00 00                             jz      loc_147016D
			8B 70 28                                      mov     esi, [eax+28h]
			81 FE 00 00 10 00                             cmp     esi, 100000h
			0F 8E 07 00 00 00                             jle     loc_147016D
			C7 46 14 00 00 80 3F                          mov     dword ptr [esi+14h], 3F800000h
			
										  loc_147016D:                       
			5E                                            pop     esi
			8B 40 10                                      mov     eax, [eax+10h]
			85 C0                                         test    eax, eax
			E9 ?? ?? ?? ??                                jmp     --old address
										  sub_1470000     endp
			
										  ; ---------------------------------------------------------------------------
			00 00 00 00                   people_mood     dd 1
			00 00 00 00                   people_food     dd 1
			00 00 00 00                   people_rest     dd 1
			00 00 00 00                   people_joy      dd 1
			00 00 00 00                   people_beauty   dd 0
			00 00 00 00                   people_space    dd 0
			01 00 00 00					  people_comfort  dd 1
			00 00 00 00

			3F800000代表单浮点数的1
	*/
char changePeopleStateByte[] = {
	0x56,0x8B,0x70,0x10,0x81,0xFE,0x00,0x00,0x10,0x00,0x0F,0x8E,0x5D,0x01,0x00,0x00,
	0x8B,0x76,0x0C,0x81,0xFE,0x00,0x00,0x10,0x00,0x0F,0x8E,0x4E,0x01,0x00,0x00,0x8B,
	0x76,0x0C,0x81,0xFE,0x00,0x00,0x10,0x00,0x0F,0x8E,0x3F,0x01,0x00,0x00,0x8B,0x76,
	0x08,0x81,0xFE,0x00,0x00,0x10,0x00,0x0F,0x8E,0x30,0x01,0x00,0x00,0x8B,0x76,0x08,
	0x81,0xFE,0x00,0x00,0x10,0x00,0x0F,0x8E,0x21,0x01,0x00,0x00,0x81,0x7E,0x0C,0x63,
	0x00,0x6F,0x00,0x0F,0x84,0x1F,0x00,0x00,0x00,0x81,0x7E,0x0C,0x74,0x00,0x72,0x00,
	0x0F,0x84,0x12,0x00,0x00,0x00,0x81,0x7E,0x0C,0x50,0x00,0x6C,0x00,0x0F,0x84,0x05,
	0x00,0x00,0x00,0xE9,0xF5,0x00,0x00,0x00,0x83,0x3D,0x78,0x01,0x47,0x01,0x00,0x0F,
	0x84,0x16,0x00,0x00,0x00,0x8B,0x70,0x10,0x81,0xFE,0x00,0x00,0x10,0x00,0x0F,0x8E,
	0xD9,0x00,0x00,0x00,0xC7,0x46,0x14,0x00,0x00,0x80,0x3F,0x83,0x3D,0x7C,0x01,0x47,
	0x01,0x00,0x0F,0x84,0x16,0x00,0x00,0x00,0x8B,0x70,0x14,0x81,0xFE,0x00,0x00,0x10,
	0x00,0x0F,0x8E,0xB6,0x00,0x00,0x00,0xC7,0x46,0x14,0x00,0x00,0x80,0x3F,0x83,0x3D,
	0x80,0x01,0x47,0x01,0x00,0x0F,0x84,0x16,0x00,0x00,0x00,0x8B,0x70,0x18,0x81,0xFE,
	0x00,0x00,0x10,0x00,0x0F,0x8E,0x93,0x00,0x00,0x00,0xC7,0x46,0x14,0x00,0x00,0x80,
	0x3F,0x83,0x3D,0x84,0x01,0x47,0x01,0x00,0x0F,0x84,0x16,0x00,0x00,0x00,0x8B,0x70,
	0x1C,0x81,0xFE,0x00,0x00,0x10,0x00,0x0F,0x8E,0x70,0x00,0x00,0x00,0xC7,0x46,0x14,
	0x00,0x00,0x80,0x3F,0x83,0x3D,0x88,0x01,0x47,0x01,0x00,0x0F,0x84,0x16,0x00,0x00,
	0x00,0x8B,0x70,0x20,0x81,0xFE,0x00,0x00,0x10,0x00,0x0F,0x8E,0x4D,0x00,0x00,0x00,
	0xC7,0x46,0x14,0x00,0x00,0x80,0x3F,0x83,0x3D,0x8C,0x01,0x47,0x01,0x00,0x0F,0x84,
	0x16,0x00,0x00,0x00,0x8B,0x70,0x28,0x81,0xFE,0x00,0x00,0x10,0x00,0x0F,0x8E,0x2A,
	0x00,0x00,0x00,0xC7,0x46,0x14,0x00,0x00,0x80,0x3F,0x83,0x3D,0x90,0x01,0x47,0x01,
	0x00,0x0F,0x84,0x16,0x00,0x00,0x00,0x8B,0x70,0x2C,0x81,0xFE,0x00,0x00,0x10,0x00,
	0x0F,0x8E,0x07,0x00,0x00,0x00,0xC7,0x46,0x14,0x00,0x00,0x80,0x3F,0x5E,0x8B,0x40,
	0x10,0x85,0xC0,0xE9,0x90,0x90,0x90,0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

	memcpy(peopleStateMem, changePeopleStateByte, 0x200);
	DWORD dwOldAttrubet;
	VirtualProtect(peopleStateMem,
		0x200,
		PAGE_EXECUTE_READWRITE,
		&dwOldAttrubet);
	g_pStateOfPeo = (PStateData)(peopleStateMem + 0x178);
	*(DWORD*)(peopleStateMem + 0x7a) = (DWORD)(&g_pStateOfPeo->mood);
	*(DWORD*)(peopleStateMem + 0x9d) = (DWORD)(&g_pStateOfPeo->food);
	*(DWORD*)(peopleStateMem + 0xc0) = (DWORD)(&g_pStateOfPeo->rest);
	*(DWORD*)(peopleStateMem + 0xe3) = (DWORD)(&g_pStateOfPeo->joy);
	*(DWORD*)(peopleStateMem + 0x106) = (DWORD)(&g_pStateOfPeo->beauty);
	*(DWORD*)(peopleStateMem + 0x129) = (DWORD)(&g_pStateOfPeo->space);
	*(DWORD*)(peopleStateMem + 0x14c) = (DWORD)(&g_pStateOfPeo->comfort);
}

HWND InitListView(HWND hwndDlg)
{

	hList = GetDlgItem(g_hwndDlg, IDC_LIST1);

	SetFocus(hList);   //获得焦点

	ListView_SetItemState(hList, 0, LVIS_SELECTED, LVIS_SELECTED);       //使第一行处于选择中状态

	ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT);      //设置整行选择风格
	//3.2 添加列
	LVCOLUMN ln;
	ln.mask = LVCF_WIDTH | LVCF_TEXT;
	ln.cx = 150;//宽度
	ln.pszText = (PTCHAR)_T("名称");//列名 
	ln.cchTextMax = 3;      //列名字符个数
	ListView_InsertColumn(
		hList,
		0,
		&ln
	);
	ln.cx = 50;//宽度
	ln.pszText = (PTCHAR)_T("座标x");//列名 
	ListView_InsertColumn(
		hList,
		1,
		&ln
	);
	ln.pszText = (PTCHAR)_T("座标y");//列名 
	ListView_InsertColumn(
		hList,
		2,
		&ln
	);
	ln.pszText = (PTCHAR)_T("数量");//列名 
	ListView_InsertColumn(
		hList,
		3,
		&ln
	);
	ln.pszText = (PTCHAR)_T("耐久");//列名 
	ListView_InsertColumn(
		hList,
		5,
		&ln
	);
	ln.cx = 80;//宽度
	ln.pszText = (PTCHAR)_T("地址");//列名 
	ListView_InsertColumn(
		hList,
		5,
		&ln
	);

	return hList;
}

VOID GetListGoodsInfo(int nItem)
{
	char *pBuf;
	int len = 100;
	pBuf = new char[len];
	memset(pBuf, 0, len);
	ListView_GetItemText(hList, nItem, 5, pBuf, len);
	SetDlgItemText(g_hwndDlg, IDC_THIS_GOODS_ADDR, pBuf);
	memset(pBuf, 0, len);
	ListView_GetItemText(hList, nItem, 3, pBuf, len);
	SetDlgItemText(g_hwndDlg, IDC_GOODS_NUM, pBuf);
	memset(pBuf, 0, len);
	ListView_GetItemText(hList, nItem, 4, pBuf, len);
	SetDlgItemText(g_hwndDlg, IDC_NAIJIU, pBuf);
	memset(pBuf, 0, len);
	ListView_GetItemText(hList, nItem, 2, pBuf, len);
	SetDlgItemText(g_hwndDlg, IDC_ZUOBIAO2, pBuf);
	memset(pBuf, 0, len);
	ListView_GetItemText(hList, nItem, 1, pBuf, len);
	SetDlgItemText(g_hwndDlg, IDC_ZUOBIAO, pBuf);
	memset(pBuf, 0, len);
	ListView_GetItemText(hList, nItem, 0, pBuf, len);
	SetDlgItemText(g_hwndDlg, IDC_GOODS_NAME, pBuf);

	delete[] pBuf;


}

VOID SetStateIcon(HWND hwndDlg,DWORD IDC_STATIC, DWORD IDI_ICON)
{
	HWND hStatic = GetDlgItem(hwndDlg, IDC_STATIC);
	HICON hIco = LoadIcon(g_hModule, MAKEINTRESOURCE(IDI_ICON));
	SendMessage(hStatic, STM_SETIMAGE, IMAGE_ICON, (LPARAM)hIco);
}


VOID SetPeopleStateIcon(HWND hwndDlg)
{

	g_pStateOfPeo->space == 0 ? SetStateIcon(hwndDlg, IDC_STATIC_SPACE, IDI_ICON_SPACE_OFF) : SetStateIcon(hwndDlg, IDC_STATIC_SPACE, IDI_ICON_SPACE_ON);
	g_pStateOfPeo->beauty == 0 ? SetStateIcon(hwndDlg, IDC_STATIC_BEAUTY, IDI_ICON_BEAUTY_OFF) : SetStateIcon(hwndDlg, IDC_STATIC_BEAUTY, IDI_ICON_BEAUTY_ON);
	g_pStateOfPeo->comfort == 0 ? SetStateIcon(hwndDlg, IDC_STATIC_COMFORT, IDI_ICON_COMFORT_OFF) : SetStateIcon(hwndDlg, IDC_STATIC_COMFORT, IDI_ICON_COMFORT_ON);
	g_pStateOfPeo->food == 0 ? SetStateIcon(hwndDlg, IDC_STATIC_FOOD, IDI_ICON_FOOD_OFF) : SetStateIcon(hwndDlg, IDC_STATIC_FOOD, IDI_ICON_FOOD_ON);
	g_pStateOfPeo->joy == 0 ? SetStateIcon(hwndDlg, IDC_STATIC_JOY, IDI_ICON_JOY_OFF) : SetStateIcon(hwndDlg, IDC_STATIC_JOY, IDI_ICON_JOY_ON);
	g_pStateOfPeo->mood == 0 ? SetStateIcon(hwndDlg, IDC_STATIC_MOOD, IDI_ICON_MOOD_OFF) : SetStateIcon(hwndDlg, IDC_STATIC_MOOD, IDI_ICON_MOOD_ON);
	g_pStateOfPeo->rest == 0 ? SetStateIcon(hwndDlg, IDC_STATIC_REST, IDI_ICON_REST_OFF) : SetStateIcon(hwndDlg, IDC_STATIC_REST, IDI_ICON_REST_ON);
	SetStateIcon(hwndDlg, IDC_STATIC_RESEARCH, IDI_ICON_RESEARCH_OFF);
	SetStateIcon(hwndDlg, IDC_STATIC_BULID, IDI_ICON_BULID_OFF);

}

INT_PTR CALLBACK DialogProc(
	__in  HWND hwndDlg,
	__in  UINT uMsg,
	__in  WPARAM wParam,
	__in  LPARAM lParam);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		g_hModule = hModule;
		DialogBox(hModule,MAKEINTRESOURCE(IDD_MAJOR),NULL,&DialogProc);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

INT_PTR CALLBACK DialogProc(
	__in  HWND hwndDlg,
	__in  UINT uMsg,
	__in  WPARAM wParam,
	__in  LPARAM lParam
)
{
	static HBRUSH hbrBkgnd = NULL;
	switch (uMsg)
	{	
	case WM_CTLCOLORSTATIC:
	{//控件背景返回小埋色
		HDC hdc = (HDC)wParam;
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, RGB(0, 0, 0));
		if (hbrBkgnd == NULL)
		{
			hbrBkgnd = CreateSolidBrush(RGB(236, 153, 101));
		}
		return (INT_PTR)hbrBkgnd;
	}
	case WM_PAINT:
	{
		HBITMAP    hbm;
		BITMAP    bminfo;
		hbm = LoadBitmap(g_hModule, MAKEINTRESOURCE(IDB_BITMAP_BACKDROP));
		GetObject(hbm, sizeof(bminfo), &bminfo);
		PAINTSTRUCT  ps;
		HDC dc = BeginPaint(hwndDlg, &ps);
		HDC memdc = CreateCompatibleDC(dc);
		SelectObject(memdc, hbm);
		BitBlt(dc, 0, 0, bminfo.bmWidth, bminfo.bmHeight, memdc, 0, 0, SRCCOPY);
		DeleteDC(memdc);
		EndPaint(hwndDlg, &ps);
		break;
	}
	case WM_INITDIALOG:
	{
		InitFastResearchData();
		InitPeopleStateData();
		SetPeopleStateIcon(hwndDlg);
		g_hwndDlg = hwndDlg;
		g_hList = InitListView(hwndDlg);
		EnumProcess(hwndDlg);
		UpdateWindow(hwndDlg);
		break;
	}
	case WM_CLOSE:
		UnHook(g_funAddress);
		EndDialog(hwndDlg,0);
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_CLICK:
			if (((LPNMHDR)lParam)->idFrom == IDC_LIST1)
			{
				LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE)lParam;//将传进来的消息转换成LPNMITEMACTIVAT
				int nItem = temp->iItem;//获得行号
				int nSubItem = temp->iSubItem;//获得列号
				GetListGoodsInfo(nItem);
				UpdateWindow(hwndDlg);
				return TRUE;

			}
			// More cases on WM_NOTIFY switch.
			break;
		}
		break;
	case WM_COMMAND:
	{
		SHORT Id = LOWORD(wParam);
		switch (Id)
		{
		case IDCANCEL:
			break;
		case IDOK:
			break;
		case IDC_BUTTON_ADD10:
			addGoodsNumber(10, hwndDlg);
			break;
		case IDC_BUTTON_ADD100:
			addGoodsNumber(100, hwndDlg);
			break;
		case IDC_BUTTON_ADD1000:
			addGoodsNumber(1000, hwndDlg);
			break;
		case IDC_BUTTON_ADD_NAIJIU:
			addGoodsDurable(10, hwndDlg);
			break;
		case IDC_FAST_RESEARCH:
			ChangeFastResearch(hwndDlg, fastResearchMem);
			SetStateIcon(hwndDlg, IDC_STATIC_RESEARCH, IDI_ICON_RESEARCH_ON);
			break;
		case IDC_FAST_BUILD:
			ChangeFastBulid();
			SetStateIcon(hwndDlg, IDC_STATIC_BULID, IDI_ICON_BULID_ON);
			break;


		//--------------------------people data
		case IDC_MAX_SPACE:
			ChangePeopleState(peopleStateMem, g_pStateOfPeo, IDC_MAX_SPACE);
			SetPeopleStateIcon(hwndDlg);
			break;
		case IDC_MAX_BEAUTY:
			ChangePeopleState(peopleStateMem, g_pStateOfPeo, IDC_MAX_BEAUTY);
			SetPeopleStateIcon(hwndDlg);
			break;
		case IDC_MAX_COMFORT:
			ChangePeopleState(peopleStateMem, g_pStateOfPeo, IDC_MAX_COMFORT);
			SetPeopleStateIcon(hwndDlg);
			break;
		case IDC_MAX_FOOD:
			ChangePeopleState(peopleStateMem, g_pStateOfPeo, IDC_MAX_FOOD);
			SetPeopleStateIcon(hwndDlg);
			break;
		case IDC_MAX_JOY:
			ChangePeopleState(peopleStateMem, g_pStateOfPeo, IDC_MAX_JOY);
			SetPeopleStateIcon(hwndDlg);
			break;
		case IDC_MAX_REST:
			ChangePeopleState(peopleStateMem, g_pStateOfPeo, IDC_MAX_REST);
			SetPeopleStateIcon(hwndDlg);
			break;
		
		case IDC_MAX_MOOD:
			ChangePeopleState(peopleStateMem, g_pStateOfPeo, IDC_MAX_MOOD);
			SetPeopleStateIcon(hwndDlg);
			break;
		default:
				break;	
		}
		break;
	}
	default:
		break;
	}
	return FALSE;
}



