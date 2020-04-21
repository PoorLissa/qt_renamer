#pragma once

#ifndef ___MP3TIGER_H
#define ___MP3TIGER_H

#include <fstream>

struct TagData {
	char  Tag     [ 3];
	char  Title   [30];
	char  Artist  [30];
	char  Album   [30];
	char  Year    [ 4];
	char  Comment [30];
	int   Genre;
};
  
class mp3Tiger
{
 public:
	mp3Tiger(const char*, TagData&);
	~mp3Tiger();
	int getTagData();

 private:
	const int mp3Tag1Size = 128;
	TagData	 *Tag;
	char	  fileName[1024];
};

#endif