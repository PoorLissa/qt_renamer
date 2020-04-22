#pragma once
#ifndef __HELPERS_H
#define __HELPERS_H

#include <string>

struct dirTreeOptions
{
	dirTreeOptions() : showDirs(false)
	{
		;
	}

	bool showDirs;
};

void convert_from_MultiByte_to_WideChar(std::wstring &, const char *);

#endif
