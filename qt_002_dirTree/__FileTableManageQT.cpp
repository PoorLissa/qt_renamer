#include "__FileTableManageQT.h"

#include <QString>
#include <QStringList>
#include <QHeaderView> // <- required for Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>

FileTableManageQT::FileTableManageQT(QTableWidget *table) : Table(table)
{
	// ��������� ������ itemChanged ���� �� ����, ����� ����� ����������� ��������� ������ CheckBoxesChanged(bool)
	connect(Table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(itemChanged(QTableWidgetItem*)));

	connect(Table, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(itemClicked(QTableWidgetItem*)));

	init();

	// installs event filter
	Table->installEventFilter(this);
}

FileTableManageQT::~FileTableManageQT()
{
	delete Table;
}

void FileTableManageQT::init()
{
	Table->clear();

	Table->setRowCount(0);
	Table->setColumnCount(2);
	Table->wordWrap();

	Table->setHorizontalHeaderItem(0, new QTableWidgetItem("Before"));
	Table->setHorizontalHeaderItem(1, new QTableWidgetItem("After"));

	// ����������� ���������� �������� � �������
	Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	Table->setStyleSheet(
		//"QTableWidget::item { border-bottom: 2px solid #EEEEEE; background-color: transparent; color: black; margin-bottom: -1px;}"

		"QTableWidget::indicator { width: 16px; height: 16px; }"

		// ��������� �����������, ��. ���� Resource Files/<name>.qrc
		"QTableWidget::indicator:unchecked { image: url(:/qt_002_dirTree/images/checkBox-unchecked.png); }"
		"QTableWidget::indicator:checked   { image: url(:/qt_002_dirTree/images/checkBox-checked.png); }"

		"QTableWidget::selected { color: red; }"

		//"QTableView { selection-background-color: qlineargradient(x1: 0, y1: 0, x2: 0.5, y2: 0.5, stop: 0 #FF92BB, stop: 1 white); }"
		"QTableView { selection-background-color: red; }"
	);
}

void FileTableManageQT::showFiles(QStringList *list) const
{
	Table->setRowCount(0);

	for(int i = 0, Rows; i < list->length(); i++) {

		if(list->at(i).right(1) == "?") {

			// ������ � ����� � ������� �����
			QString text = list->at(i);
			text.chop(1);
			QTableWidgetItem* head = new QTableWidgetItem(text);
			//head->setFont(QFont("MS Sans Serif", 7, 11, false));
			head->setTextAlignment(Qt::AlignRight | Qt::AlignHCenter);
			//head->setTextColor(QColor(0, 0, 255));
			head->setFlags(head->flags() ^ Qt::ItemIsEditable);			// ������ ���������������
			head->setFlags(head->flags() ^ Qt::ItemIsUserCheckable);	// ������ ���������

			//QColor color( Qt::red );

			head->setBackgroundColor(QColor(255, 100, 33));
			//head->setBackgroundColor(color);
			Table->insertRow(i);
			Table->setRowHeight(i, 20);
			Table->setSpan(i, 0, 1, 2);
			Table->setItem(i, 0, head);
		}
		else {

			QTableWidgetItem* item = new QTableWidgetItem(list->at(i));
			item->setToolTip(list->at(i));
			item->setFlags(item->flags() | Qt::ItemIsUserCheckable);	// set checkable flag
			item->setCheckState(Qt::Unchecked);							// AND initialize check state
			item->setFlags(item->flags() ^ Qt::ItemIsEditable);			// ������ ���������������

			// this will give the present number of rows available.
			Rows = Table->rowCount();
			// insert the row at the bottom of the table widget
			Table->insertRow(Rows);
			Table->setRowHeight(Rows, 25);
			// After a new row is inserted we can add the table widget items as required.
			Table->setItem(Rows, 0, item);
		}
	}
}

