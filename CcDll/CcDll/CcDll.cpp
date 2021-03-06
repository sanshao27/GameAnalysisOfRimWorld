// CcDll.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"
#include "CcDll.h"

/*访问研究数值修改位置的特征
1607BBC5 - 8D 4C B1 10           - lea ecx,[ecx+esi*4+10]
1607BBC9 - D9 19                 - fstp dword ptr [ecx]
1607BBCB - 8B 48 30              - mov ecx,[eax+30]
1607BBCE - 41                    - inc ecx
1607BBCF - 89 48 30              - mov [eax+30],ecx
1607BBD2 - 8D 65 F4              - lea esp,[ebp-0C]
1607BBD5 - 5E                    - pop esi
1607BBD6 - 5F                    - pop edi
1607BBD7 - 5B                    - pop ebx
1607BBD8 - C9                    - leave 
1607BBD9 - C3                    - ret 
  */  
char mem_of_fast_research[] = { 0x8D ,0x4C, 0xB1, 0x10,0xD9, 0x19,0x8B, 0x48 ,0x30,0x41,0x89, 0x48, 0x30,0x8D, 0x65, 0xF4,0x5E,0x5F,0x5B,0xC9,0xC3,0x00 };

/*调用物品信息的函数特征，他的第二个参数是我能获得的物品结构地址，
这个函数是一个大循环里面的，会一直执行
028F1790    55              push ebp
028F1791    8BEC            mov ebp,esp
028F1793    83EC 08         sub esp,0x8
028F1796    8B4D 0C         mov ecx,dword ptr ss:[ebp+0xC]
028F1799    8B41 34         mov eax,dword ptr ds:[ecx+0x34]
028F179C    8B49 08         mov ecx,dword ptr ds:[ecx+0x8]
028F179F    83EC 04         sub esp,0x4
028F17A2    51              push ecx
028F17A3    50              push eax
028F17A4    FF75 08         push dword ptr ss:[ebp+0x8]
*/
char mem_of_goods[] = { 0x55,0x8B,0xEC,0x83,0xEC,0x08,0x8B,0x4D,0x0C,0x8B,0x41,0x34,0x8B,0x49,0x08,0x83,0xEC,0x04,0x51,0x50,0xFF,0x75,0x08,0x00 };
char mem_of_research_list[] = { 0x00 ,0x00 ,0x96 ,0x43 ,0x00 ,0x00 ,0xC8 ,0x43 ,0x00 ,0x00 ,0x96 ,0x43 ,0x00 ,0x00 ,0x16 ,0x44,0x00 };

/*循环访问所有人物结构特征
24ADB2C8 - 55                    - push ebp
24ADB2C9 - 8B EC                 - mov ebp,esp
24ADB2CB - 83 EC 08              - sub esp,08 { 00000008 }
24ADB2CE - 8B 45 08              - mov eax,[ebp+08]
24ADB2D1 - 8B 40 08              - mov eax,[eax+08]
24ADB2D4 - 8B 40 64              - mov eax,[eax+64]
24ADB2D7 - 8B 40 10              - mov eax,[eax+10]
24ADB2DA - 85 C0                 - test eax,eax*/
char mem_of_callpeople[] = { 0x55,0x8b,0xec,0x83, 0xec, 0x08, 0x8b, 0x45, 0x08, 0x8b, 0x40, 0x08, 0x8b, 0x40, 0x64, 0x8b, 0x40, 0x10, 0x85, 0xc0,0x00 };

