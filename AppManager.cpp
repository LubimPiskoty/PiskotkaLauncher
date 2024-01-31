#include "AppManager.h"
#include <fstream>
#include <sstream>

using namespace psk;

vector<AppdataStruct>* AppManager::GetApplications() {
	if (!applications->size()) {
		printf("(AppManager) Applications vector empty. Loading data from appdata file.\n");
		LoadAppData();
	}
	return applications;
}

bool AppManager::AddApplication(string& executablePath) {
	// first check if it already exists in vector
	for (int i = 0; i < applications->size(); i++) {
		if (applications->at(i).executable == executablePath) {
			// The same app
			printf("(AppManager) Application already added!");
			return false;
		}
	}

	AppdataStruct app;
	app.name = fs::path(executablePath).filename().string();
	app.executable = executablePath;
	app.executablePath = executablePath;
	applications->push_back(app);

	printf("(AppManager) Adding application (%s)\n", executablePath.c_str());
	SaveAppData();
	return true;
}

bool psk::AppManager::RenameApplication(string oldName, string newName)
{
	printf("(AppManger) Renaming %s ", oldName.c_str());
	// prevent renaming to same names
	for (int i = 0; i < applications->size(); i++) {
		if (applications->at(i).name == newName) {
			printf("FAILED - Already exists\n");
			return false;
		}
	}


	// find original
	for (int i = 0; i < applications->size(); i++) {
		if (applications->at(i).name == oldName) {
			fs::path oldFilepath = applications->at(i).datafilePath;

			applications->at(i).name = newName;
			applications->at(i).datafilePath.replace_filename(newName.append(".json"));

			printf("SUCCESS - Renamed to %s\n", newName.c_str());

			DeleteFile(oldFilepath);
			SaveAppData();
			return true;
		}
	}
	printf("FAILED - No app with this name\n");
	return false;
}

bool psk::AppManager::RemoveApplication(string name)
{
	printf("(AppManger) Removing %s ", name.c_str());
	int index = -1;
	fs::path filePath;
	for (int i = 0; i < applications->size(); i++) {
		if (applications->at(i).name == name) {
			index = i;
			filePath = applications->at(i).datafilePath;
			break;
		}
	}
	if (index != -1) {
		vector<psk::AppdataStruct>::iterator iter1 = applications->begin() + index;
		applications->erase(iter1);
		printf("SUCCESS\n");
		DeleteFile(filePath);
		return true;
	}
	printf("FAILED\n");
	return false;
}

void AppManager::SaveAppData() {
	if (!fs::is_directory(appdataPath)) {
		printf("(!!) ERROR: reading directory (%s)\n", appdataPath.string().c_str());
		CreateDir();
		return;
	}
	
	// Loop over all apps and find/create .json files
	for (const auto& app : *applications){
		fs::path filePath = appdataPath.string() + "\\" + app.name + ".json";
		/*if (!fs::exists(filePath))
			CreateFile();*/

		// Save the data to its json file
		ofstream file(filePath);
		file << AppDataToJson(app).dump();
		file.close();
		printf("(AppManager) Saved - %s\n", app.name.c_str());
	}
}

void AppManager::LoadAppData() {
	if (!fs::exists(appdataPath)) {
		printf("(!!) ERROR: reading directory (%s)\n", appdataPath.string().c_str());
		CreateDir();
		// Maybe end up recursing :3
		LoadAppData();
		return;
	}

	applications->clear();
	for (const auto& entry : fs::directory_iterator(appdataPath)) {
		fs::path filePath = entry.path();


		// Skip non json files
		if (filePath.extension() != ".json")
			continue;

		// Read the file
		std::string line, contents;
		ifstream file(filePath);
		if (file)
			while (getline(file, line))
				contents.append(line);

		file.close();

		
		printf("(AppData) Found file: %s\n", filePath.filename().string().c_str());

		if (!contents.size()) {
			// No data skip parsing
			return;
		}

		// Parse it into json
		json parsedData = json::parse(contents);
		if (!parsedData.contains("name") || !parsedData.contains("executablePath"))
		{
			printf("(AppManager) ERROR: json format lacks fields (%s)", filePath.string().c_str());
		}
		// Loading the data into memory
		AppdataStruct appStruct;
		appStruct.name = parsedData["name"];
		appStruct.executable = parsedData["executablePath"];
		appStruct.executablePath = fs::path(string(parsedData["executablePath"]));
		appStruct.datafilePath = filePath;

		applications->push_back(appStruct);
	}
}


void AppManager::CreateDir()
{
	//for (const auto& entry : fs::directory_iterator(appdataPath))
	fs::create_directories(appdataPath);
	printf("(AppManager) Created new appdata dir\n");
}

void psk::AppManager::CreateFile()
{

}

bool psk::AppManager::DeleteFile(fs::path filePath)
{
	printf("(AppManger) Deleting file: %s ", filePath.string().c_str());
	if (fs::remove(filePath)) {
		printf("SUCCESS\n");
		return true;
	}
	printf("FAILED\n");
	return false;
}

json AppManager::AppDataToJson(AppdataStruct appData)
{
	json formated = json();

	formated["name"] = appData.name;
	formated["executablePath"] = appData.executablePath;

	return formated;
}

// Manage the singleton instance
AppManager* AppManager::appManager = nullptr;

AppManager* AppManager::GetInstance() {
	if (appManager == nullptr) {
		appManager = new AppManager();
	}
	return appManager;
}