#ifndef QT_002_DIRTREE_H
#define QT_002_DIRTREE_H

#include <QtWidgets/QMainWindow>
#include <QListWidget>
#include <QListWidgetItem>
#include "ui_qt_002_dirtree.h"

#include "__ScanFileTreeQT.h"
#include "__FileListManage.h"
#include "__FileTableManageQT.h"
#include "__FileRenameQT.h"
#include "__bitSet.h"
#include "__ini_file.h"
#include "__helpers.h"



class qt_002_dirTree : public QMainWindow
{
	Q_OBJECT

 public:
	qt_002_dirTree(QString path, QWidget *parent = 0);
   ~qt_002_dirTree();

 private:
	Ui::qt_002_dirTreeClass ui;
	ScanFileTreeQT			*tree;			// дерево папок
	FileTableManageQT		*table;			// таблица для отображения файлов

	QStringList				filesList;		// список файлов в директории
	QMap<QString, QString>	filesMap;		// map для хранения исходных имен файлов

	ini_file				ini;
	dirTreeOptions			dtOptions;

 public slots:

	// There is method called QMetaObject::connectSlotsByName(qt_002_dirTreeClass)
	// It will automatically connect controls to predefined slots, named accordingly: checkBox --> on_checkBox_clicked(bool)
	void TreeItemSelectionChanged();

	void on_checkBoxSelAll_clicked(bool);
	void on_listWidgetExtensions_itemSelectionChanged();
	void onCheckBoxesChanged(bool, bool);

	void on_pushButtonProcess_clicked();
	void on_reReadPushButton_clicked();
	void on_pb_Undo_clicked();

	void on_cb_dirs_clicked(bool state)
	{
		dtOptions.showDirs = state;
		on_reReadPushButton_clicked();
	}

	void on_cb_files_clicked(bool state)
	{
		dtOptions.showFiles = state;
		on_reReadPushButton_clicked();
	}
};

#endif // QT_002_DIRTREE_H
