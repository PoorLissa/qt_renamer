#include "__FileRenameQT.h"
#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <clocale>

#include <time.h>

// -----------------------------------------------------------------------------------------------------------------------

// макрос для прохода по всем файлам из списка
#define LoopFiles for(int fileNo = 0; fileNo < newFiles->length(); fileNo++)

// -----------------------------------------------------------------------------------------------------------------------

FileRenameQT::FileRenameQT(QStringList& oldfiles, QStringList& newfiles, QMap<QString, QString>* map, ini_file *iniFile)
	: oldFiles(&oldfiles), newFiles(&newfiles), Map(map), ini(iniFile)
{
	// заполним новый список из старого именами файлов без пути
	// соответственно, после вызова конструктора новый список заполнен исходными неизмененными файлами
/*
	newFiles->reserve(oldFiles->length());
	for(int i = 0; i < oldFiles->length(); i++)
		newFiles->push_back( (oldFiles)->at(i).right( (oldFiles)->at(i).length() - (oldFiles)->at(i).lastIndexOf('\\') -1) );
*/

	// в новом списке отрезаем пути, оставляя только имена файлов
	for(int i = 0; i < newFiles->length(); i++)
		newFiles->replace(i, (newFiles)->at(i).right( (newFiles)->at(i).length() - (newFiles)->at(i).lastIndexOf('\\') -1));
}

FileRenameQT::~FileRenameQT()
{
	;
}
// -----------------------------------------------------------------------------------------------------------------------

// реальное переименование файлов
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

			// rename не захотела переименовывать файлы с кириллицей, так что заменил на _wrename
			if( !_wrename(oldName.data(), newName.data()) )
			{
				// если файл успешно переименовался, найдем в Map его исходное имя, удалим старый ключ и создадим новый,
				// где key - новое имя, а value - исходное имя файла до переименования
				// можно было бы делать наоборот, но поиск по key быстрый, а по value - слишком медленный
				QString oldName = Map->value(oldFile);
				Map->remove(oldFile);
				//Map->insert(oldName, path + newFiles->at(i));
				Map->insert(path + newFiles->at(i), oldName);

				// также заменяем в старом списке исходное имя файла на новое
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

// понижаем регистр: 0 (default) - всего файла, 1 - имени файла, 2 - расширения файла
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

// повышаем регистр: 0 (default) - всего файла, 1 - имени файла, 2 - расширения файла
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

// каждое новое слово начинается с заглавной буквы
// если excludeSmallWords == true, то короткие слова из словарика начинаются с маленькой буквы
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

			newPos = str.indexOf(Delimiter, oldPos);		// ищем первое вхождение разделителя
			word   = str.mid(oldPos, newPos - oldPos);		// вычленили отдельное слово

			if( (word != Delimiter) && (word != "") )
			{
				// находим первую подстроку, начинающуюся с буквы (первое слово в строке ("01 - Let it be.mp3") ):
				// если это слово из списка коротких слов, его первая буква все равно должна быть заглавной (если excludeSmallWords == true)
				// поэтому, если count равен 1, не даем слову начаться с маленькой буквы

				// считаем количество слов: словом считаем строку, которая начинается с буквы
				ch = word.at(0);
				if( ch.isLetter() )
					count++;

				// если excludeSmallWords == true, не повышаем первый символ для слов из списка коротких слов, за исключением самого первого встреченного слова
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

// удалить пробелы до и после имени файла
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

// удалить все небуквенные символы в начале имени файла
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

// вставить разделитель между буквами и не-буквами (исключая)
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

// заменить подстроку на другую подстроку указанное количество раз (0 - заменить все вхождения), начиная слева или справа
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

			// не забываем начать поиск следующего вхождения с конца последнего вставленного фрагмента,
			// т.к. в противном случае при замене, к примеру, "-" на "--" мы уйдем в бесконечный цикл
			pos = fromRight ?
				(pos > 0 ? (*Str).lastIndexOf(from, pos-1) : -1 ) :
				(*Str).indexOf(from, pos + lenTo);
		}

		(*newFiles)[fileNo] = str + ext;
	}
}
// -----------------------------------------------------------------------------------------------------------------------

// удалить указанное количество символов в начале или в конце строки, начиная с указанной позиции
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

// заменить последовательность символов/строк на одну такую строку ("a___a" => "a_a")
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

// удалить все символы, пока не будет встречен указанный символ
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

// предварить все заглавные буквы пробелом (кроме первой)
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

// вставить подстроку в указанную позицию (опционально, начиная справа)
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

// вставить подстроку перед/после указанной подстроки (один раз)
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

