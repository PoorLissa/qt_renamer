#include "__FileRenameQT.h"
#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <clocale>

#include <time.h>

// -----------------------------------------------------------------------------------------------------------------------

// ������ ��� ������� �� ���� ������ �� ������
#define LoopFiles for(int fileNo = 0; fileNo < newFiles->length(); fileNo++)

// -----------------------------------------------------------------------------------------------------------------------

FileRenameQT::FileRenameQT(QStringList& oldfiles, QStringList& newfiles, QMap<QString, QString>* map, ini_file *iniFile)
	: oldFiles(&oldfiles), newFiles(&newfiles), Map(map), ini(iniFile)
{
	// �������� ����� ������ �� ������� ������� ������ ��� ����
	// ��������������, ����� ������ ������������ ����� ������ �������� ��������� ������������� �������
/*
	newFiles->reserve(oldFiles->length());
	for(int i = 0; i < oldFiles->length(); i++)
		newFiles->push_back( (oldFiles)->at(i).right( (oldFiles)->at(i).length() - (oldFiles)->at(i).lastIndexOf('\\') -1) );
*/

	// � ����� ������ �������� ����, �������� ������ ����� ������
	for(int i = 0; i < newFiles->length(); i++)
		newFiles->replace(i, (newFiles)->at(i).right( (newFiles)->at(i).length() - (newFiles)->at(i).lastIndexOf('\\') -1));
}

FileRenameQT::~FileRenameQT()
{
	;
}
// -----------------------------------------------------------------------------------------------------------------------

// �������� �������������� ������
int FileRenameQT::Rename()
{
	int errCount = 0;
	QMap<std::wstring, int> map_FilesExist;

	for(int i = 0; i < oldFiles->length(); i++)
	{
		QString oldFile	= oldFiles->at(i);
		QString path	= oldFile.left( oldFile.lastIndexOf('\\') + 1);
		QString newFile = path + newFiles->at(i);

		if( oldFile != newFile )
		{
			std::wstring oldName = oldFile.toStdWString();
			std::wstring newName = newFile.toStdWString();

			// If the file already exists, we need to add some unique postfix to it
			if( isFileExist(newFile) )
			{
				// But if the oldname and newname are the same when converted to lowercase, we assume the file is being renamed (lAlAlA --> Lalala)
				if( oldFile.toLower() != newFile.toLower() )
				{
					QString name = getFileName(newFile);
					QString ext = getFileExt(newFile);

					std::wstring newNameLower = name.toLower().toStdWString();

					int count = map_FilesExist.count(newNameLower);

					map_FilesExist[newNameLower] = count ? map_FilesExist[newNameLower] + 1 : 1;

					name += " __" + QString::number(map_FilesExist[newNameLower]) + ext;

					newName = name.toStdWString();
				}
			}

			// rename �� �������� ��������������� ����� � ����������, ��� ��� ������� �� _wrename
			if( !_wrename(oldName.data(), newName.data()) )
			{
				// ���� ���� ������� ��������������, ������ � Map ��� �������� ���, ������ ������ ���� � �������� �����,
				// ��� key - ����� ���, � value - �������� ��� ����� �� ��������������
				// ����� ���� �� ������ ��������, �� ����� �� key �������, � �� value - ������� ���������
				QString oldName = Map->value(oldFile);
				Map->remove(oldFile);
				//Map->insert(oldName, path + newFiles->at(i));
				Map->insert(path + newFiles->at(i), oldName);

				// ����� �������� � ������ ������ �������� ��� ����� �� �����
				oldFiles->replace(i, newFile);
			}
			else
			{
				newFiles->replace(i, oldFile.right( oldFile.length() - oldFile.lastIndexOf('\\') - 1));
				errCount++;
			}
		}
	}

	return errCount;
}
// -----------------------------------------------------------------------------------------------------------------------

