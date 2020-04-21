#include "__FileTableManageQT.h"

#include <QString>
#include <QStringList>
#include <QHeaderView> // <- required for Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>

FileTableManageQT::FileTableManageQT(QTableWidget *table) : Table(table)
{
	// коннектим сигнал itemChanged сами на себя, чтобы иметь возможность испустить сигнал CheckBoxesChanged(bool)
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

	// настраиваем растяжение столбцов в таблице
	Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	Table->setStyleSheet(
		//"QTableWidget::item { border-bottom: 2px solid #EEEEEE; background-color: transparent; color: black; margin-bottom: -1px;}"

		"QTableWidget::indicator { width: 16px; height: 16px; }"

		// добавляем изображения, см. файл Resource Files/<name>.qrc
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

			// строка с путем к текущей папке
			QString text = list->at(i);
			text.chop(1);
			QTableWidgetItem* head = new QTableWidgetItem(text);
			//head->setFont(QFont("MS Sans Serif", 7, 11, false));
			head->setTextAlignment(Qt::AlignRight | Qt::AlignHCenter);
			//head->setTextColor(QColor(0, 0, 255));
			head->setFlags(head->flags() ^ Qt::ItemIsEditable);			// делаем нередактируемым
			head->setFlags(head->flags() ^ Qt::ItemIsUserCheckable);	// делаем нечекабле

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
			item->setFlags(item->flags() ^ Qt::ItemIsEditable);			// делаем нередактируемым

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

// получить список всех файлов таблицы, отмеченных галочками, с полным путем к файлу
void FileTableManageQT::getCheckedFiles(QStringList& oldList, QStringList& newList)
{
	oldList.clear();
	newList.clear();
	QString path = "";

	for(int i = 0; i < Table->rowCount(); i++) {

		QTableWidgetItem* itemL = Table->item(i, 0);
		QTableWidgetItem* itemR = Table->item(i, 1);

		// берем путь к файлу из строк с путем
		if( !(itemL->flags() & Qt::ItemIsUserCheckable) )
			path = itemL->text();

		// заносим отмеченные файлы в списки (при занесении в новый список приоритет у правой части таблицы)
		if( itemL->checkState() ) {
			if( itemR != NULL )
				newList.push_back( path + itemR->text() );
			else
				newList.push_back( path + itemL->text() );

			oldList.push_back( path + itemL->text() );
		}
	}
}

// отметить галочками все файлы, отображенные в таблице
void FileTableManageQT::checkAllFiles(bool State)
{
	QTableWidgetItem* item, *visibleItem = NULL;

	for(int i = 0; i < Table->rowCount(); i++) {
	
		item = Table->item(i, 0);

		if( item->flags() & Qt::ItemIsUserCheckable && !Table->isRowHidden(i) ) {
			item->setCheckState(State ? Qt::Checked : Qt::Unchecked);

			// запомним первую попавшуюся строку, которая не скрыта
			if(visibleItem == NULL )
				visibleItem = item;
		}
	}

	// сделаем вид, что кликнули по чекбоксу, чтобы приложение могло правильно отобразить зависимые контролы (CheckedAll, Process, etc.)
	itemClicked(visibleItem);
}

// скрыть из таблицы строки, не соответствующие маске по расширению файла
// если mode == true - скрываем строки по маске, если Mode == false - показываем все строки, не обращая внимания на маску
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

				// при скрытии строки снимаем у нее чекбокс, чтобы она не была скрытой, но при этом отмеченной
				// а также удаляем текст из второго столбца таблицы: если скрыли строку, не хотим, чтобы она попадала в итоговый результат
				if( doHide ) {
					Table->setRowHidden(i, true);
					item->setCheckState(Qt::Unchecked);

					item = Table->item(i, 1);
					delete item;
				}
				else {
					// запомним первую попавшуюся строку, которая не скрыта
					if(visibleItem == NULL )
						visibleItem = item;
				}
			}
		}
	}

	// сделаем вид, что кликнули по чекбоксу, чтобы приложение могло правильно отобразить зависимые контролы (CheckedAll, Process, etc.)
	itemClicked(visibleItem);
}

