#ifndef ___FILELISTMANAGE_H
#define ___FILELISTMANAGE_H

#include <QObject>
#include <QListWidget>
#include <QListWidgetItem>

class FileListManage : public QObject {
	Q_OBJECT

 public:
	FileListManage(QListWidget *list);
	~FileListManage();

	void showFiles(QStringList*);

 private:
	 void init();

 private:
	QListWidget *List;
};

#endif // ___FILELISTMANAGE_H
