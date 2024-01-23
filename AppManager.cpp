#include "AppManager.h"
#include <fstream>

using namespace psk;

vector<AppdataStruct>* AppManager::GetApplications() {
	if (!applications->size()) {
		printf("(AppManager) Applications vector empty. Loading data from appdata file.\n");
		LoadAppData();
	}
	return applications;
}

bool AppManager::AddApplication(string& executablePath) {
	AppdataStruct app;
	app.name = fs::path(executablePath).filename().string();
	app.executablePath = executablePath;
	applications->push_back(app);

	printf("(AppManager) Adding application (%s)\n", executablePath.c_str());
	SaveAppData();
	return true;
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
		ifstream file(filePath);
		string contents = "";
		file >> contents;
		file.close();

		if (!contents.size()) {
			// No data skip parsing
			return;
		}

		// Parse it into json
		json parsedData = json::parse(contents);
		if (!parsedData.contains("name") || parsedData.contains("executablePath"))
		{
			printf("(AppManager) ERROR: json format lacks fields (%s)", filePath.string().c_str());
		}
		// Loading the data into memory
		AppdataStruct appStruct;
		appStruct.name = parsedData["name"];
		appStruct.executablePath = parsedData["executablePath"];

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