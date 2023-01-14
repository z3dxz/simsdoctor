#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <windowsx.h>
#include <filesystem>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <future>
#include <string>
#include <string_view>
#include <xtree>
#include <vector>
#include <shlobj.h>

std::vector<std::wstring> names;

int CheckFileExists(const std::wstring& url) {
	const TCHAR* file = url.c_str();
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(file, &FindFileData);
	int found = handle != INVALID_HANDLE_VALUE;
	if (found)
	{
		//FindClose(&handle); this will crash
		FindClose(handle);
	}
	return found;
}

int CheckFolderExists(const std::wstring& url) {
	const TCHAR* file = url.c_str();
	return GetFileAttributes(file) < 1;
}

std::wstring FindGame()
{

	std::wstring root = L"C:";

	std::wstring url = root + std::wstring(L"\\Program Files\\EA Games\\The Sims 4\\Game\\Bin\\TS4_x64.exe");

	if (CheckFileExists(url)) {
		return url;
	}

	url = root + std::wstring(L"\\Program Files (x86)\\EA Games\\The Sims 4\\Game\\Bin\\TS4_x64.exe");

	if (CheckFileExists(url)) {
		return url;
	}

	url = root + std::wstring(L"\\Program Files\\Origin Games\\The Sims 4\\Game\\Bin\\TS4_x64.exe");

	if (CheckFileExists(url)) {
		return url;
	}

	url = root + std::wstring(L"\\Program Files (x86)\\Origin Games\\The Sims 4\\Game\\Bin\\TS4_x64.exe");

	if (CheckFileExists(url)) {
		return url;
	}

	//

	url = root + std::wstring(L"\\Program Files\\EA Games\\The Sims 4\\Game\\Bin\\TS4_x86.exe");

	if (CheckFileExists(url)) {
		return url;
	}

	url = root + std::wstring(L"\\Program Files (x86)\\EA Games\\The Sims 4\\Game\\Bin\\TS4_x86.exe");

	if (CheckFileExists(url)) {
		return url;
	}

	url = root + std::wstring(L"\\Program Files\\Origin Games\\The Sims 4\\Game\\Bin\\TS4_x86.exe");

	if (CheckFileExists(url)) {
		return url;
	}

	url = root + std::wstring(L"\\Program Files (x86)\\Origin Games\\The Sims 4\\Game\\Bin\\TS4_x86.exe");

	if (CheckFileExists(url)) {
		return url;
	}


	return std::wstring(L"Invalid");
}

struct Division {
	uint64_t start;
	uint64_t end;
};

struct DivStatus {
	bool success;
	bool a;
	bool b;
	bool none;
};

enum {
	TypeStatusErr,
	TypeStatusNo,
	TypeStatusYes,
	TypeStatusDone
};

#define valid 0
#define invalid 1
/*

enum validation {
	valid = 0,
	invalid = 1
};
*/

struct DivisionGroup {
	Division first;
	Division second;
	int validation = valid;
};

typedef int TestStatus;

//DivisionGroup invalid_div = { {0,0}, {0,0}, invalid };

DivisionGroup SplitGroup(const Division& input) {

	int realitive_end = input.end - input.start;

	if (realitive_end < 1) {
		DivisionGroup invaliddv = { {input.start,0}, {0,0}, invalid };

		return invaliddv;
	}

	int size = realitive_end + 1;

	int realitive_group_end = realitive_end / 2;


	int realitive_group_two_start = realitive_group_end + 1;



	Division first = { input.start, realitive_group_end + input.start};
	Division second = { input.start + realitive_group_two_start, input.end};

	return {first,second};
}

// once the broken file is found, it can be moved to a broken folder and removed the the list

// in the process, working files can be logged

// when the broken file is found, it can move the working files to a working folder and removed form the list

// if the whole array is in the working folder then your done! yay! test again to verify again


// INITALIZE STRINGS FOR MATHS + MODS FOLDERS
std::wstring mods = L""; // mods folder
std::wstring path = L""; // content/waiting room folder
std::wstring wfolder = L""; // working mods folder
std::wstring nfolder = L""; // bad mods folder 

void MoveBackMods() {
	// exits the test folder and goes back to the waiting room
	// does this for ALL mods

	// NEEDS OPTIMIZATION (loop over things in test, not the whole mods index)

	int counter = 0;
	for (std::wstring& element : names) {
		counter++;
		std::wstring from = mods + std::wstring(L"\\") + element;

		std::wstring to = path + std::wstring(L"\\") + element;

		MoveFile(from.c_str(), to.c_str());

		//std::wcout << L"moved " << std::to_wstring(counter) << L"/" << std::to_wstring(names.size()) << '\n';
	}
}

