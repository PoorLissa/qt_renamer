#include "__mp3Tiger.h"

#include <windows.h>  

mp3Tiger::mp3Tiger(const char *file, TagData& tag) : Tag(&tag)
{
	strcpy(fileName, file);
}

mp3Tiger::~mp3Tiger()
{
}

// прочитаем последние 128 байт файла и заполним структуру
int mp3Tiger::getTagData()
{
	std::ifstream mp3File;
	mp3File.open(fileName);

	if( !mp3File )
		return 1;

	// move file pointer to 128 bytes off the end of the file.
	mp3File.seekg( -(mp3Tag1Size), std::ios::end );

	if( mp3File.fail() )
		return 1;
  
	// get the data
	mp3File.read( reinterpret_cast <char*>(&(*Tag)), mp3Tag1Size );

	if( mp3File.fail() )
		return 1;

	mp3File.close();

	// убедимся, что теги прочитались: поле Tag должно содержать строку "TAG" без завершающего нуля
	char *ch = (*Tag).Tag;
	if( ch[0] != 'T' && ch[1] != 'A' && ch[2] != 'G' )
		return -1;

	return 0;
}

/*
	HFILE hMP3File;
	char szBuffer[30];
	DWORD dwNumofBytes, dwFSize, dwFKSize;


	WCHAR szMP3File[_MAX_PATH];
    const size_t cSize = strlen(fileName)+1;
    mbstowcs (szMP3File, fileName, cSize);

	hMP3File = (HFILE)CreateFile((LPCWSTR)szMP3File, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	SetFilePointer((void *)hMP3File, 0, 0, FILE_BEGIN);
	ReadFile((void *)hMP3File, &szBuffer, 4, &dwNumofBytes, NULL);

	DWORD dwMP3Info = (DWORD)(
		((szBuffer[0] & 255) << 24) |
		((szBuffer[1] & 255) << 16) |
		((szBuffer[2] & 255) << 8 ) |
		((szBuffer[3] & 255)      )
	);

	#define EXTRACT_BITS_RL(the_val, bits_start, bits_len) ((the_val >> (bits_start - 1)) & ((1 << bits_len) - 1))

	int iFrameSync		= EXTRACT_BITS_RL(dwMP3Info, 22, 11);
	int iVersionIndex	= EXTRACT_BITS_RL(dwMP3Info, 20, 2);
	int iLayerIndex		= EXTRACT_BITS_RL(dwMP3Info, 18, 2);
	int iProtectionBit	= EXTRACT_BITS_RL(dwMP3Info, 17, 1);
	int iBitrateIndex	= EXTRACT_BITS_RL(dwMP3Info, 13, 4);
	int iFrequencyIndex	= EXTRACT_BITS_RL(dwMP3Info, 11, 2);
	int iPaddingBit		= EXTRACT_BITS_RL(dwMP3Info, 10, 1);
	int iPrivateBit		= EXTRACT_BITS_RL(dwMP3Info, 9,  1);
	int iModeIndex		= EXTRACT_BITS_RL(dwMP3Info, 7,  2);
	int iModeExtIndex	= EXTRACT_BITS_RL(dwMP3Info, 5,  2);
	int iCoprightBit	= EXTRACT_BITS_RL(dwMP3Info, 4,  1);
	int iOrginalBit		= EXTRACT_BITS_RL(dwMP3Info, 3,  1);
	int iEmphasisIndex	= EXTRACT_BITS_RL(dwMP3Info, 1,  2);

	CloseHandle((void *)hMP3File);
*/