// �������� ������ ���� ������ �������, ���������� ���������, � ������ ����� � �����
void FileTableManageQT::getCheckedFiles(QStringList& oldList, QStringList& newList)
{
	oldList.clear();
	newList.clear();
	QString path = "";

	for(int i = 0; i < Table->rowCount(); i++) {

		QTableWidgetItem* itemL = Table->item(i, 0);
		QTableWidgetItem* itemR = Table->item(i, 1);

		// ����� ���� � ����� �� ����� � �����
		if( !(itemL->flags() & Qt::ItemIsUserCheckable) )
			path = itemL->text();

		// ������� ���������� ����� � ������ (��� ��������� � ����� ������ ��������� � ������ ����� �������)
		if( itemL->checkState() ) {
			if( itemR != NULL )
				newList.push_back( path + itemR->text() );
			else
				newList.push_back( path + itemL->text() );

			oldList.push_back( path + itemL->text() );
		}
	}
}

// �������� ��������� ��� �����, ������������ � �������
void FileTableManageQT::checkAllFiles(bool State)
{
	QTableWidgetItem* item, *visibleItem = NULL;

	for(int i = 0; i < Table->rowCount(); i++) {
	
		item = Table->item(i, 0);

		if( item->flags() & Qt::ItemIsUserCheckable && !Table->isRowHidden(i) ) {
			item->setCheckState(State ? Qt::Checked : Qt::Unchecked);

			// �������� ������ ���������� ������, ������� �� ������
			if(visibleItem == NULL )
				visibleItem = item;
		}
	}

	// ������� ���, ��� �������� �� ��������, ����� ���������� ����� ��������� ���������� ��������� �������� (CheckedAll, Process, etc.)
	itemClicked(visibleItem);
}

// ������ �� ������� ������, �� ��������������� ����� �� ���������� �����
// ���� mode == true - �������� ������ �� �����, ���� Mode == false - ���������� ��� ������, �� ������� �������� �� �����
void FileTableManageQT::applyExtensionFilter(bool mode, QString mask = "*.*")
{
	QStringList Mask = mask.split(";");
	QTableWidgetItem* item, *visibleItem = NULL;

	for(int i = 0; i < Table->rowCount(); i++) {

		item = Table->item(i, 0);
	
		if( item->flags() & Qt::ItemIsUserCheckable ) {

			Table->setRowHidden(i, false);

			if( mode ) {

				bool doHide = true;

				for(int j = 0; j < Mask.length(); j++) {

					QString rMask = Mask[j].right(Mask[j].length() - Mask[j].lastIndexOf('.'));

					if( Table->item(i, 0)->text().right(rMask.length()) == rMask || rMask == ".*") {
						doHide = false;
						break;
					}
				}

				// ��� ������� ������ ������� � ��� �������, ����� ��� �� ���� �������, �� ��� ���� ����������
				// � ����� ������� ����� �� ������� ������� �������: ���� ������ ������, �� �����, ����� ��� �������� � �������� ���������
				if( doHide ) {
					Table->setRowHidden(i, true);
					item->setCheckState(Qt::Unchecked);

					item = Table->item(i, 1);
					delete item;
				}
				else {
					// �������� ������ ���������� ������, ������� �� ������
					if(visibleItem == NULL )
						visibleItem = item;
				}
			}
		}
	}

	// ������� ���, ��� �������� �� ��������, ����� ���������� ����� ��������� ���������� ��������� �������� (CheckedAll, Process, etc.)
	itemClicked(visibleItem);
}

// ���������� ����� ������ ������ �� ������ ������� �������, �������� ���������� ������
void FileTableManageQT::displayRenamedFiles(QStringList& newList, QStringList& oldList)
{
	QTableWidgetItem *item;

	bool replaceOld = &newList != &oldList;

	// ��������� � ������� � � ������ �� ���������, �.�. � ������� ���� �������������� ������ � ����� � �����
	// ������� �������� �� ������� �������� ��������, � ������ ������� �� ���������, ������� ��������� ������ ����� ������� ���������� ������� � �������
	QStringList::const_iterator constIterator;
	constIterator = newList.constBegin();

	for(int i = 0; i < Table->rowCount(); i++) {

		// �������������� ������� ��� �������� �� ������ �������, ����� ������ �� ���������� � ������� �����
		item = Table->item(i, 1);
		delete item;

		if( Table->item(i, 0)->checkState() ) {
		
			item = new QTableWidgetItem(*constIterator);
			item->setToolTip(*constIterator);
			//item->setFlags(item->flags() ^ Qt::ItemIsEditable);			// ������ ���������������

			Table->setItem(i, 1, item);

			// ���� ���������� ���� ������������� �����, �������� � ����� ����� ������ ����� �� �����
			if( replaceOld )
				Table->item(i, 0)->setText(*constIterator);

			++constIterator;
		}
	}
}