void MoveTestMods(const Division& participants) {
	// moves from the waiting room to the test (mods) folder

	// BROKEN (no longer broken)

	int divison_size = (participants.end - participants.start) + 1;

	for (int i = participants.start; i < participants.end+1; i++) {

		std::wstring folder = path + std::wstring(L"\\") + names[i];
		std::wstring to_test = mods + std::wstring(L"\\") + names[i];

		MoveFile(folder.c_str(), to_test.c_str());
	}
}

TestStatus test(const Division& participants) {

	std::cout << "\n\nTESTING!\n\nMoving mods to main...\n";

	MoveBackMods();

	MoveTestMods(participants);

	std::cout << "\nTest\n";

	std::wstring game = FindGame();

	game = L"\"" + game + L"\"";

	if (game == L"Invalid") {
		std::cout << "ERROR: Can't find sims game\n";
		return 0;
	}

	std::wcout << L"Game: " << game;

	std::wstring test_msg = L"The game is ready to test. Testing division from cc " + std::to_wstring((int)participants.start) + L" to " + std::to_wstring((int)participants.end) + L". Click OK when ready.\n";
	
		
	MessageBox(0, test_msg.c_str(), L"Ready to test", MB_OK);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	bool working = false;

	bool processS = CreateProcess(NULL,(LPWSTR)game.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (!processS) {
		MessageBox(0, L"Sims 4 Failed To Open. Will mark this as not working cc.", L"Oh No!", MB_OK);
		std::cout << "Sims 4 failed to open\n";
		return 1;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	int yes = MessageBox(0, L"Did your game work? Answer correctly or the algorithm will be incorrect.", L"Did it work?", MB_YESNO);

	if (yes == IDYES) {
		working = true;
	}

	std::cout << "Done testing\n\n";

	return (working)+1;
}

DivStatus TestGroup(const DivisionGroup& div) {
	DivStatus ds;

	int test1 = test(div.first);
	int test2 = test(div.second);

	ds.a = test1 == 2;
	ds.b = test2 == 2;

	int st_a = !(test1==0);
	int st_b = !(test2 == 0);

	ds.success = st_a && st_b;

	ds.none = !(ds.a) && !(ds.b);

	return ds;
}

void PrintDivG(const DivisionGroup& a) {

	std::wcout << L"Division: From " << std::to_wstring(a.first.start) << L" to " << std::to_wstring(a.first.end) << L" and from " << std::to_wstring(a.second.start) << L" to " << std::to_wstring(a.second.end) << L"\n";

}


// move all mods that are working into the working buffer
// once found a broken file, move all from the working buffer to a working folder and remove it from the "names" index

void MoveBrokenFile(std::wstring file) {

	std::wstring from = path + std::wstring(L"\\") + file;

	std::wstring to = nfolder + std::wstring(L"\\") + file;

	// moves to broken folder
	MoveFile(from.c_str(), to.c_str());

	std::vector<std::wstring>::iterator itr = std::find(names.begin(), names.end(), file);
	if (itr != names.end()) names.erase(itr);
}

void MoveFoldersToWorking(const std::vector<std::wstring>& workingBuffer) {

	// moves the files that are marked as "working" to a working directory

	for (const std::wstring& i : workingBuffer) {

		std::wstring from = path + std::wstring(L"\\") + i;

		std::wstring to = wfolder + std::wstring(L"\\") + i;

		// moves to working folder
		MoveFile(from.c_str(), to.c_str());

	}

	for (const std::wstring& i : workingBuffer) {
		// remove from the index

		std::vector<std::wstring>::iterator itr = std::find(names.begin(), names.end(), i);
		if (itr != names.end()) names.erase(itr);
		//names.erase(names.begin() + i);

	}


}

std::wstring FindBrokenFile(int iter) {

	std::vector<std::wstring> workingBuffer;

	std::wcout << "finding broken file: iter " << std::to_wstring(iter) << "\n";

	int running = true;
	int interval = 0;

	DivisionGroup div = SplitGroup({ 0, names.size() - 1 });;

	PrintDivG(div);

	while (running) {
		interval++;
		if (interval > 50) {
			std::wcout << "Timeout Error\n";
			running = false;
			
		}

		if (div.validation == invalid) {
			std::cout << "invalid division = one or less file's left! broken file found!\n";

			std::wcout << L"Working entities: ";
			for (const std::wstring& i : workingBuffer)
				std::wcout << i << ' ';
			std::wcout << L"\n";

			// move back to content
			MoveBackMods();


			std::wstring brokenmod_index = names[div.first.start];

			// move folder group to working
			MoveFoldersToWorking(workingBuffer);

			// INDEXING SYSTEM DEAD! 

			return brokenmod_index;
		}

		DivStatus status = TestGroup(div);
		

		if (status.none) {
			// none working
			if (rand() % 2 == 0) {
				div = SplitGroup(div.second);

				continue;
			}
			else {
				div = SplitGroup(div.first);

				continue;
			}
		}

		if (status.a && status.b) {
			// both working
			// 
			// 
			// move to working
			
			for (int i = div.first.start; i < div.first.end + 1; i++) {
				workingBuffer.push_back(names[i]);
			}

			for (int i = div.second.start; i < div.second.end + 1; i++) {
				workingBuffer.push_back(names[i]);
			}

			continue;
		}

		if (status.a) {
			// a works
			// move to working

			for (int i = div.first.start; i < div.first.end + 1; i++) {
				workingBuffer.push_back(names[i]);
			}

			// b is broken
			// divide b
			div = SplitGroup(div.second);
		}
		else {
			// b works
			// move to working

			for (int i = div.second.start; i < div.second.end + 1; i++) {
				workingBuffer.push_back(names[i]);
			}

			// a is broken
			// divide a
			div = SplitGroup(div.first);
		}


	}

	std::cout << "Something died. OOoooops! :( sad face emoji. Can't find broken file\n";

	return names[div.first.start];
}

int setupfolders() {

	LPWSTR my_documents = NULL;

	SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &my_documents);
	std::wstring documents = std::wstring(my_documents);


	LPWSTR my_desktop = NULL;
	SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &my_desktop);
	std::wstring desktop = std::wstring(my_desktop);

	std::wcout << L"Documents folder: " << documents << "\n";
	std::wcout << L"Desktop folder: " << desktop << "\n";

	mods = documents + L"\\Electronic Arts\\The Sims 4\\Mods";

	if (CheckFolderExists(mods)) {
		std::wcout << L"no mods folder :( ";
		std::wcout << mods;
		std::wcout << L"\n";
		return 0;
	}

	std::wcout << L"Mods Folder Found: " << mods << L'\n';

	path = desktop + L"\\waiting_room";

	wfolder = desktop + L"\\working_room";

	nfolder = desktop + L"\\bad_mods";

	std::wcout << L"Temp Content Dir: " << path << L'\n';

	std::wcout << L'\n';

	std::wcout << L"Creating Directory's...\n";

	CreateDirectory(path.c_str(), 0);
	CreateDirectory(wfolder.c_str(), 0);
	CreateDirectory(nfolder.c_str(), 0);

	std::wcout << L'\n';

	std::wcout << L"Created Directory's!\n";

	std::wcout << L'\n';


	std::wstring mods_f = mods + L"\\*";


	std::wcout << L"Allocating mod buffer...\n";


	std::wcout << L"Indexing mod buffer...\n";
	{

		WIN32_FIND_DATA data;
		HANDLE hFind = FindFirstFile(mods_f.c_str(), &data);      // DIRECTORY

		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (!(std::wstring(data.cFileName).starts_with(L".")) && !(std::wstring(data.cFileName).starts_with(L"desktop.ini")) && !(std::wstring(data.cFileName).starts_with(L"Resource.cfg"))) {

					//std::wcout << data.cFileName << "\n";
					names.push_back(std::wstring(data.cFileName));
				}
			} while (FindNextFile(hFind, &data));
			FindClose(hFind);
		}
	}

	std::wcout << L"Mod Buffer Indexed!...\n";

	return 1;
}

int recursive_iterator() {



	std::wcout << L"\n\n" << std::to_wstring(names.size()) << L" Mods Found!\n";

	if (names.size() <= 0) {
		std::cout << "No mods. Bye!\n";
		return 0;
	}

	// copy files

	std::wcout << L"Press enter to continue\n";
	std::cin.get();


	std::wcout << L"Getting ready\n";

	std::wcout << L"Folder size: " << std::to_wstring(names.size()) << L"\n";

	std::wstring file = FindBrokenFile(0);

	MoveBrokenFile(file);

	MessageBox(0, file.c_str(), L"Found broken file", MB_OK);

	

	return 1;
}

int main() {

	int s = setupfolders();
	if (!s) {
		return 0;
	}

	while (true) {
		if (!recursive_iterator()) {
			std::wcout << "DONE! WOOHOO!\n";
			std::cin.get();
		}
	}
}