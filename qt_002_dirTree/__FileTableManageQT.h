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

	// �������� ��� �����, ����������� � ���������� ����������� ������
	void showFiles(QStringList*) const;
	// �������� ������ ���������� ������ �� ������� ������� �������
	void getCheckedFiles(QStringList&, QStringList&);
	// �������� ��� �������� � �������
	void checkAllFiles(bool);
	// ������ �� ������� ������, �� ��������������� ����� �� ���������� ����� � ������ ������� �������
	void applyExtensionFilter(bool, QString);
	// ���������� �� ������ ������� ������� ����� ������ ������
	void displayRenamedFiles(QStringList&, QStringList&);

 private:
	 void init();
	 void checkSelectedFiles();

 private:
	QTableWidget *Table;

 protected:
	bool eventFilter(QObject*, QEvent*);

 private slots:
	// ����� ��� ����� ���-�� �������� � ����� ������ �������, �������� ����������� ������ � �������� � ���� ����
	void itemClicked(QTableWidgetItem*);

 signals:
	// �������� ������, ����� �������� �������� ����������, ���� �� ���������� �������� � �������
	void TableCheckBoxesChanged(bool, bool);
};

#endif // ___FILETABLEMANAGEQT_H