// �������� �������: 0 (default) - ����� �����, 1 - ����� �����, 2 - ���������� �����
void FileRenameQT::toLowerCase(int param)
{
	QString str1, str2;

	LoopFiles
	{
		str1 = getFileName((*newFiles)[fileNo]);
		str2 = getFileExt( (*newFiles)[fileNo]);

		switch(param)
		{
			case 1:
				str1 = str1.toLower();
				break;

			case 2:
				str2 = str2.toLower();
				break;

			default:
				str1 = str1.toLower();
				str2 = str2.toLower();
		}

		(*newFiles)[fileNo] = str1 + str2;
	}

	return;
}
// -----------------------------------------------------------------------------------------------------------------------

// �������� �������: 0 (default) - ����� �����, 1 - ����� �����, 2 - ���������� �����
void FileRenameQT::toUpperCase(int param)
{
	QString str1, str2;

	LoopFiles {

		str1 = getFileName((*newFiles)[fileNo]);
		str2 = getFileExt( (*newFiles)[fileNo]);

		switch(param) {
		
			case 1:
				str1 = str1.toUpper();
			break;

			case 2:
				str2 = str2.toUpper();
			break;

			default:
				str1 = str1.toUpper();
				str2 = str2.toUpper();
		}

		(*newFiles)[fileNo] = str1 + str2;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// ������ ����� ����� ���������� � ��������� �����
// ���� excludeSmallWords == true, �� �������� ����� �� ��������� ���������� � ��������� �����
void FileRenameQT::firstSymbolOfEveryWordToUpperCase(bool excludeSmallWords, const QString Delimiter)
{
	LoopFiles
	{
		QString str = getFileName((*newFiles)[fileNo].toLower());
		QString ext = getFileExt( (*newFiles)[fileNo]);
		QString word;
		QChar   ch;
		int		oldPos = 0, newPos, count = 0;

		do {

			newPos = str.indexOf(Delimiter, oldPos);		// ���� ������ ��������� �����������
			word   = str.mid(oldPos, newPos - oldPos);		// ��������� ��������� �����

			if( (word != Delimiter) && (word != "") )
			{
				// ������� ������ ���������, ������������ � ����� (������ ����� � ������ ("01 - Let it be.mp3") ):
				// ���� ��� ����� �� ������ �������� ����, ��� ������ ����� ��� ����� ������ ���� ��������� (���� excludeSmallWords == true)
				// �������, ���� count ����� 1, �� ���� ����� �������� � ��������� �����

				// ������� ���������� ����: ������ ������� ������, ������� ���������� � �����
				ch = word.at(0);
				if( ch.isLetter() )
					count++;

				// ���� excludeSmallWords == true, �� �������� ������ ������ ��� ���� �� ������ �������� ����, �� ����������� ������ ������� ������������ �����
				if( excludeSmallWords && count > 1 && isAShortWord(word) )
				{
					oldPos = newPos + 1;
					continue;
				}

				firstSymbolToUpperCase(word);
				str.replace(oldPos, word.length(), word);
			}

			oldPos = newPos + 1;

		}
		while( newPos != std::string::npos );

		(*newFiles)[fileNo] = str + ext;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// ������� ������� �� � ����� ����� �����
void FileRenameQT::trimSpaces()
{
	LoopFiles {
		QString str = getFileName((*newFiles)[fileNo]);
		QString ext = getFileExt( (*newFiles)[fileNo]);
		str = str.trimmed();
		(*newFiles)[fileNo] = str + ext;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// ������� ��� ����������� ������� � ������ ����� �����
void FileRenameQT::deleteNonLettersFromTheStart()
{
	LoopFiles {

		QString str = (*newFiles)[fileNo];
		int pos = 0;

		while( !str[pos].isLetter() )
			pos++;

		if( pos < str.length() ) {
			(*newFiles)[fileNo] = str.right(str.length() - pos);
		}
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// �������� ����������� ����� ������� � ��-������� (��������)
void FileRenameQT::delimBetweenLettersAndNonLetters()
{
	QString delim = " ";

	LoopFiles {

		QString str = getFileName((*newFiles)[fileNo]);
		QString ext = getFileExt( (*newFiles)[fileNo]);

		int len = str.length();
		int pos = 1;

		if( len > 1 ) {

			while( pos < len ) {

				if( str[pos].isLetter() != str[pos-1].isLetter() && QChar(str[pos]) != delim && QChar(str[pos-1]) != delim ) {
					str.insert(pos, delim);
					len++;
					pos++;
				}

				pos++;
			}

			(*newFiles)[fileNo] = str + ext;
		}
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// �������� ��������� �� ������ ��������� ��������� ���������� ��� (0 - �������� ��� ���������), ������� ����� ��� ������
void FileRenameQT::replaceSubstring(const QString from, const QString to, const int count, const bool fromRight, const bool fileName)
{
	LoopFiles {

		QString str = getFileName((*newFiles)[fileNo]);
		QString ext = getFileExt( (*newFiles)[fileNo]);

		QString *Str = fileName ? &str : &ext;

		int Cnt     = count;
		int lenFrom = from.length();
		int lenTo   = to.length();
		int pos     = fromRight ? (*Str).lastIndexOf(from) : (*Str).indexOf(from);

		while( pos != -1 ) {
		
			(*Str).replace(pos, lenFrom, to);

			if( count && !--Cnt)
				break;

			// �� �������� ������ ����� ���������� ��������� � ����� ���������� ������������ ���������,
			// �.�. � ��������� ������ ��� ������, � �������, "-" �� "--" �� ����� � ����������� ����
			pos = fromRight ?
				(pos > 0 ? (*Str).lastIndexOf(from, pos-1) : -1 ) :
				(*Str).indexOf(from, pos + lenTo);
		}

		(*newFiles)[fileNo] = str + ext;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// ������� ��������� ���������� �������� � ������ ��� � ����� ������, ������� � ��������� �������
void FileRenameQT::deleteSomeSymbols(const int count, const bool left, const int pos)
{
	LoopFiles {

		QString str = getFileName((*newFiles)[fileNo]);
		QString ext = getFileExt( (*newFiles)[fileNo]);

		int Pos = left ? pos : str.length() - pos - count;

		str = str.remove(Pos, count);

		(*newFiles)[fileNo] = str + ext;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// �������� ������������������ ��������/����� �� ���� ����� ������ ("a___a" => "a_a")
void FileRenameQT::sequenceToSingle(const QString sample)
{
	LoopFiles {

		QString str = getFileName((*newFiles)[fileNo]);
		QString ext = getFileExt( (*newFiles)[fileNo]);
		QString SAMPLE = sample + sample;

		while( str.indexOf(SAMPLE) != -1 )
			str.replace(SAMPLE, sample);

		(*newFiles)[fileNo] = str + ext;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// ������� ��� �������, ���� �� ����� �������� ��������� ������
void FileRenameQT::deleteSymbolsBeforeDelim(const QString delim, bool includeDelim, bool fromLeft)
{
	LoopFiles {

		QString str = getFileName((*newFiles)[fileNo]);
		QString ext = getFileExt( (*newFiles)[fileNo]);
		int pos;

		if( str.contains(delim) && str.length() > delim.length() ) {
		
			if( fromLeft ) {

				pos = str.indexOf(delim);

				if(includeDelim)
					pos += delim.length();

				str.remove(0, pos);

			}
			else {

				pos = str.lastIndexOf(delim) + delim.length();

				if(includeDelim)
					pos -= delim.length();

				str.remove(pos, str.length() - pos);

			}

			if( str.length() > 0 )
				(*newFiles)[fileNo] = str + ext;
		}
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// ���������� ��� ��������� ����� �������� (����� ������)
void FileRenameQT::isolateCapitalLetters()
{
	LoopFiles {

		QString str = getFileName((*newFiles)[fileNo]);
		QString ext = getFileExt( (*newFiles)[fileNo]);

		int len = str.length() - 1;

		while( len >= 0 ) {
		
			if( str.at(len).isUpper() && len != 0 && str.at(len-1) != ' ' ) {
				str.insert(len, ' ');
			}

			len--;
		}



		(*newFiles)[fileNo] = str + ext;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// �������� ��������� � ��������� ������� (�����������, ������� ������)
void FileRenameQT::insertAt(const QString data, int pos, bool fromRight)
{
	LoopFiles {

		QString str = getFileName((*newFiles)[fileNo]);
		QString ext = getFileExt( (*newFiles)[fileNo]);

		if( fromRight )
			pos = str.length() - pos;

		str.insert(pos, data);

		(*newFiles)[fileNo] = str + ext;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// �������� ��������� �����/����� ��������� ��������� (���� ���)
void FileRenameQT::insertAtSubstring(const QString data, const QString substr, bool insertAfter)
{
	LoopFiles {

		QString str = getFileName((*newFiles)[fileNo]);
		QString ext = getFileExt( (*newFiles)[fileNo]);

		int pos = str.indexOf(substr);

		if( pos >= 0 ) {
		
			if( insertAfter )
				pos = pos + substr.length();

			str.insert(pos, data);
		}

		(*newFiles)[fileNo] = str + ext;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// �������� ��������� �� ����� (��������, %d?%b = �������� ����� ��������� ����� ������ � ��������� ������)
void FileRenameQT::insertByMask(const QString data, QString mask, int Count)
{
	// ����� �� ������ ���� ������ ��� ������� ��������,
	// ������ ��������� ���� �� ���� ������ '?', ������� ���������� �� ������� ��������� data,
	// ������ ��������� ���� �� ���� ������ '%', ������� ����������, ��� ��������� �� ��� ������ �������� ���������,
	// �� ������ ������������� �� ������ '%', �.�. �� ��� ������ ��������� ��������� ������
	if( mask[mask.length()-1] == '%' )
		return;

	if( !mask.contains('?') )
		mask.append('?');

	int count = 0;

	LoopFiles {

		QString str = getFileName((*newFiles)[fileNo]);
		QString ext = getFileExt( (*newFiles)[fileNo]);
		QChar ch;
		int strLength = str.length();

		// �������� �� ���� ����� ������ � ��� ������ ������� ���������, ��������� �� ��� � ������
		for(int i = 0; i < strLength; i++) {

			bool weAreGood = true;
			int  maskCount = 0;
			int  pos, insertPos;

			// �������� �� ���� ����� �����
			for(int j = 0; j < mask.length(); j++) {

				if( !weAreGood )
					break;

				switch ( mask[j].toLatin1() ) {

					case '?':
						insertPos = maskCount;
					break;

					case '%':
						j++;
						ch = mask[j];
						maskCount++;
						pos = i + maskCount - 1;

						switch (ch.toLatin1()) {

							case 'S':
							case 's':
								// ��������� ������ � ������ ����� ���� �����
							break;

							case 'D':
							case 'd':
								// ��������� ������ � ������ ������ ���� ������
								if( pos < strLength && !str[pos].isDigit() )
									weAreGood = false;
							break;

							case 'A':
							case 'a':
								// ��������� ������ � ������ ������ ���� ����� ������ � ����� ��������
								if( pos < strLength && !str[pos].isLetter() )
									weAreGood = false;
							break;

							case 'B':
							case 'b':
								// ��������� ������ � ������ ������ ���� ������ � ������ ��������
								if( pos < strLength && ( !str[pos].isLetter() || !str[pos].isLower() ) )
									weAreGood = false;
							break;

							case 'C':
							case 'c':
								// ��������� ������ � ������ ������ ���� ������ � ������� ��������
								if( pos < strLength && ( !str[pos].isLetter() || str[pos].isLower() ) )
									weAreGood = false;
							break;

							// ���� ������� ������ �� ���������, �� �������, ��� ������ ����� �������� ����, � �������
							default:
								return;
						}
					break;

					default:
						// ����������� ������� ������ ���������� � ��������� ������
						maskCount++;
						if( mask[j] != str[i + maskCount - 1] )
							weAreGood = false;
				}
			}

			if( weAreGood ) {

				// ���� ��� ������� ��������, ���� ���� ��������� ��� ��������

				if( Count )
					if( count < Count )
						count++;
					else
						break;

				str.insert(i + insertPos, data);
			}
		}

		(*newFiles)[fileNo] = str + ext;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// �������� � ��� ����� ���� ��������/������/�������/���������
void FileRenameQT::insertAttrDate(QString mask, int Button, int pos, bool fromRight)
{
	LoopFiles {

		QString str  = getFileName((*newFiles)[fileNo]);
		QString ext  = getFileExt( (*newFiles)[fileNo]);
		QString file = (*oldFiles)[fileNo];

		struct _stat statBuf;
		QString date = "";

		if( !_wstat(file.toStdWString().data(), &statBuf) ) {

			switch (Button) {
				case 1:
					date = _ctime64(&statBuf.st_ctime);		// ����� ��������
				break;
				case 2:
					date = _ctime64(&statBuf.st_mtime);		// ����� ���������
				break;
				case 3:
					date = _ctime64(&statBuf.st_atime);		// ����� �������
				break;
			}
		}

		if( fromRight )
			pos = str.length() - pos;

		str.insert(pos, parseDate(date, mask));

		(*newFiles)[fileNo] = str + ext;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// ???
void FileRenameQT::mp3Tag()
{
	LoopFiles {

		QString str  = getFileName((*newFiles)[fileNo]);
		QString ext  = getFileExt( (*newFiles)[fileNo]);
		QString file = (*oldFiles)[fileNo];

		TagData Tag = {};

		mp3Tiger Tiger(file.toStdString().data(), Tag);

		if( !Tiger.getTagData() ) {

			str.insert(0, "_");
			str.insert(0, Tag.Title); 
		}

		(*newFiles)[fileNo] = str + ext;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// �������������, ��������� ������: * ���������� �� �������� ��� �����, # ���������� �� ���������� �����, %1% ���������� �� ������ ������
void FileRenameQT::renByTemplate(const QString mask, const int startingNo)
{
	if( !mask.length() )
		return;

	LoopFiles {

		QString str  = getFileName((*newFiles)[fileNo]);
		QString ext  = getFileExt( (*newFiles)[fileNo]);
		QString Mask = mask;
		QString numIns;
		QChar   ch;
		int     offset;
		bool	Error = false;

		for(int i = 0; i < Mask.length(); i++)
		{
			offset = 0;
			ch = Mask[i];

			if( Error )
				break;

			switch (ch.toLatin1() ) {
			
				case '*':

					Mask.remove(i, 1);
                    Mask.insert(i, str);

				break;

				case '#':

					do {
						offset++;
					} while( i + offset < Mask.length() && Mask[i+offset] == '#' );

                    numIns = QString::number(fileNo + startingNo);

                    while( numIns.length() < offset )
						numIns.prepend('0');

                    Mask.remove(i, offset);
                    Mask.insert(i, numIns);

				break;

				case '%':

					do {
						offset++;
					} while( i + offset < Mask.length() && Mask[i+offset] != '%' );

					if( Mask[i] == '%' && Mask[i+offset] == '%' ) {

						int pos = Mask.mid(i+1, offset-1).toInt() - 1;

						if( pos >= 0 && pos < str.length() ) {
							Mask.remove(i, offset+1);
							Mask.insert(i, str[pos]);
							offset = 0;
						}
						else {
							Error = true;
						}
					}
					else {
						Error = true;
					}

				break;
			}

			i = i + offset;
		}

		if( !Error )
			(*newFiles)[fileNo] = Mask + ext;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// ����������� n �������� �� ������� pos1 � pos2
void FileRenameQT::moveFromPosToPos(const int n, int pos1, int pos2, bool fromEnd1, bool fromEnd2)
{
	LoopFiles
	{
		QString str = getFileName((*newFiles)[fileNo]);
		QString ext = getFileExt ((*newFiles)[fileNo]);

		int origLen = str.length();

		int p1 = fromEnd1 ? origLen - pos1 - n : pos1;
		int p2 = fromEnd2 ? origLen - pos2 - 0 : pos2;

		QString sub = str.mid(p1, n);

		str.insert(p2, sub);

		if (p2 < p1)
			p1 = p1 + n;

		str.remove(p1, n);

		(*newFiles)[fileNo] = str + ext;
	}

	return;
}
// -----------------------------------------------------------------------------------------------------------------------

// �������� ������� 2 ����� ����� �����, ����������� �����������
void FileRenameQT::swap_Left_and_Right(const QString delim)
{
	LoopFiles
	{
		QString str = getFileName((*newFiles)[fileNo]);
		QString ext = getFileExt ((*newFiles)[fileNo]);

		int pos = str.indexOf(delim);

		if (pos > 0)
		{
			QString tmp = str.mid(pos + delim.length());
			tmp += delim;
			tmp += str.mid(0, pos);
			str = tmp;
		}

		(*newFiles)[fileNo] = str + ext;
	}

	return;
}
// -----------------------------------------------------------------------------------------------------------------------

// �������� ��� ����� ��� ����������
QString FileRenameQT::getFileName(QString str)
{
	return str.left(str.lastIndexOf('.'));
}
// -----------------------------------------------------------------------------------------------------------------------

// �������� ���������� ��� ����� �����
QString FileRenameQT::getFileExt(QString str)
{
	int pos = str.lastIndexOf('.');

	if( pos >= 0 )
		return str.right(str.length() - pos);

	return "";
}
// -----------------------------------------------------------------------------------------------------------------------

// ������ ������ ������ ��������� � ������� �������
void FileRenameQT::firstSymbolToUpperCase(QString &str)
{
	str.replace(0, 1, str[0].toUpper());
}
// -----------------------------------------------------------------------------------------------------------------------

// ������ �� ����� � ������ �������� ����
bool FileRenameQT::isAShortWord(QString str)
{
	str = str.toLower();
	iniData *data = &(ini->get_ini_data().front());

	auto checkWord = [&str](std::list<std::wstring> *list)
	{
		bool res = false;

		for (auto iter = list->begin(); iter != list->end(); ++iter)
		{
			std::wstring wStr(*iter);

			if (str.toStdWString() == wStr)
			{
				res = true;
				break;
			}
		}

		return res;
	};

	auto listEn = &data->list_short_words_en;
	auto listRu = &data->list_short_words_ru;

	return checkWord(listEn) || checkWord(listRu);
}
// -----------------------------------------------------------------------------------------------------------------------

// ������ ����, ���������� �� ������� _ctime64 � ���������� ������, � ������� �� ������� ���� ���, ����� � ���� ���� ����
// ���� �������� � ���������� ���� ���� "Mon Jul 16 02:03:55 1987\n\0", ������� ��������� ����� � ��� ������ �� ��� �� ��������
QString FileRenameQT::parseDate(QString date, QString mask)
{
	QString Year, Month, Day, res = "";
	const char* Months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	int count, Len = mask.length();
	mask = mask.toLower();

	if( date != "" )
	{
		// ������� ���
		Year = date.mid(20, 4);

		// ������� �����
		Month = date.mid(4, 3);
		for(int i = 0; i < 12; i++)
			if( Months[i] == Month ) {
				Month = (i < 9 ? "0" : "") + QString::number(i+1);
				break;
			}

		// ������� ����
		Day = date.mid( 8, 2);

		// ������� ���� � ������������ � ������
		for(int i = 0; i < Len; i++)
		{
			char ch = mask.toStdWString().at(i);
			count = 1;

			switch( ch )
			{
				case 'y':
					while( ++i < Len && mask.toStdWString().at(i) == ch )
						count++;
					res += Year.right(count);

					if( i < Len )
						i--;
				break;

				case 'm':
					while( ++i < Len && mask.toStdWString().at(i) == ch )
						count++;

					if( Month[0] == '0' && count < 2 )
						res += Month[1];
					else
						res += Month.right(count);

					if( i < Len )
						i--;
				break;

				case 'd':
					while( ++i < Len && mask.toStdWString().at(i) == ch )
						count++;

					if( Day[0] == '0' && count < 2 )
						res += Day[1];
					else
						res += Day.right(count);
				
					if( i < Len )
						i--;
				break;

				default:
					res = res + ch;
			}
		}
	}

	return res;
}
// -----------------------------------------------------------------------------------------------------------------------

#include "__ScanFolder.h"

// ���������, ���������� �� ����
bool FileRenameQT::isFileExist(QString path)
{
	LPCWSTR Path = (const wchar_t*)path.utf16();
	return GetFileAttributes(Path) != INVALID_FILE_ATTRIBUTES;
}
// -----------------------------------------------------------------------------------------------------------------------