// ���� ��������� 2 ����: ������� �� ���� �� ���� ���� � ������� � �������� �� ��� ����� � �������.
// �� ��������� �������� ��������� ������ ��� ����������, ����� ��� ����� �������� ��������� ����� ���������
// ��������� �� ������������� �������/����� itemChanged, �.�. ��� ��������� � ����, ��� ����� checkAllFiles(bool) ������� itemChanged ��� ������ ������ � �������
void FileTableManageQT::itemClicked(QTableWidgetItem *Item)
{
	// ���� �� ���� �� ���� ���������� ������� (��� ��������� ������ Process)
	bool atLeastOneItemChecked = false;

	// �������� �� ��� �������� � ������� (��� ���������/������ �������� AllChecked)
	// �������� � ��������� �������� �������� ��� ��������: ���� �� �� �������, ������ ��� ����������� ���� ��� �� �����
	// �� ��������, ��� ����� ������ ���� �� ������ applyExtensionFilter, ������� ������ �������� �� NULL
	bool allItemsChecked = Item == NULL ? false : Item->checkState();

	for(int i = 0; i < Table->rowCount(); i++) {

		QTableWidgetItem* item = Table->item(i, 0);

		// �������� ������ ������� ������ � ����������
		if( item->flags() & Qt::ItemIsUserCheckable && !Table->isRowHidden(i) ) {

			// ���� ������� ���� �� ����, atLeastOneItemChecked ��� ����� ������ �� ���������
			if( Table->item(i, 0)->checkState() ) {
				atLeastOneItemChecked = true;

				// ���� allItemsChecked ��� false, ������ ��� ���� ����� ������ �� ���������
				if( !allItemsChecked )
					break;
			}
			else {
			
				// ... � ��������: ���� ���� �� ���� �� �������, ������ AllChecked �������������� false
				allItemsChecked = false;

				// � ���� ��� ���� ���� �� ���� �������, �� �������
				if( atLeastOneItemChecked )
					break;
			}
		}
	}

	emit TableCheckBoxesChanged(atLeastOneItemChecked, allItemsChecked);
}

// ������������ ������� �� ������� 'Space' � ������� - �������� ��������� ��� ���������� ������
// ������ ����� eventFilter, �.�. ������ ����������� ������� � ������� ����
// http://doc.qt.io/qt-5/qobject.html#installEventFilter
bool FileTableManageQT::eventFilter(QObject* object, QEvent* event)
{
	// if key pressed
    if (  event->type() == QEvent::KeyPress) {

        // transforms QEvent into QKeyEvent
        QKeyEvent* pKeyEvent = static_cast<QKeyEvent*>(event);

        switch( pKeyEvent->key() ) {

			case Qt::Key_Space:
				checkSelectedFiles();
			break;

            case Qt::Key_A:
				if (pKeyEvent->modifiers() & Qt::ControlModifier) {
					checkAllFiles(true);
				}
			break;

			// ���������� ��� ��������� ������� � ����������� ����������
			default:
				return QObject::eventFilter(object, event);
        }

        return true;
    }
	else {

        return QObject::eventFilter(object, event);

    }
}

// ������ Checked �� Unchecked (� ��������) ��� ���� ���������� ����� (�� ������� Space)
void FileTableManageQT::checkSelectedFiles()
{
	QTableWidgetItem* item, *visibleItem = NULL;

	for(int i = 0; i < Table->rowCount(); i++) {

		item = Table->item(i, 0);

		// �������� ������ ������� ������ � ����������
		if( item->flags() & Qt::ItemIsUserCheckable && !Table->isRowHidden(i) && item->isSelected() ) {

			if( visibleItem == NULL )
				visibleItem = item;

			if( item->checkState() )
				item->setCheckState(Qt::Unchecked);
			else
				item->setCheckState(Qt::Checked);
		}
	}

	// ������� ���, ��� �������� �� ��������, ����� ���������� ����� ��������� ���������� ��������� �������� (CheckedAll, Process, etc.)
	itemClicked(visibleItem);
}