// показываем новый список файлов во втором столбце таблицы, напротив отмеченных файлов
void FileTableManageQT::displayRenamedFiles(QStringList& newList, QStringList& oldList)
{
	QTableWidgetItem *item;

	bool replaceOld = &newList != &oldList;

	// нумерация в таблице и в списке не совпадает, т.к. в таблице есть дополнительные строки с путем к папке
	// поэтому проходим по таблице сквозным проходом, а список обходим по итератору, который смещается только когда находим подходящий элемент в таблице
	QStringList::const_iterator constIterator;
	constIterator = newList.constBegin();

	for(int i = 0; i < Table->rowCount(); i++) {

		// предварительно очистим все элементы во втором столбце, чтобы ничего не оставалось с прошлых разов
		item = Table->item(i, 1);
		delete item;

		if( Table->item(i, 0)->checkState() ) {
		
			item = new QTableWidgetItem(*constIterator);
			item->setToolTip(*constIterator);
			//item->setFlags(item->flags() ^ Qt::ItemIsEditable);			// делаем нередактируемым

			Table->setItem(i, 1, item);

			// если фактически были переименованы файлы, заменяем в левой части старые имена на новые
			if( replaceOld )
				Table->item(i, 0)->setText(*constIterator);

			++constIterator;
		}
	}
}

// слот проверяет 2 вещи: отмечен ли хотя бы один файл в таблице и отмечены ли все файлы в таблице.
// по окончании проверки испускает сигнал для приложения, чтобы оно могло изменить состояние своих контролов
// отказался от использования сигнала/слота itemChanged, т.к. это приводило к тому, что метод checkAllFiles(bool) вызывал itemChanged для каждой строки в таблице
void FileTableManageQT::itemClicked(QTableWidgetItem *Item)
{
	// есть ли хотя бы один отмеченный чекбокс (для активации кнопки Process)
	bool atLeastOneItemChecked = false;

	// отмечены ли все чекбоксы в таблице (для установки/снятия чекбокса AllChecked)
	// начинаем с состояния текущего элемента под курсором: если он не отмечен, значит все отмеченными быть уже не могут
	// не забываем, что можем прийти сюда из метода applyExtensionFilter, поэтому делаем проверку на NULL
	bool allItemsChecked = Item == NULL ? false : Item->checkState();

	for(int i = 0; i < Table->rowCount(); i++) {

		QTableWidgetItem* item = Table->item(i, 0);

		// проверим только видимые строки с чекбоксами
		if( item->flags() & Qt::ItemIsUserCheckable && !Table->isRowHidden(i) ) {

			// если отмечен хотя бы один, atLeastOneItemChecked уже можно дальше не проверять
			if( Table->item(i, 0)->checkState() ) {
				atLeastOneItemChecked = true;

				// если allItemsChecked уже false, значит его тоже можно дальше не проверять
				if( !allItemsChecked )
					break;
			}
			else {
			
				// ... и наоборот: если хотя бы один не отмечен, значит AllChecked гарантированно false
				allItemsChecked = false;

				// и если при этом хотя бы один отмечен, то выходим
				if( atLeastOneItemChecked )
					break;
			}
		}
	}

	emit TableCheckBoxesChanged(atLeastOneItemChecked, allItemsChecked);
}

// обрабатываем нажатие на клавишу 'Space' в таблице - выделяем галочками все выделенные строки
// делаем через eventFilter, т.к. своего обработчика нажатия у таблицы нету
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

			// возвращаем все остальные нажатия в стандартный обработчик
			default:
				return QObject::eventFilter(object, event);
        }

        return true;
    }
	else {

        return QObject::eventFilter(object, event);

    }
}

// меняем Checked на Unchecked (и наоборот) для всех выделенных строк (по нажатию Space)
void FileTableManageQT::checkSelectedFiles()
{
	QTableWidgetItem* item, *visibleItem = NULL;

	for(int i = 0; i < Table->rowCount(); i++) {

		item = Table->item(i, 0);

		// проверим только видимые строки с чекбоксами
		if( item->flags() & Qt::ItemIsUserCheckable && !Table->isRowHidden(i) && item->isSelected() ) {

			if( visibleItem == NULL )
				visibleItem = item;

			if( item->checkState() )
				item->setCheckState(Qt::Unchecked);
			else
				item->setCheckState(Qt::Checked);
		}
	}

	// сделаем вид, что кликнули по чекбоксу, чтобы приложение могло правильно отобразить зависимые контролы (CheckedAll, Process, etc.)
	itemClicked(visibleItem);
}
