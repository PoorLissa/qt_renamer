#ifndef ___FILERENAMEQT_H
#define ___FILERENAMEQT_H

#include <QStringList>
#include <QString>
#include <QObject>

#include "__mp3Tiger.h"
#include "__ini_file.h"



class FileRenameQT {

	public:
	
		FileRenameQT(QStringList&, QStringList&, QMap<QString, QString>*, ini_file *);
	   ~FileRenameQT();

		int Rename();

		void firstSymbolOfEveryWordToUpperCase(bool = true, const QString = " ");		// первый символ каждого слова в верхний регистр
		
		void toLowerCase(int = 0);														// строку к нижнему регистру
		void toUpperCase(int = 0);														// строку к верхнему регистру
		void trimSpaces();																// удалить пробелы до и после имени файла
		void deleteNonLettersFromTheStart();											// удалить небуквенные символы в начале имени файла
		void delimBetweenLettersAndNonLetters();										// вставить разделитель между буквами и не-буквами
		void replaceSubstring(const QString,											// заменить подстроку на другую подстроку
			const QString, const int = 0,
			const bool = true, const bool = true);
		void deleteSomeSymbols(const int, const bool, const int);						// удалить указанное количество символов в начале или в конце строки, начиная с указанной позиции
		void sequenceToSingle(const QString);											// заменить последовательность символов/строк на одну такую строку ("a___a" => "a_a")
		void deleteSymbolsBeforeDelim(const QString, bool includeDelim, bool = true);	// удалить все символы, пока не будет встречен указанный символ
		void isolatePunctuation();														// обособить знаки препинания: перед запятой нет пробела, но есть после, тире обрамляется пробелами с двух сторон
		void isolateCapitalLetters();													// предварить все заглавные буквы пробелом (кроме первой)
		void insertAt(const QString, int, bool);										// вставить подстроку в указанную позицию (опционально, начиная справа)
		void insertAtSubstring(const QString, const QString, bool);						// вставить подстроку перед/после указанной подстроки
		void insertByMask(const QString, QString, int = 0);								// вставить подстроку по маске (например, %d?%l = вставить новую подстроку между цифрой и маленькой буквой)
		void insertAttrDate(QString, int, int, bool);									// вставить в имя файла дату создания/записи/доступа/изменения
		void mp3Tag();																	// поименовать mp3-файл из его тегов ID3v1
		void renByTemplate(const QString, const int = 0);								// переименовать, используя шаблон: * заменяется на исходное имя файла, # заменяется на порядковый номер
		void moveFromPosToPos(const int, int, int, bool, bool);							// Переместить n символов из позиции pos1 в pos2
		void swap_Left_and_Right(const QString);										// Поменять местами 2 части имени файла, разделенные делимитером

	private:

		bool isAShortWord(QString);														// входит ли строка в список коротких слов
		void firstSymbolToUpperCase(QString &);											// первый символ строки переводим в верхний регистр
		QString getFileName(QString);													// имя файла без расширения
		QString getFileExt(QString);													// расширение файла без имени
		QString parseDate(QString, QString);											// парсим дату, полученную из функции _ctime64 и возвращаем форматированную дату, в соответствии с заданной маской
		bool isFileExist(QString);														// Проверяем, существует ли файл

	private:

		QStringList				*oldFiles;												// указатель на старый список файлов
		QStringList				*newFiles;												// указатель на новый список файлов
		QMap<QString, QString>	*Map;													// указатель на Мап, в котором хранится соответствие нового имени файла его изначальному имени для undo
		ini_file				*ini;

};

#endif // ___FILERENAMEQT_H