/*
seg000:0C4EEE56 D9 43 58                                      fld     dword ptr [ebx+58h]
seg000:0C4EEE59 D9 45 F0                                      fld     [ebp+var_10]
seg000:0C4EEE5C DE C1                                         faddp   st(1), st
seg000:0C4EEE5E D9 5B 58                                      fstp    dword ptr [ebx+58h]
seg000:0C4EEE61 D9 43 58                                      fld     dword ptr [ebx+58h]
seg000:0C4EEE64 D9 45 E0                                      fld     [ebp+var_20]
seg000:0C4EEE67 DF F1                                         fcomip  st, st(1)
seg000:0C4EEE69 DD D8                                         fstp    st
seg000:0C4EEE6B 7A 23                                         jp      short loc_C4EEE90 ---nop
seg000:0C4EEE6D 77 21                                         ja      short loc_C4EEE90 ---nop
seg000:0C4EEE6F 83 EC 08                                      sub     esp, 8
seg000:0C4EEE72 56                                            push    esi
seg000:0C4EEE73 53                                            push    ebx
seg000:0C4EEE74 39 1B                                         cmp     [ebx], ebx
*/
char mem_of_fast_bulid[] = { 0xD9, 0x43, 0x58, 0xD9, 0x45, 0xF0, 0xDE, 0xC1, 0xD9, 0x5B, 0x58, 0xD9, 0x43, 0x58, 0xD9, 0x45, 0xE0, 0xDF, 0xF1, 0xDD, 0xD8, 0x7A, 0x23, 0x77, 0x21, 0x83, 0xEC, 0x08, 0x56, 0x53, 0x39, 0x1B ,0x00 };

PDWORD g_fastResearchAddress;//用于获取快速建造地址的全局变量
PDWORD g_changePeopleAddress;//用于获取人物状态地址的全局变量

/*
		typedef struct _MEMORY_BASIC_INFORMATION {
		  PVOID  BaseAddress;			// 内存块基地址指针
		  PVOID  AllocationBase;		// VirtualAlloc 函数分配的基地址指针
		  DWORD  AllocationProtect;		// 内存块初始内存保护属性
		  SIZE_T RegionSize;			// 内存块大小
		  DWORD  State;					// 内存块状态（COMMIT、FREE、RESERVE）
		  DWORD  Protect;				// 内存块当前内存保护属性
		  DWORD  Type;					// 内存块类型（IMAGE、MAPPED、PRIVATE）
		} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;


		PAGE_NOACCESS
		PAGE_READONLY
		PAGE_READWRITE
		PAGE_EXECUTE
		PAGE_EXECUTE_READ
		PAGE_EXECUTE_READWRITE
*/
vector<MEMORY_BASIC_INFORMATION> vec;//用于遍历内存地址的容器

vector<long> vecCodeAddr;//保存所有命中特征的地址

//匹配内存中参数特征的位置
long MemoryCmpToFeatureCode(PVOID BaseAddress, long mem_size, PCHAR Strings, int isstrlen = 0)
{
	vecCodeAddr.clear();
	vecCodeAddr.reserve(200);
	int index = 0;
	int flag = -1;
	int strlens;
	//获取特征长度
	if (isstrlen == 0)
	{
		strlens = strlen(Strings);
	}
	else
	{
		strlens = isstrlen;
	}
	for (long j = 0; j < mem_size - strlens; j++)
	{
		index = 0;
		for (int i = 0; i < strlens; i++)
		{
			char* compare = (char*)BaseAddress;
			//判断内存特征与传进来的特征对比
			if (compare[j + i] != Strings[i])
			{
				break;
			}
			index++;
		}
		if (index == strlens)
		{
			vecCodeAddr.push_back(j);
			flag = 1;
		}
	}
	return flag;
}

