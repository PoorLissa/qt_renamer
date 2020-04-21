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

	void firstSymbolOfEveryWordToUpperCase(bool = true,	// ������ ������ ������� ����� � ������� �������
		const QString = " ");
	void toLowerCase(int = 0);							// ������ � ������� ��������
	void toUpperCase(int = 0);							// ������ � �������� ��������
	void trimSpaces();									// ������� ������� �� � ����� ����� �����
	void deleteNonLettersFromTheStart();				// ������� ����������� ������� � ������ ����� �����
	void delimBetweenLettersAndNonLetters();			// �������� ����������� ����� ������� � ��-�������
	void replaceSubstring(const QString,				// �������� ��������� �� ������ ���������
		const QString, const int = 0,
		const bool = true, const bool = true);
	void deleteSomeSymbols(const int,					// ������� ��������� ���������� �������� � ������ ��� � ����� ������, ������� � ��������� �������
		const bool, const int);
	void sequenceToSingle(const QString);				// �������� ������������������ ��������/����� �� ���� ����� ������ ("a___a" => "a_a")
	void deleteSymbolsBeforeDelim(const QString,		// ������� ��� �������, ���� �� ����� �������� ��������� ������
		bool includeDelim, bool = true);
	void isolatePunctuation();							// ��������� ����� ����������: ����� ������� ��� �������, �� ���� �����, ���� ����������� ��������� � ���� ������
	void isolateCapitalLetters();						// ���������� ��� ��������� ����� �������� (����� ������)
	void insertAt(const QString, int, bool);			// �������� ��������� � ��������� ������� (�����������, ������� ������)
	void insertAtSubstring(const QString,				// �������� ��������� �����/����� ��������� ���������
		const QString, bool);
	void insertByMask(const QString, QString, int = 0);	// �������� ��������� �� ����� (��������, %d?%l = �������� ����� ��������� ����� ������ � ��������� ������)
	void insertAttrDate(QString, int, int, bool);		// �������� � ��� ����� ���� ��������/������/�������/���������
	void mp3Tag();										// ����������� mp3-���� �� ��� ����� ID3v1
	void renByTemplate(const QString, const int = 0);	// �������������, ��������� ������: * ���������� �� �������� ��� �����, # ���������� �� ���������� �����

 private:
	bool isAShortWord(QString);							// ������ �� ������ � ������ �������� ����
	void firstSymbolToUpperCase(QString &);				// ������ ������ ������ ��������� � ������� �������
	QString getFileName(QString);						// ��� ����� ��� ����������
	QString getFileExt(QString);						// ���������� ����� ��� �����
	QString parseDate(QString, QString);				// ������ ����, ���������� �� ������� _ctime64 � ���������� ��������������� ����, � ������������ � �������� ������

 private:
	QStringList				*oldFiles;					// ��������� �� ������ ������ ������
	QStringList				*newFiles;					// ��������� �� ����� ������ ������
	QMap<QString, QString>	*Map;						// ��������� �� ���, � ������� �������� ������������ ������ ����� ����� ��� ������������ ����� ��� undo
};

#endif // ___FILERENAMEQT_H
