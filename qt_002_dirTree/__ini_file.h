#pragma once

/*
	-- Reads data from ini-file --

	File example:
		[Path]c:\\aaa\\bbb\\ccc\\Home
		[ShortWordsEn]a in to from who where my

	Usage example:
		ini_file ini;
		ini.read_ini_file();

		auto data = ini.get_ini_data().front();
		std::string path = data.path;
		std::list<std::string> *lst_words = &data.list_short_words_en;

	To read more data from file, iniData class must:
		- declare additional public field(s)
		- make readData parse additional string data into these fields
*/

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <algorithm>

// -----------------------------------------------------------------------------------------------

#define SWITCH()	do
#define SWITCH_END	while(false)
#define CASE(x)		if(x)  {
#define BREAK		break; }
#define DEFAULT		{

// -----------------------------------------------------------------------------------------------

class iniData_Interface
{
	public:
		iniData_Interface()				{ ; }
		virtual ~iniData_Interface()	{ ; }

		virtual void reset() = 0;
		virtual void readData(const std::wstring &, const std::wstring &) = 0;
};

// -----------------------------------------------------------------------------------------------

class iniData : public iniData_Interface
{
	public:

		// public ini fields
		std::wstring				path;
		std::list<std::wstring>		list_short_words_en, list_short_words_ru;

	public:

		iniData() : iniData_Interface() { ; }

		virtual void reset() override
		{
			;
		}

		virtual void readData(const std::wstring &name, const std::wstring &data) override
		{
			// switch by string
			SWITCH()
			{
				CASE(name == L"[Path]")
					path = data;
					BREAK;

				CASE(name == L"[ShortWordsEn]")
					parseStringList(list_short_words_en, data);
					BREAK;

				CASE(name == L"[ShortWordsRu]")
					parseStringList(list_short_words_ru, data);
					BREAK;

			} SWITCH_END;

			return;
		}

	private:

		void parseStringList(std::list<std::wstring> &, const std::wstring &);
};

// -----------------------------------------------------------------------------------------------

class ini_file {

	typedef std::vector<iniData> dataVec;

	public:

		int				read_ini_file(bool = false);			// Read data from the .ini-file
		dataVec&		get_ini_data();							// Return vector with all the records

	private:

		std::string*	get_exe_path();							// Get full path to this program's .exe file
		std::string*	get_ini_path();							// Get full path to this program's .ini file
		std::string*	get_file_path(const char file);			// Get .exe or .ini fileName
		void			makeVisible(const char *, bool);		// Make file NOT hidden, system or readOnly, because in Windows 7 fstream::open fails for hidden files OR: Make file hidden

	private:

		dataVec		_vec_ini;
		std::string	_error;
};

// -----------------------------------------------------------------------------------------------

#undef SWITCH
#undef SWITCH_END
#undef CASE
#undef BREAK
#undef DEFAULT

// -----------------------------------------------------------------------------------------------
