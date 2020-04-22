#pragma once
#ifndef ___SCANFILETREEQT_H
#define ___SCANFILETREEQT_H

#include <QObject>

#include <fstream>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "__ScanFolder.h"
#include "__helpers.h"

typedef QMap<QString, QString>	qMapSS;
typedef QMap<QString, char>		qMapSC;



class ScanFileTreeQT : public QObject {
	Q_OBJECT

 public:
	ScanFileTreeQT(QTreeWidget *TreeViewObj, bool sMode = false);
	~ScanFileTreeQT();

 public:
	// ��������� ��������� ����� ����������� ����; ������������ �� ������� itemExpanded(QTreeWidgetItem*)
	void NodeExpand(QTreeWidgetItem*);

	QString getCurrNodeName();

	// �������� ������ ������ (QStringList), ����������� ��� ���������� �����������/������������
	void getFileList(QStringList&, qMapSS&, qMapSC&, const dirTreeOptions &);

	// ����������� ������ ��� ���������� ����� / �����
	void reReadSelectedDir();

	// sort function; use it with the 'TreeView1Compare()' event
	//int NodeCompare(TTreeNode*, TTreeNode*);

	// adds all the found files to suplied vector; use it with the 'TreeView1Change()' event
	//void filesToVector(TTreeNode*, vector<AnsiString>&);

	// ��������� ����� ��� ������ ������ �� ���
	//void setMask(const char*);

	// flips IncludeSub flag from false to true and vice versa
	//void SubDirFlip();

	// ������� � ������ ��������� �������������� ����� �� ��������� ���� (��� ����������� � ��� �������, ���� ����� ����� ��� �� ����������)
	// �� ����� �������� ���� ����� �� ������������, �.�. �� ������� ���������� ����, ������� �������� ����� ������� ������ ScanFileTreeQT, ������� ��� �� �����, �.�. ����������� �� ��������� �� �����
	void setLastDirectory(QString path = "");

 private:
	void init();
	void ScanHardDrives();
	void BuildTree(QTreeWidgetItem*, const QString, const QString);
	bool pathIsDirectory(QString, int &);
	void findFiles(QString, QStringList &, qMapSS &, qMapSC &, const dirTreeOptions &dtOpt);

	// �������� ������ ���� � ��������, ����������� �����
	QString getPath(QTreeWidgetItem*);

	// ��������� � ��������� �� ����� ��������� �������������� ���� � ����� ������
	void	lastPathToFile();
	QString lastPathFromFile();

	// ������� � ������ ����� �� ��������� ���� (��� ����������� � ��� �������, ���� ����� ����� ��� �� ����������)
	void setDirectory(QString);

 public slots:
	void itemExpanded(QTreeWidgetItem*);
	void itemCollapsed(QTreeWidgetItem*);

 private:
	QTreeWidget* Tree;
	bool		 FastSearch;
	QString		 Mask;
	QIcon*		 normalDirIcon;
	QIcon*		 hiddenDirIcon;
	QIcon*		 driveIcon;
};

#endif // ___SCANFILETREEQT_H
