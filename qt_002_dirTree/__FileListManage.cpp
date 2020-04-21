#include "__FileListManage.h"

#include <QString>
#include <QStringList>


FileListManage::FileListManage(QListWidget *list) : List(list) {
	init();
}

FileListManage::~FileListManage() {
	delete List;
}

void FileListManage::init() {

	List->clear();

	List->setStyleSheet(
		"QListView {"
			"background: #FFFFFF;"
			"font: 12px;"
		"}"

		"QListView::item {\
			color: black;\
			background-color: transparent;\
			border-bottom: 2px solid #EEEEEE;\
			padding: 3px;\
		}"

		"QListView::item:selected {\
			background-color: transparent;\
		}"

		"QListView::item:checked {\
			background-color: red;\
		}"

		"QListView::item:hover {\
		}"
	);
}

// отображаем список файлов в виджете
void FileListManage::showFiles(QStringList *list) {

	List->clear();

	QListWidgetItem* head = new QListWidgetItem(list->at(0), List);
	head->setFont(QFont("MS Sans Serif", 7, 7, true));
	List->insertItem(0, head);

	for(int i = 1; i < list->length(); i++) {
		QListWidgetItem* item = new QListWidgetItem(list->at(i), List);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);	// set checkable flag
		item->setCheckState(Qt::Unchecked);							// AND initialize check state
		List->insertItem(0, item);									// добавляем item в список
	}
}
