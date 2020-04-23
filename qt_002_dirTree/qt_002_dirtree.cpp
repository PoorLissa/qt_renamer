#include "qt_002_dirtree.h"

#include <QMessageBox>
#include <QStringListModel>

// opt(1) => ui.cb1->checkState()
#define opt(num) ui.cb##num->checkState()



qt_002_dirTree::qt_002_dirTree(QString path, QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(TreeItemSelectionChanged()));

	table = new FileTableManageQT(ui.tableWidget);

	tree  = new ScanFileTreeQT(ui.treeWidget, false);

	// Set current directory: from file OR from command line parameter
	tree->setLastDirectory(path);

	// Read ini file
	ini.read_ini_file();

	this->setStyleSheet(
		"QListView { background: #FFFFFF;" "font: 13px;" "}"
		"QListView::item { color: black; border-bottom: 1px solid #EEEEEE; }"
		"QListView::item:selected { background-color: red; color: white; }"
		"QListView::item:hover { background-color: #EEFFEE; }"
		"QListView::item:selected::hover { background-color: red; color: darkred; }"

		"QPushButton {\
			border: 1px solid #8f8f91;\
			border-radius: 3px;\
			background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde);\
			min-width: 80px;\
		}"

		// QSpinBox
		"QSpinBox { border: 1px groove grey; border-radius: 2px; }"
		// большие кнопки, обе справа
		//"QSpinBox::up-button   { subcontrol-position: right; width:  18px; height: 18px; right: 1px;  }"
		//"QSpinBox::down-button { subcontrol-position: right; height: 18px; width:  18px; right: 20px; }"

		// QLineEdit
		//"QLineEdit { border: 1px groove grey; border-radius: 2px; }"
	);

	ui.listWidgetExtensions->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

	// для кнопки Process - енаблим и дисаблим ее в зависимости от того, есть ли отмеченные чекбоксы в таблице
	ui.pushButtonProcess->setEnabled(false);
	ui.checkBoxSelAll->setChecked(false);
	connect(table, SIGNAL(TableCheckBoxesChanged(bool, bool)), this, SLOT(onCheckBoxesChanged(bool, bool)));

	// минимальные размеры окна
	ui.centralWidget->setMinimumWidth(980);
	ui.centralWidget->setMinimumHeight(600);

	// чекбокс на кнопке Process
	ui.cb0->setParent(ui.pushButtonProcess);
	ui.cb0->setGeometry(QRect(5, 5, 20, 20));

	// проставим плейсхолдеры для полей
	ui.lineEdit_07->setPlaceholderText("String to insert");
	ui.lineEdit_09->setPlaceholderText("Any Substring");
	ui.lineEdit_10->setPlaceholderText("Mask to match");
	ui.lineEdit_10->setToolTip("Your text will be inserted at the position\nthat matches with the given mask, where\n"\
		"%d or %D = any digit\n"\
		"%b or %B = any small (block) letter\n"\
		"%c or %C = any big (capital) letter\n"\
		"%a or %A = just any letter\n"\
		"%s or %S = any symbol\n"\
		"? = your text to insert\n\n"\
		"The whole mask might look like this:\n"\
		"%d%d - %a%s_?"
	);
	ui.lineEdit_11->setPlaceholderText("Template [### - *]");
	ui.lineEdit_11->setToolTip("The file will be renamed using your template, where\n"\
		"* = initial file name\n"\
		"### = generated number starting with the number in the SpinBox field, where\n"\
		"\t # = 1, 2, 3...\n"\
		"\t ## = 01, 02, 03...\n"\
		"\t ### = 001, 002, 003...\n"\
		"%Pos% = symbol from the initial file name, where\n"\
		"\tPos is a position number in the file name, starting at 1\n"
	);
}

qt_002_dirTree::~qt_002_dirTree()
{
	delete tree;
	delete table;
}
// --------------------------------------------------------------------------------------------

