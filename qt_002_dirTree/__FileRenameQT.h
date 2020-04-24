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

		void firstSymbolOfEveryWordToUpperCase(bool = true, const QString = " ");		// ������ ������ ������� ����� � ������� �������
		
		void toLowerCase(int = 0);														// ������ � ������� ��������
		void toUpperCase(int = 0);														// ������ � �������� ��������
		void trimSpaces();																// ������� ������� �� � ����� ����� �����
		void deleteNonLettersFromTheStart();											// ������� ����������� ������� � ������ ����� �����
		void delimBetweenLettersAndNonLetters();										// �������� ����������� ����� ������� � ��-�������
		void replaceSubstring(const QString,											// �������� ��������� �� ������ ���������
			const QString, const int = 0,
			const bool = true, const bool = true);
		void deleteSomeSymbols(const int, const bool, const int);						// ������� ��������� ���������� �������� � ������ ��� � ����� ������, ������� � ��������� �������
		void sequenceToSingle(const QString);											// �������� ������������������ ��������/����� �� ���� ����� ������ ("a___a" => "a_a")
		void deleteSymbolsBeforeDelim(const QString, bool includeDelim, bool = true);	// ������� ��� �������, ���� �� ����� �������� ��������� ������
		void isolatePunctuation();														// ��������� ����� ����������: ����� ������� ��� �������, �� ���� �����, ���� ����������� ��������� � ���� ������
		void isolateCapitalLetters();													// ���������� ��� ��������� ����� �������� (����� ������)
		void insertAt(const QString, int, bool);										// �������� ��������� � ��������� ������� (�����������, ������� ������)
		void insertAtSubstring(const QString, const QString, bool);						// �������� ��������� �����/����� ��������� ���������
		void insertByMask(const QString, QString, int = 0);								// �������� ��������� �� ����� (��������, %d?%l = �������� ����� ��������� ����� ������ � ��������� ������)
		void insertAttrDate(QString, int, int, bool);									// �������� � ��� ����� ���� ��������/������/�������/���������
		void mp3Tag();																	// ����������� mp3-���� �� ��� ����� ID3v1
		void renByTemplate(const QString, const int = 0);								// �������������, ��������� ������: * ���������� �� �������� ��� �����, # ���������� �� ���������� �����
		void moveFromPosToPos(const int, int, int, bool, bool);							// ����������� n �������� �� ������� pos1 � pos2
		void swap_Left_and_Right(const QString);										// �������� ������� 2 ����� ����� �����, ����������� �����������

	private:

		bool isAShortWord(QString);														// ������ �� ������ � ������ �������� ����
		void firstSymbolToUpperCase(QString &);											// ������ ������ ������ ��������� � ������� �������
		QString getFileName(QString);													// ��� ����� ��� ����������
		QString getFileExt(QString);													// ���������� ����� ��� �����
		QString parseDate(QString, QString);											// ������ ����, ���������� �� ������� _ctime64 � ���������� ��������������� ����, � ������������ � �������� ������
		bool isFileExist(QString);														// ���������, ���������� �� ����

	private:

		QStringList				*oldFiles;												// ��������� �� ������ ������ ������
		QStringList				*newFiles;												// ��������� �� ����� ������ ������
		QMap<QString, QString>	*Map;													// ��������� �� ���, � ������� �������� ������������ ������ ����� ����� ��� ������������ ����� ��� undo
		ini_file				*ini;

};

#endif // ___FILERENAMEQT_H