//枚举指定进程所有内存块
//assert(hProcess != nullptr);
//参数:
//  hProcess:  要枚举的进程,需拥有PROCESS_QUERY_INFORMATION权限
//  memories:  返回枚举到的内存块数组
//返回:
//  成功返回true,失败返回false.
static bool EnumAllMemoryBlocks(HANDLE hProcess, OUT vector<MEMORY_BASIC_INFORMATION>& memories) {
	// 如果 hProcess 为空则结束运行
	assert(hProcess != nullptr);

	// 初始化 vector 容量
	memories.clear();
	memories.reserve(200);

	// 获取 PageSize 和地址粒度
	SYSTEM_INFO sysInfo = { 0 };
	GetSystemInfo(&sysInfo);
	/*
		typedef struct _SYSTEM_INFO {
		  union {
			DWORD dwOemId;							// 兼容性保留
			struct {
			  WORD wProcessorArchitecture;			// 操作系统处理器体系结构
			  WORD wReserved;						// 保留
			} DUMMYSTRUCTNAME;
		  } DUMMYUNIONNAME;
		  DWORD     dwPageSize;						// 页面大小和页面保护和承诺的粒度
		  LPVOID    lpMinimumApplicationAddress;	// 指向应用程序和dll可访问的最低内存地址的指针
		  LPVOID    lpMaximumApplicationAddress;	// 指向应用程序和dll可访问的最高内存地址的指针
		  DWORD_PTR dwActiveProcessorMask;			// 处理器掩码
		  DWORD     dwNumberOfProcessors;			// 当前组中逻辑处理器的数量
		  DWORD     dwProcessorType;				// 处理器类型，兼容性保留
		  DWORD     dwAllocationGranularity;		// 虚拟内存的起始地址的粒度
		  WORD      wProcessorLevel;				// 处理器级别
		  WORD      wProcessorRevision;				// 处理器修订
		} SYSTEM_INFO, *LPSYSTEM_INFO;
	*/

	//遍历内存
	const char* p = (const char*)sysInfo.lpMinimumApplicationAddress;
	MEMORY_BASIC_INFORMATION  memInfo = { 0 };
	while (p < sysInfo.lpMaximumApplicationAddress) {
		// 获取进程虚拟内存块缓冲区字节数
		size_t size = VirtualQueryEx(
			hProcess,								// 进程句柄
			p,										// 要查询内存块的基地址指针
			&memInfo,								// 接收内存块信息的 MEMORY_BASIC_INFORMATION 对象
			sizeof(MEMORY_BASIC_INFORMATION32)		// 缓冲区大小
		);
		if (size != sizeof(MEMORY_BASIC_INFORMATION32)) { break; }

		// 经过分析我们需要的内存块属性分别是是PAGE_EXECUTE_READWRITE\MEM_COMMIT\MEM_PRIVATE，所以将不必要的内存块排除掉
		if (memInfo.Protect == PAGE_EXECUTE_READWRITE)
			if (memInfo.State == MEM_COMMIT)
				if (memInfo.Type == MEM_PRIVATE)
					memories.push_back(memInfo);// 将内存块信息追加到 vector 数组尾部

		// 移动指针
		p += memInfo.RegionSize;
	}

	return memories.size() > 0;
}

//遍历所有进程找到游戏窗口，获取物品特征
void EnumProcess(HWND huwdlg)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);		// 进程快照句柄
	PROCESSENTRY32 process = { sizeof(PROCESSENTRY32) };							// 接收进程信息的对象

	// 遍历进程
	while (Process32Next(hProcessSnap, &process)) {
		// 找到想要的进程
		if (strcmp(process.szExeFile, "RimWorldWin.exe") == 0) {
			// 获取进程句柄
			HANDLE h_rundll32 = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process.th32ProcessID);
			if (!h_rundll32) { MessageBox(NULL,"OpenProcess failed.", "failed",NULL); }

			int addressOffset = 0;
			// 遍历该进程的内存块
			if (EnumAllMemoryBlocks(h_rundll32, vec)) {
				for (int i = 0; i < vec.size(); i++) {
					// 查找物品特征
					addressOffset = MemoryCmpToFeatureCode(vec[i].BaseAddress, vec[i].RegionSize, mem_of_goods);
					if (addressOffset != -1)//物品函数特征地址查找
					{
						for (int j = 0; j < vecCodeAddr.size(); j++) {
							int int_value = (int)vec[i].BaseAddress + (int)vecCodeAddr[j];
							g_funAddress = (PDWORD)int_value;
							CString temp_value = _T("");   //temp_value用来处理int值
							temp_value.Format(_T("%x"), int_value);//固定格式
							SetDlgItemText(huwdlg, IDC_PHONE_NUMBER, temp_value);
							OnHook(g_funAddress);
						}
					}
				}
			}
			else { MessageBox(NULL, "EnumAllMemoryBlocks failed.","failed", NULL); }
		}

	}
}