// переименовываем выбранные файлы
void qt_002_dirTree::on_pushButtonProcess_clicked()
{
	// проверим, включена ли опция физического переименования
	bool doRename = opt(0);


	// получим список полных исходных имен файлов с путем, отмеченных в левой части таблицы
	QStringList oldFilesList, newFilesList;
	table->getCheckedFiles(oldFilesList, newFilesList);


	if( oldFilesList.length() > 0 )
	{
		// передадим списки в класс Renamer, который нам и сделает переименование, и затем применим к файлам все отмеченные преобразования
		// в filesMap сохраним исходные имена файлов (будут храниться, пока мы не сменим директорию или принудительно не перечитаем содержимое директорий)
		FileRenameQT Renamer(oldFilesList, newFilesList, &filesMap, &ini);


		if( opt(1) )
		{
			opt(2) ? 
				Renamer.firstSymbolOfEveryWordToUpperCase() :
				Renamer.firstSymbolOfEveryWordToUpperCase(false);
		}

		// extension to upper
		if( opt(3) ) {
			Renamer.toUpperCase(2);
		}

		// extension to lower
		if( opt(4) ) {
			Renamer.toLowerCase(2);
		}

		// file name to upper
		if( opt(5) ) {
			Renamer.toUpperCase(1);
		}

		// file name to lower
		if( opt(6) ) {
			Renamer.toLowerCase(1);
		}

		// trim spaces
		if( opt(7) ) {
			Renamer.trimSpaces();
		}

		// delete non-letter symbols from the beginning of the file
		if( opt(8) ) {
			Renamer.deleteNonLettersFromTheStart();
		}

		// insert delimeter between letters and non-letters
		if( opt(9) ) {
			Renamer.delimBetweenLettersAndNonLetters();
		}

		// replace substring with another substring in Filename
		if( opt(10) ) {

			QString from = ui.lineEdit_01->text();
			QString to	 = ui.lineEdit_02->text();

			if( from != "" )
				Renamer.replaceSubstring(from, to, ui.spinBox_01->value(), opt(11), true);
		}

		// replace substring with another substring in Extension
		if( opt(12) ) {

			QString from = ui.lineEdit_03->text();
			QString to	 = ui.lineEdit_04->text();

			if( from != "" )
				Renamer.replaceSubstring(from, to, ui.spinBox_02->value(), opt(13), false);
		}

		// delete number of symbols at the beginning of at the end pf the string
		if( opt(14) ) {

			int count = ui.spinBox_03->value();
			bool left = ui.comboBox_01->currentIndex() == 0;
			int pos   = opt(15) ? ui.spinBox_04->value() : 0;

			Renamer.deleteSomeSymbols(count, left, pos);
		}

		// replace sequence of samples to single sample
		if( opt(16) ) {
	
			if( ui.lineEdit_05->text() != "" )
				Renamer.sequenceToSingle(ui.lineEdit_05->text());
		}

		// remove all symbols before/after the delimeter
		if( opt(17) ) {
	
			if( ui.lineEdit_06->text() != "" )
				Renamer.deleteSymbolsBeforeDelim(ui.lineEdit_06->text(), opt(19), !opt(18));
		}

		// put 'Space' before every capital letter except for the first one
		if( opt(20) ) {
		
			Renamer.isolateCapitalLetters();
		}

		// insert substring
		if( opt(21) )
		{
			if( ui.rb_004->isChecked() )
				// insert substring at given position
				Renamer.insertAt(ui.lineEdit_07->text(), ui.spinBox_05->value(), opt(22));
			else if( ui.rb_005->isChecked() )
				// insert substring just before the substring found within the string (optionaly, just after the substring)
				Renamer.insertAtSubstring(ui.lineEdit_07->text(), ui.lineEdit_09->text(), opt(27));
			else if( ui.rb_006->isChecked() )
				// insert substring using the mask
				Renamer.insertByMask(ui.lineEdit_07->text(), ui.lineEdit_10->text(), ui.spinBox_07->value());
		}

		// rename using date/time attributes of the files
		if( opt(24) )
		{
			int btn = -1;
			if( ui.rb_001->isChecked() )
				btn = 1;
			else if( ui.rb_002->isChecked() )
				btn = 2;
			else if( ui.rb_003->isChecked() )
				btn = 3;

			Renamer.insertAttrDate(ui.lineEdit_08->text(), btn, ui.spinBox_06->value(), opt(25));
		}

		// rename mp3's using mp3 tags
		if( opt(26) )
		{
			Renamer.mp3Tag();
		}

		if( opt(28) )
		{
			Renamer.renByTemplate(ui.lineEdit_11->text(), ui.spinBox_08->value());
		}

		if (opt(29))
		{
			bool fromEnd1 = ui.cb_29_1->isChecked();
			bool fromEnd2 = ui.cb_29_2->isChecked();

			Renamer.moveFromPosToPos(ui.spinBox_29_1->value(), ui.spinBox_29_2->value(), ui.spinBox_29_3->value(), fromEnd1, fromEnd2);
		}

		// запускаем фактическое переименование или просто отображаем файлы в таблице
		if( doRename )
		{
			int errCount = Renamer.Rename();
			table->displayRenamedFiles(newFilesList, oldFilesList);

			if (errCount)
				QMessageBox::information(ui.centralWidget, "Ahtung!", QString::number(errCount) + "files were NOT renamed :'(");
		}
		else
		{
			table->displayRenamedFiles(newFilesList, newFilesList);
		}

	}
}