// вставить подстроку по маске (например, %d?%b = вставить новую подстроку между цифрой и маленькой буквой)
void FileRenameQT::insertByMask(const QString data, QString mask, int Count)
{
	// маска не должна быть пустой или слишком короткой,
	// должна содержать хотя бы один символ '?', который заменяется на входную подстроку data,
	// должна содержать хотя бы один символ '%', который определяет, что следующий за ним символ является служебным,
	// не должна заканчиваться на символ '%', т.к. за ним обязан следовать служебный символ
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

		// проходим по всей длине строки и для каждой позиции проверяем, совпадает ли она с маской
		for(int i = 0; i < strLength; i++) {

			bool weAreGood = true;
			int  maskCount = 0;
			int  pos, insertPos;

			// проходим по всей длине маски
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
								// очередной символ в строке может быть любым
							break;

							case 'D':
							case 'd':
								// очередной символ в строке должен быть цифрой
								if( pos < strLength && !str[pos].isDigit() )
									weAreGood = false;
							break;

							case 'A':
							case 'a':
								// очередной символ в строке должен быть любой буквой в любом регистре
								if( pos < strLength && !str[pos].isLetter() )
									weAreGood = false;
							break;

							case 'B':
							case 'b':
								// очередной символ в строке должен быть буквой в нижнем регистре
								if( pos < strLength && ( !str[pos].isLetter() || !str[pos].isLower() ) )
									weAreGood = false;
							break;

							case 'C':
							case 'c':
								// очередной символ в строке должен быть буквой в верхнем регистре
								if( pos < strLength && ( !str[pos].isLetter() || str[pos].isLower() ) )
									weAreGood = false;
							break;

							// если текущий символ не служебный, то считаем, что вместо маски получили кашу, и выходим
							default:
								return;
						}
					break;

					default:
						// неслужебные символы просто сравниваем с символами строки
						maskCount++;
						if( mask[j] != str[i + maskCount - 1] )
							weAreGood = false;
				}
			}

			if( weAreGood ) {

				// пока что странно работает, если надо несколько раз вставить

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

// вставить в имя файла дату создания/записи/доступа/изменения
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
					date = _ctime64(&statBuf.st_ctime);		// время создания
				break;
				case 2:
					date = _ctime64(&statBuf.st_mtime);		// время изменения
				break;
				case 3:
					date = _ctime64(&statBuf.st_atime);		// время доступа
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

// переименовать, используя шаблон: * заменяется на исходное имя файла, # заменяется на порядковый номер, %1% заменяется на первый символ
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

// Переместить n символов из позиции pos1 в pos2
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

// Поменять местами 2 части имени файла, разделенные делимитером
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

// получить имя файла без расширения
QString FileRenameQT::getFileName(QString str)
{
	return str.left(str.lastIndexOf('.'));
}
// -----------------------------------------------------------------------------------------------------------------------

// получить расширение без имени файла
QString FileRenameQT::getFileExt(QString str)
{
	int pos = str.lastIndexOf('.');

	if( pos >= 0 )
		return str.right(str.length() - pos);

	return "";
}
// -----------------------------------------------------------------------------------------------------------------------

// первый символ строки переводим в верхний регистр
void FileRenameQT::firstSymbolToUpperCase(QString &str)
{
	str.replace(0, 1, str[0].toUpper());
}
// -----------------------------------------------------------------------------------------------------------------------

// входит ли слово в список коротких слов
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

// парсим дату, полученную из функции _ctime64 и возвращаем список, в котором по порядку идут год, месяц и день этой даты
// дата приходит в постоянном виде типа "Mon Jul 16 02:03:55 1987\n\0", поэтому отдельные части в ней всегда на тех же позициях
QString FileRenameQT::parseDate(QString date, QString mask)
{
	QString Year, Month, Day, res = "";
	const char* Months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	int count, Len = mask.length();
	mask = mask.toLower();

	if( date != "" )
	{
		// получим год
		Year = date.mid(20, 4);

		// получим месяц
		Month = date.mid(4, 3);
		for(int i = 0; i < 12; i++)
			if( Months[i] == Month ) {
				Month = (i < 9 ? "0" : "") + QString::number(i+1);
				break;
			}

		// получим день
		Day = date.mid( 8, 2);

		// запишем дату в соответствии с маской
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

// Проверяем, существует ли файл
bool FileRenameQT::isFileExist(QString path)
{
	LPCWSTR Path = (const wchar_t*)path.utf16();
	return GetFileAttributes(Path) != INVALID_FILE_ATTRIBUTES;
}
// -----------------------------------------------------------------------------------------------------------------------