//修改物品数量
DWORD addGoodsNumber(int number,HWND huwdlg)
{
	char str[10] = { 0 };
	GetDlgItemText(huwdlg, IDC_THIS_GOODS_ADDR, str, 10);
	int address = strtol(str, NULL, 16);
	if (address < 2)
	{
		MessageBox(NULL, "未选中物品", "错误", NULL);
		return 0;
	}

	PGoodsInfo changeGoods = (PGoodsInfo)address;
	changeGoods->GoodsNumber += number;
	g_start = true;
	MessageBox(NULL, "添加成功", "成功", NULL);
	return true;
}

//修改物品耐久
DWORD addGoodsDurable(int number, HWND huwdlg)
{
	char str[10] = { 0 };
	GetDlgItemText(huwdlg, IDC_THIS_GOODS_ADDR, str, 10);
	int address = strtol(str, NULL, 16);
	if (address < 2)
	{
		MessageBox(NULL, "未选中物品", "错误", NULL);
		return 0;
	}
	PGoodsInfo changeGoods = (PGoodsInfo)address;
	if (changeGoods->GoodsDurable == -1)
	{
		MessageBox(NULL, "该物品没有耐久", "错误", NULL);
		return 0;
	}
	
	changeGoods->GoodsDurable += 10;
	g_start = true;
	MessageBox(NULL, "添加成功", "成功", NULL);
	return true;
}


VOID ChangeFastResearch(HWND huwdlg, char* fastResearchMem)
{
	HANDLE selfProHandle = GetCurrentProcess();
	DWORD dwOffset;
	/*
	02081557 - E9 00000000           - jmp xxxxxxxx 跳转到申请的地址
	*/
	char changeByte2[] = { 0xE9,0x90,0x90 ,0x90 ,0x90 };
	bool flag_research = true;
	int addressOffset = 0;
	int offset_addr = 0;
	if (EnumAllMemoryBlocks(selfProHandle, vec)) {
		for (int i = 0; i < vec.size(); i++) {
			// 查找快速研究数据
			if (flag_research)
			{
				addressOffset = MemoryCmpToFeatureCode(vec[i].BaseAddress, vec[i].RegionSize, mem_of_research_list,0x10);
				if (addressOffset != -1)
				{
					for (int j = 0; j < vecCodeAddr.size(); j++) {
						flag_research = false;
						offset_addr = (int)vec[i].BaseAddress + (int)vecCodeAddr[j];
						*(DWORD*)(fastResearchMem + 0x2) = offset_addr + 0x100;
						*(DWORD*)(fastResearchMem + 0xA) = offset_addr - 0x100;
						break;
					}
				}
			}
			
			// 查找快速研究特征
			addressOffset = MemoryCmpToFeatureCode(vec[i].BaseAddress, vec[i].RegionSize, mem_of_fast_research);
			if (addressOffset != -1)
			{
				for (int j = 0; j < vecCodeAddr.size(); j++) {

					int int_value = (int)vec[i].BaseAddress + (int)vecCodeAddr[j] ;
					g_fastResearchAddress = (PDWORD)(int_value + 0x6);
					dwOffset = ((DWORD)fastResearchMem - ((DWORD)g_fastResearchAddress)) - 0x5;

					*(DWORD*)(changeByte2 + 0x1) = dwOffset;
					memcpy(g_fastResearchAddress, changeByte2, 0x5);
				}
			}
		}
	}
	else 
	{ MessageBox(NULL, "EnumAllMemoryBlocks failed.", "failed", NULL); }

}