// отслеживаем изменение выделения в listWidgetExtensions и применяем выбранную маску к файлам в таблице
void qt_002_dirTree::on_listWidgetExtensions_itemSelectionChanged()
{
	QString mask = "";

	QList<QListWidgetItem*> Selected = ui.listWidgetExtensions->selectedItems();

	for(int i = 0 ; i < Selected.count(); i++)
		mask.append(Selected[i]->text() + ";");

	mask.chop(1);

	if( mask != "" )
	{
		if( mask.contains("*.*") )
			table->applyExtensionFilter(false, mask);
		else
			table->applyExtensionFilter(true, mask);
	}
}

// когда меняется отмеченная папка в дереве, перестраиваем список файлов в таблице
void qt_002_dirTree::TreeItemSelectionChanged()
{
	QMap<QString, char> ExtensionsMap;

	tree->getFileList(filesList, filesMap, ExtensionsMap, dtOptions);
	table->showFiles(&filesList);

	// отображаем все присутствующие расширения файлов в списке
	QList<QString> listExt = ExtensionsMap.keys();
	listExt.prepend("*.*");
	ui.listWidgetExtensions->clear();

	for(int i = 0; i < listExt.length(); i++)
	{
		QListWidgetItem* item = new QListWidgetItem(listExt[i]);
		ui.listWidgetExtensions->addItem(listExt[i]);
	}

	ui.listWidgetExtensions->item(0)->setSelected(true);
	ui.checkBoxSelAll->setChecked(false);
	ui.pushButtonProcess->setEnabled(false);
}

// клик по кнопке "Перечитать директорию"
void qt_002_dirTree::on_reReadPushButton_clicked()
{
	ui.reReadPushButton->setEnabled(false);
	tree->reReadSelectedDir();

	// ??? хотим сделать сортировку, возможно нужно использовать map
	//filesList.sort();

	ui.reReadPushButton->setEnabled(true);
}

// отметить/снять отметку со всех файлов в таблице
// отказался от использования сигнала/слота _stateChanged, т.к. это приводило к закольцовыванию
void qt_002_dirTree::on_checkBoxSelAll_clicked(bool State)
{
	ui.pushButtonProcess->setEnabled(State);
	table->checkAllFiles(State);
}

// вернуть отмеченные файлы в зад, как было при открытии папки
void qt_002_dirTree::on_pb_Undo_clicked()
{
	QMap<QString, QString>::iterator iter;

	// получим список полных исходных имен файлов с путем, отмеченных в левой части таблицы
	QStringList oldFilesList, newFilesList;
	table->getCheckedFiles(oldFilesList, newFilesList);

	// проходим по всему мапу и заносим в новый список исходные имена, вытащенные из мапа
	for (iter = filesMap.begin(); iter != filesMap.end(); ++iter)
	{
		int pos = oldFilesList.indexOf(iter.key());

		if( pos >= 0)
			newFilesList[pos] = iter.value();
	}

	FileRenameQT Renamer(oldFilesList, newFilesList, &filesMap, &ini);

	int errCount = Renamer.Rename();
	table->displayRenamedFiles(newFilesList, oldFilesList);

	if(errCount)
		QMessageBox::information(ui.centralWidget, "Ahtung!", QString::number(errCount) + "files were NOT renamed :'(");
}

// отреагировать на включение/выключение чекбоксов в таблице
void qt_002_dirTree::onCheckBoxesChanged(bool atLeastOneItemChecked, bool allItemsChecked)
{
	ui.pushButtonProcess->setEnabled(atLeastOneItemChecked);
	ui.checkBoxSelAll->setChecked(allItemsChecked);
}
