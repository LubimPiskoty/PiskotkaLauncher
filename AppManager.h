#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>;
#include <filesystem>

namespace fs = std::filesystem;
using json = nlohmann::json;
using namespace std;

namespace psk {

	struct AppdataStruct {
		string name;
		string executablePath;
	};

	static void PrintAppDataStruct(AppdataStruct& app) {
		printf("-------------------\n");
		printf("Name: %s\nExecutable path: %s\n", app.name.c_str(), app.executablePath.c_str());
		printf("-------------------\n");
	}


	class AppManager {
	protected:
		AppManager()
		{
			//FIX: find better way to locate apdpata roaming
			appdataPath = std::filesystem::temp_directory_path()
				.parent_path()
				.parent_path().parent_path();

			appdataPath /= "Roaming\\Piskotka\\PiskotkaLauncher\\";
		}
		static AppManager* appManager;
		fs::path appdataPath;

	public:
		// Not clonable, not assignable
		AppManager(AppManager& other) = delete;
		void operator=(const AppManager&) = delete;

		// Access singleton
		static AppManager* GetInstance();

		vector<AppdataStruct>* GetApplications();

		bool AddApplication(string& executablePath);

	private:
		vector<AppdataStruct>* applications = new vector<AppdataStruct>;

		void SaveAppData();

		void LoadAppData();

		void CreateDir();

		void CreateFile();

		json AppDataToJson(AppdataStruct appData);

	};
}
