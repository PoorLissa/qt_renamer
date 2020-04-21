#ifndef ___FILETABLEMANAGEQT_H
#define ___FILETABLEMANAGEQT_H

#include <QObject>
#include <QTableWidget>
#include <QTableWidgetItem>

class FileTableManageQT : public QObject
{
	Q_OBJECT

public:
	FileTableManageQT(QTableWidget*);
	~FileTableManageQT();

	// показать все файлы, находящиеся в отмеченных директориях дерева
	void showFiles(QStringList*) const;
	// получить список отмеченных файлов из первого столбца таблицы
	void getCheckedFiles(QStringList&, QStringList&);
	// отметить все чекбоксы в таблице
	void checkAllFiles(bool);
	// скрыть из таблицы строки, не соответствующие маске по расширению файла в первом столбце таблицы
	void applyExtensionFilter(bool, QString);
	// отобразить во втором столбце таблицы новый список файлов
	void displayRenamedFiles(QStringList&, QStringList&);

 private:
	 void init();
	 void checkSelectedFiles();

 private:
	QTableWidget *Table;

 protected:
	bool eventFilter(QObject*, QEvent*);

 private slots:
	// когда при клике что-то меняется в любой строке таблицы, источаем одноименный сигнал и приходим в этот слот
	void itemClicked(QTableWidgetItem*);

 signals:
	// источаем сигнал, чтобы сообщить внешнему приложению, есть ли отмеченные чекбоксы в таблице
	void TableCheckBoxesChanged(bool, bool);
};

#endif // ___FILETABLEMANAGEQT_H
