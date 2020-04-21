#include "__ini_file.h"

// Parse data string into words list
void iniData::parseStringList(std::list<std::wstring> &list, const std::wstring &data)
{
	std::wstring word;

	for (size_t i = 0; i < data.length(); i++)
	{
		if (data[i] == ' ')
		{
			if (word.length())
			{
				list.push_back(word);
				word.clear();
			}

			continue;
		}

		word.push_back(data[i]);
	}

	if (word.length())
		list.push_back(word);

	return;
}

// -----------------------------------------------------------------------------------------------

// Return vector with all the recorde
ini_file::dataVec& ini_file::get_ini_data()
{
	return _vec_ini;
}
// -----------------------------------------------------------------------------------------------

// Get full path to this program's .exe file
std::string* ini_file::get_exe_path()
{
	return get_file_path('e');
}
// -----------------------------------------------------------------------------------------------

// Get full path to this program's .ini file
std::string* ini_file::get_ini_path()
{
	return get_file_path('i');
}
// -----------------------------------------------------------------------------------------------

// Make file NOT hidden, system or readOnly, because in Windows 7 fstream::open fails for hidden files OR: Make file hidden
void ini_file::makeVisible(const char *file, bool visible)
{
	if (file)
	{
		DWORD attr = GetFileAttributesA(file);

		if (visible)
		{
			if (attr & FILE_ATTRIBUTE_HIDDEN || attr & FILE_ATTRIBUTE_SYSTEM || attr & FILE_ATTRIBUTE_READONLY)
			{
				DWORD removeAttr = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY;
				SetFileAttributesA(file, attr & ~removeAttr);
			}
		}
		else
		{
			SetFileAttributesA(file, attr | FILE_ATTRIBUTE_HIDDEN);
		}
	}

	return;
}
// -----------------------------------------------------------------------------------------------

// Get .exe or .ini fileName
std::string* ini_file::get_file_path(const char file)
{
	std::string *res = nullptr;

	static std::string iniFileName("");
	static std::string exeFileName("");

	if (iniFileName.empty() && exeFileName.empty())
	{
		TCHAR path[MAX_PATH];

		int len = GetModuleFileName(NULL, path, MAX_PATH);

		if (len)
		{
			exeFileName = std::string(&path[0], &path[len]);

			std::transform(exeFileName.begin(), exeFileName.end(), exeFileName.begin(), ::tolower);

			int pos = exeFileName.find_last_of('.');

			iniFileName = exeFileName.substr(0, pos + 1) + "ini";
		}
	}

	switch (file)
	{
		case 'i':
			res = &iniFileName;
			break;

		case 'e':
			res = &exeFileName;
			break;
	}

	return res;
}
// -----------------------------------------------------------------------------------------------

// Read data from the .ini-file
int ini_file::read_ini_file(bool doReadProfiles /*default=false*/)
{
	int res = -1;

	auto *vec = &_vec_ini;
	vec->clear();

	std::string *fileName = get_ini_path();

	if ((*fileName).length())
	{
		std::wfstream file;
		std::string error("");

		// Make .ini NOT hidden, system or readOnly, because in Windows 7 fstream::open fails for hidden files:
		makeVisible(fileName->c_str(), true);

		// If the file exists, we read data from it. Otherwise, we create it.
		file.open(*fileName, std::wfstream::in | std::wfstream::app);


		if (file.is_open())
		{
			//file.imbue(std::locale("rus_rus.866"));
			file.imbue(std::locale("rus_rus.1251"));

			std::wstring	line, str;
			int				lineNo = 0, pos;
			bool			isRecord = false;
			iniData			ini;

			while (std::getline(file, line) && error.empty())
			{
				lineNo++;

				int len = line.length();

				// Empty line or commentary indicate the end of the record
				if (!len || line[0] == '#')
				{
					if (isRecord)
					{
//						ini.isChecked = false;
						vec->push_back(ini);
						ini.reset();
					}

					isRecord = false;
					continue;
				}

				// { marks the beginning of the global record
				if (line[0] == '{' && doReadProfiles)
				{
					pos = line.find_first_of('}') + 1;

					std::wstring lineName = line.substr(0, pos);
					std::wstring lineData = line.substr(pos, len - pos);
/*
					// switch by string
					SWITCH()
					{
						CASE(lineName == "{Profiles}")
							prf->push_back(getMyStr(lineData));
							BREAK;

					} SWITCH_END;

					if (str_profile.empty())
						str_profile = prf->back();
*/
					continue;
				}

				// [ marks the beginning of the record
				if (line[0] == '[')
				{
					isRecord = true;

					pos = line.find_first_of(']') + 1;

					std::wstring lineName = line.substr(0, pos);
					std::wstring lineData = line.substr(pos, len - pos);

					ini.readData(lineName, lineData);
				}
			}

			res = vec->size();

			file.close();

			// Make .ini hidden
			makeVisible(fileName->c_str(), false);
		}
		else
		{
			error = "Could not open .ini file";
		}

		if (!error.empty())
		{
			vec->clear();
			_error = error;
			res = -1;
		}
	}

	return res;
}
// -----------------------------------------------------------------------------------------------
