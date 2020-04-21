#ifndef ___FILERENAMEQT_H
#define ___FILERENAMEQT_H

#include <QStringList>
#include <QString>
#include <QObject>

#include "__mp3Tiger.h"


class FileRenameQT {
 public:
	FileRenameQT(QStringList&, QStringList&, QMap<QString, QString>*);
	~FileRenameQT();

	int Rename();

	void firstSymbolOfEveryWordToUpperCase(bool = true,	// первый символ каждого слова в верхний регистр
		const QString = " ");
	void toLowerCase(int = 0);							// строку к нижнему регистру
	void toUpperCase(int = 0);							// строку к верхнему регистру
	void trimSpaces();									// удалить пробелы до и после имени файла
	void deleteNonLettersFromTheStart();				// удалить небуквенные символы в начале имени файла
	void delimBetweenLettersAndNonLetters();			// вставить разделитель между буквами и не-буквами
	void replaceSubstring(const QString,				// заменить подстроку на другую подстроку
		const QString, const int = 0,
		const bool = true, const bool = true);
	void deleteSomeSymbols(const int,					// удалить указанное количество символов в начале или в конце строки, начиная с указанной позиции
		const bool, const int);
	void sequenceToSingle(const QString);				// заменить последовательность символов/строк на одну такую строку ("a___a" => "a_a")
	void deleteSymbolsBeforeDelim(const QString,		// удалить все символы, пока не будет встречен указанный символ
		bool includeDelim, bool = true);
	void isolatePunctuation();							// обособить знаки препинания: перед запятой нет пробела, но есть после, тире обрамляется пробелами с двух сторон
	void isolateCapitalLetters();						// предварить все заглавные буквы пробелом (кроме первой)
	void insertAt(const QString, int, bool);			// вставить подстроку в указанную позицию (опционально, начиная справа)
	void insertAtSubstring(const QString,				// вставить подстроку перед/после указанной подстроки
		const QString, bool);
	void insertByMask(const QString, QString, int = 0);	// вставить подстроку по маске (например, %d?%l = вставить новую подстроку между цифрой и маленькой буквой)
	void insertAttrDate(QString, int, int, bool);		// вставить в имя файла дату создания/записи/доступа/изменения
	void mp3Tag();										// поименовать mp3-файл из его тегов ID3v1
	void renByTemplate(const QString, const int = 0);	// переименовать, используя шаблон: * заменяется на исходное имя файла, # заменяется на порядковый номер

 private:
	bool isAShortWord(QString);							// входит ли строка в список коротких слов
	void firstSymbolToUpperCase(QString &);				// первый символ строки переводим в верхний регистр
	QString getFileName(QString);						// имя файла без расширения
	QString getFileExt(QString);						// расширение файла без имени
	QString parseDate(QString, QString);				// парсим дату, полученную из функции _ctime64 и возвращаем форматированную дату, в соответствии с заданной маской

 private:
	QStringList				*oldFiles;					// указатель на старый список файлов
	QStringList				*newFiles;					// указатель на новый список файлов
	QMap<QString, QString>	*Map;						// указатель на Мап, в котором хранится соответствие нового имени файла его изначальному имени для undo
};

#endif // ___FILERENAMEQT_H