bool ChangePeopleMem(char* changePeopleMem)
{
	HANDLE selfProHandle = GetCurrentProcess();
	DWORD dwOffset;
	/*
	address + 0000 0000 - 55                    - push ebp
	address + 0000 0001 - 8B EC                 - mov ebp,esp
	address + 0000 0003 - 83 EC 08              - sub esp,08
	address + 0000 0006 - 8B 45 08              - mov eax,[ebp+08]
	address + 0000 0009 - 8B 40 08              - mov eax,[eax+08]
	address + 0000 000C - 8B 40 64              - mov eax,[eax+64]
	address + 0000 000F - 8B 40 10              - mov eax,[eax+10]  ---------------------------》改到自己的内存 E9 ?? ?? ?? ??  
	address + 0000 0012 - 85 C0                 - test eax,eax  
	address + 0000 0014 - 75 08                 - jne 24D70D26    《----------------------------运行完自己的逻辑在跳转回来
	*/
	char changePeopleByte[] = { 0xE9,0x90,0x90 ,0x90 ,0x90 };
	bool flag_research = true;
	int addressOffset = 0;
	int offset_addr = 0;
	if (EnumAllMemoryBlocks(selfProHandle, vec)) {
		for (int i = 0; i < vec.size(); i++) {
			// 查找特征
			addressOffset = MemoryCmpToFeatureCode(vec[i].BaseAddress, vec[i].RegionSize, mem_of_callpeople);
			if (addressOffset != -1)
			{
				for (int j = 0; j < vecCodeAddr.size(); j++) {

					int int_value = (int)vec[i].BaseAddress + (int)vecCodeAddr[j];
					g_changePeopleAddress = (PDWORD)(int_value + 0xF);//定位到修改
					dwOffset = ((DWORD)changePeopleMem - ((DWORD)g_changePeopleAddress)) - 0x5;

					*(DWORD*)(changePeopleByte + 0x1) = dwOffset;
					memcpy(g_changePeopleAddress, changePeopleByte, 0x5);

					g_changePeopleAddress = (PDWORD)(int_value + 0x14);
					dwOffset =(DWORD)g_changePeopleAddress - (DWORD)(changePeopleMem + 0x173)-0x5;//跳转回来
					*(DWORD*)(changePeopleMem + 0x173 + 0x1) = dwOffset;
					return true;
				}
			}
		}
	}
	else
	{
		MessageBox(NULL, "memallpeople failed.", "failed", NULL);
		return false;
	}

}


bool findPeopleMeFlage = false;
void ChangePeopleState(char* changePeopleMem, PStateData peopleStates, DWORD isFlage)
{

	if (findPeopleMeFlage == false)
	{
		findPeopleMeFlage = ChangePeopleMem(changePeopleMem);
		if (findPeopleMeFlage == false)
			return;
	}

	switch (isFlage)
	{
	case IDC_MAX_SPACE:
		peopleStates->space = (peopleStates->space == 0) ? 1 : 0;
		break;
	case IDC_MAX_BEAUTY:
		peopleStates->beauty = (peopleStates->beauty == 0) ? 1 : 0;
		break;
	case IDC_MAX_COMFORT:
		peopleStates->comfort = (peopleStates->comfort == 0) ? 1 : 0;
		break;
	case IDC_MAX_FOOD:
		peopleStates->food = (peopleStates->food == 0) ? 1 : 0;
		break;
	case IDC_MAX_JOY:
		peopleStates->joy = (peopleStates->joy == 0) ? 1 : 0;
		break;
	case IDC_MAX_REST:
		peopleStates->rest = (peopleStates->rest == 0) ? 1 : 0;
		break;
	case IDC_MAX_MOOD:
		peopleStates->mood = (peopleStates->mood == 0) ? 1 : 0;
		break;
	default:
		break;
	}
}


VOID ChangeFastBulid()
{
	HANDLE selfProHandle = GetCurrentProcess();
	int addressOffset = 0;
	if (EnumAllMemoryBlocks(selfProHandle, vec)) {
		for (int i = 0; i < vec.size(); i++) {

			// 查找快速建造特征
			addressOffset = MemoryCmpToFeatureCode(vec[i].BaseAddress, vec[i].RegionSize, mem_of_fast_bulid);
			if (addressOffset != -1)
			{
				for (int j = 0; j < vecCodeAddr.size(); j++) {

					int int_value = (int)vec[i].BaseAddress + (int)vecCodeAddr[j];

					*(DWORD*)(int_value + 0x15) = 0x90909090;
				}
			}
		}
	}
	else
	{
		MessageBox(NULL, "EnumAllMemoryBlocks failed.", "failed", NULL);
	}

}

