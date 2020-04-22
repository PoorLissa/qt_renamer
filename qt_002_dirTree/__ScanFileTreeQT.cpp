/*
	Although you can't set a size of the branch directly you can manipulate it indirectly like this:
	style="box-sizing: border-box;">QTreeView::item { height: 100px; } // manipulates item but influences the height of the branch too
	QTreeView { qproperty-indentation: 100; } // offsets an item and so also widens the branch, note no unit (px)
*/


#include "__ScanFileTreeQT.h"

#define ATTR_NORMAL	0
#define ATTR_HIDDEN	1

#include <QPainter>
#include <QHeaderView>



ScanFileTreeQT::ScanFileTreeQT(QTreeWidget *TreeViewObj, bool sMode) : Tree(TreeViewObj), FastSearch(sMode)
{
	// Customly connected slots/signals
	connect(Tree, SIGNAL(itemExpanded (QTreeWidgetItem*)), this, SLOT(itemExpanded (QTreeWidgetItem*)));
	connect(Tree, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(itemCollapsed(QTreeWidgetItem*)));

	init();
	ScanHardDrives();
}

ScanFileTreeQT::~ScanFileTreeQT()
{
	lastPathToFile();

	delete normalDirIcon;
	delete hiddenDirIcon;
	delete driveIcon;
	delete Tree;
}

// ����, ����������� ������ � ��������� ����
void ScanFileTreeQT::itemExpanded(QTreeWidgetItem *item)
{
	//item->setTextColor(0, RGB(66, 132, 255));
	NodeExpand(item);
}

// ����, ����������� ������ � �������� ����
void ScanFileTreeQT::itemCollapsed(QTreeWidgetItem *item)
{
	//QTreeWidgetItem *Parent_item = item->parent();
}

// ����������� ��� treeView
void ScanFileTreeQT::init()
{
	Tree->setColumnCount(1);
	Tree->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
	Tree->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);	// ����� �������� ��������� ����� � ������� Ctrl
	Tree->setIconSize(QSize(32, 32));
	Tree->setFont(QFont("MS Sans Serif"));
	Tree->setAnimated(false);
	Tree->setHeaderHidden(true);
	Tree->setIndentation(40);

	// ��������� ����� ������� QTreeWidget (���� ������� ������ '+'/'-')
	Tree->setStyleSheet(
		/*
		"QTreeWidget {"
			"background-color: yellow;"
		"}"*/

		// ��������� �����������, ��. ���� Resource Files/<name>.qrc
		"QTreeView::branch:has-children:!has-siblings:closed, QTreeView::branch:closed:has-children:has-siblings {\
			border-image: none;\
			image: url(:/qt_002_dirTree/images/treeBranch-closed.png);\
		}"

		"QTreeView::branch:open:has-children:!has-siblings, QTreeView::branch:open:has-children:has-siblings {\
			border-image: none;\
			image: url(:/qt_002_dirTree/images/treeBranch-open.png);\
		}"
	);

	Mask = "*.*";

	// �������� ������
	normalDirIcon = new QIcon();
	hiddenDirIcon = new QIcon();
	driveIcon     = new QIcon();

	// ������ ��� ������
	*driveIcon = Tree->style()->standardIcon(QStyle::SP_DriveHDIcon);

	// ������� ������ ��� ����� � ������
	*normalDirIcon = Tree->style()->standardIcon(QStyle::SP_DirIcon);

	// ������� ������ ��� ������� ����� � ������
	QPixmap normalPixmap((*normalDirIcon).pixmap((*normalDirIcon).actualSize(QSize(32, 32))));
	QPixmap disabledPixmap(QSize(32, 32));
	disabledPixmap.fill(Qt::transparent);
	QPainter painter(&disabledPixmap);

	painter.setBackgroundMode(Qt::TransparentMode);
	painter.setBackground(QBrush(Qt::transparent));
	painter.eraseRect(normalPixmap.rect());

	painter.setOpacity(0.5);
	painter.drawPixmap(0, 0, normalPixmap);
	painter.end();

	(*hiddenDirIcon).addPixmap(disabledPixmap, QIcon::Disabled, QIcon::On);
}

// ������� ��� ������������ ������� ����� � ������ ������ ������� ������ ������
void ScanFileTreeQT::ScanHardDrives()
{
	Tree->clear();

	WCHAR	Drives[255];
	QString	Drive;
	WCHAR*	DrivePointer;

	// �������� ������ ���� ��������� ������; ���� �� ������ ��� 256/4, �� ����
	if (GetLogicalDriveStrings(256, Drives) <= 256) {

		DrivePointer = Drives;
		WCHAR fileSystem[80], volumeName[80];
		unsigned long p1 = 80;

		while (*DrivePointer != 0)
		{
			Drive = QString::fromStdWString(DrivePointer);
			int driveType = GetDriveType(DrivePointer);

			GetVolumeInformation(Drive.toStdWString().data(), volumeName, 80, &p1, &p1, &p1, fileSystem, p1);

			// ��� ������� ����������� ��� �������� ����� �������� ������
			// �������� � ������ ����� �� ����������
			// driveType == DRIVE_REMOVABLE �� ������������, �.�. ���������� ������ � ������ ������
			if (driveType == DRIVE_FIXED)
				BuildTree(NULL, Drive, QString::fromStdWString(volumeName));

			// ��������� �� 4 ������� �� ������� Drives
			DrivePointer += 4;
		}
	}
}

void ScanFileTreeQT::BuildTree(QTreeWidgetItem *Node, const QString Path, const QString volumeName = "")
{
	// ��������� �������� ��������� � ������
	if (Node == NULL)
	{
		QTreeWidgetItem *upperLevelNode = new QTreeWidgetItem(Tree);

		upperLevelNode->setText(0, volumeName + " (" + Path.section('\\', 0, 0) + ")");
		upperLevelNode->setExpanded(false);
		upperLevelNode->setSizeHint(0, QSize(0, 30));
		upperLevelNode->setIcon(0, *driveIcon);

		Tree->addTopLevelItem(upperLevelNode);

		Node = upperLevelNode;
	}

	QTreeWidgetItem *newNode;

	// ����� ������ ������ � ����������
	QString Mask = Path + "\\" + "*.*", nodePath, path, mask;

	// ���� ���������� ����� ����� � ������
	for (CScanFolder SF(Mask, 0); SF; SF++)
	{
		nodePath = QString::fromWCharArray(SF);
		int attr;

		if ( nodePath != "." && nodePath != ".." )
		{
			// ���������� ����������
			if (pathIsDirectory(Path + nodePath, attr))
			{
				QTreeWidgetItem *newNode = new QTreeWidgetItem();

				newNode->setText(0, nodePath);
				newNode->setIcon(0, attr == ATTR_HIDDEN ? *hiddenDirIcon : *normalDirIcon);

				newNode->setExpanded(false);
				newNode->setSizeHint(0, QSize(0, 30));

				Node->addChild(newNode);

				// ������� ������ ���������� � ��������� �� � ���� ����� ����������, ����� � ��� �������� ������ '+'
				QTreeWidgetItem *secretNode = new QTreeWidgetItem();
				secretNode->setText(0, "...");

				if (FastSearch == true)
				{
					// ���������� �����:
					// ����� �� ���������� ���� �������� ������, � ������ ����� � ������ ���� ������ '+', ���� ���� � ��� ��� ��������
					newNode->addChild(secretNode);
				}
				else
				{
					// ���� ����� ��������� �����:
					// ���� �� ������� ������ � ���� ������� ���� �� ���� ��������� �����, �� ������� ������ ���������� � ��������� �����.
					// ���� � ����� ���� ��������, �� � ��� �� ����� ������ '+'
					path = Path + "\\" + nodePath;
					mask = path + "\\" + "*.*";
					int attr;

					for (CScanFolder SF(mask, 0); SF; SF++)
					{
						QString innerPath = QString::fromWCharArray(SF);

						if ( innerPath != "." && innerPath != ".." )
						{
							if (pathIsDirectory(path + "\\" + innerPath, attr))
							{
								// �������� ������ �������������� ���������� (���� � ����� ���� ���� �� ���� ��������)
								// ��� �������� ������� ����, ��� ����� �� �����. ��� ��������� ���� ��� ����������
								// ��������� � ���������� ����������� �������� �����
								newNode->addChild(secretNode);
								break;
							}
						}
					}
				}
			}
			else
			{
				// ���������� ������ � ������. ����� �� �����, �.�. ����� �������������� � ������� findFiles()
				// newNode = Tree->Items->AddChild(Node, (AnsiString(SF)));
				// newNode->ImageIndex = 1;
				// newNode->SelectedIndex = newNode->ImageIndex;
			}
		}
	}

	Node->sortChildren(0, Qt::SortOrder::AscendingOrder);
}

// ���������, �������� �� ���� �����������
bool ScanFileTreeQT::pathIsDirectory(QString path, int &attr)
{
	LPCWSTR Path  = (const wchar_t*)path.utf16();
	DWORD dwAttrs = GetFileAttributes(Path);
	attr          = ATTR_NORMAL;

	// ���� ������ ��� ������
	if (dwAttrs & FILE_ATTRIBUTE_READONLY) {
		dwAttrs &= ~FILE_ATTRIBUTE_READONLY;
	}

	// ���� �������
	if (dwAttrs & FILE_ATTRIBUTE_HIDDEN) {
		dwAttrs &= ~FILE_ATTRIBUTE_HIDDEN;
		attr = ATTR_HIDDEN;
	}

	// ���� ���������
	if (dwAttrs & FILE_ATTRIBUTE_SYSTEM) {
		dwAttrs &= ~FILE_ATTRIBUTE_SYSTEM;
		attr = ATTR_HIDDEN;
	}

	// ���� �������� �����������
	if (dwAttrs & FILE_ATTRIBUTE_DIRECTORY ) {
		if(path.contains("pagefile.sys")) {
			attr = ATTR_HIDDEN;
		}
		else  {
			dwAttrs &= ~FILE_ATTRIBUTE_DIRECTORY;
			return true;
		}
	}

	return false;
}

// �������� ���� ����� ��� ��������� ����
void ScanFileTreeQT::NodeExpand(QTreeWidgetItem *Node)
{
	// �������� ����
	Tree->setCurrentItem(Node, 0);

	// ���������� ����� ����������� ����������, � ������� ����� ������� ������� � ������ "..."
	if ((Node->childCount() == 1) && (Node->child(0)->text(0) == "...")) {
		Node->removeChild(Node->child(0));
		BuildTree(Node, getPath(Node));
	}
}

// ������� ������ ���� � �������� ����������/����
QString ScanFileTreeQT::getPath(QTreeWidgetItem *Node)
{
	QTreeWidgetItem *currentNode = Node;
	QString Path = "", root;

	// �������� ������ ���� � ���������� ����� �����
	while (currentNode->parent() != NULL) {
		Path.prepend(currentNode->text(0) + "\\");
		currentNode = currentNode->parent();
	}

	root = currentNode->text(0);
	root.chop(root.length() - root.indexOf(':') - 1);
	Path.prepend(root.right(2) + "\\");

	return Path;
}

QString ScanFileTreeQT::getCurrNodeName()
{
	QTreeWidgetItem *currentNode = Tree->selectedItems()[0];
	return getPath(currentNode);
}

// ��������� ��������������� ������ �������, ������������ � ��������� �����������
void ScanFileTreeQT::getFileList(QStringList& list, qMapSS& filesMap, qMapSC& ExtensionsMap, const dirTreeOptions &dtOpt)
{
	list.clear();
	filesMap.clear();

	QList<QTreeWidgetItem*> Nodes = Tree->selectedItems();

	// �������� �� ���� �����, ��� ������ ���� �������� ������ ������ � ��������� ��� � ��� ������
	for(int i = 0; i < Nodes.length(); i++)
		findFiles(getPath(Nodes[i]), list, filesMap, ExtensionsMap, dtOpt);
}

// ��������������� ������� ����� � ��������� ����������
void ScanFileTreeQT::findFiles(QString Path, QStringList& list, qMapSS& filesMap, qMapSC& ExtensionsMap, const dirTreeOptions &dtOpt)
{
	QString nodePath;

	// Set up search options
	bool includeSub = false;
	bool includeDir = dtOpt.showDirs;

	list.push_back(Path + nodePath + "?");

    for(CScanFolder SF(Path + "*.*", 0); SF; SF++)
	{
		nodePath = QString::fromWCharArray(SF);
		int attr;

		// �������� � ������ ����� � ���������, ���� ���������� ��������������� ���������
		if( pathIsDirectory(Path + nodePath, attr) )
		{
			if (nodePath != "." && nodePath != "..")
			{
				if (includeSub)
				{
					findFiles(Path + nodePath, list, filesMap, ExtensionsMap, dtOpt);
				}

				// ??? se later -- also need to be able to sort folders before the files
				if (includeDir)
				{
					list.push_back(nodePath);
				}
			}
		}
        else
		{
			// �������� � ������ ����� � ������� �����
			//if( allowByMask(AnsiString(SF)) )
            {
				//list.push_back( Path[Path.length()-1] == '\\' ? Path + nodePath : Path + '\\' + nodePath);	// ������ ����
				// � ������ ������ ������ ��� �����
				list.push_back( nodePath );

				// � map ������ ������ ����
				QString fullPath = Path[Path.length()-1] == '\\' ? Path + nodePath : Path + '\\' + nodePath;
				filesMap.insert(fullPath, fullPath);

				QString ext = nodePath.right(nodePath.length() - nodePath.lastIndexOf('.'));

				ExtensionsMap.insert(ext, '.');
			}
		}
	}

	return;
}

void ScanFileTreeQT::lastPathToFile()
{
	QList<QTreeWidgetItem*> currentNodeList = Tree->selectedItems();

	if( currentNodeList.length() > 0 ) 
	{
		char ch[MAX_PATH];
		GetModuleFileNameA(NULL, ch, MAX_PATH);

		std::string fileName(ch);
		fileName = fileName.substr(0, fileName.find_last_of('\\')+1) + "_path";

		QString path = getPath(currentNodeList[0]);
		std::ofstream outFile(fileName);
		outFile << path.toStdString().data();
		outFile.close();
	}
}

QString ScanFileTreeQT::lastPathFromFile()
{
	QString path = "";
    char ch[MAX_PATH];

	std::fstream inFile;

	inFile.open("_path", std::fstream::in);

	if (inFile.is_open())
	{
		while (inFile >> ch) {
			path += ch;
			path += " ";	// wtf? �� ��� ���� ������� �� ���� �������� ��� �������...
		}

//		path = path.TrimRight();
		inFile.close();
	}

    return path;
}

void ScanFileTreeQT::setLastDirectory(QString path)
{
	if (path.isEmpty())
		setDirectory(lastPathFromFile());
	else
		setDirectory(path);
}

// ��������� � ������ ��������� ����� �� ������� ������
void ScanFileTreeQT::setDirectory(QString path)
{
	path = path.toLower();
	QStringList dirs = path.split("\\");
	QTreeWidgetItem* currentNode = Tree->invisibleRootItem();

	// �������� �� ���� �������� ������ ���� � �����
	for(int i = 0; i < dirs.length(); i++) {
	
		path = dirs[i];
		bool found = false;

		// �������� �� ���� �������� ������� ���� � ������
		for(int j = 0; j < currentNode->childCount(); j++)
		{
			QString nodeName = currentNode->child(j)->text(0).toLower();

			// ���� ����� ������� ����� ������� ����� ���� (��� �������� ����� ����� + ':'), ������������� �� ������� ������
			if( path == nodeName || ( i == 0 && nodeName.contains(path) ) )
			{
				found = true;
				currentNode = currentNode->child(j);
				break;
			}
		}

		if( found )
			currentNode->setExpanded(true);
		else
			break;
	}

	return;
}

// ���������� ��� ���������� ���������� (�������� ���� ����� �� �������� �������, ��������, �� ������)
// ����� ���������� ����� � ���� �����������, ����� � ��� �� ����� ������� ����� ����� ������ ������� getFileList(filesList);
void ScanFileTreeQT::reReadSelectedDir()
{
	QList<QTreeWidgetItem*> selected = Tree->selectedItems();

	for(int i = 0; i < selected.length(); i++) {

		QTreeWidgetItem *item = selected[i];
		QString path = getPath(item);

		bool isExpanded = item->isExpanded();

		for(int j = item->childCount()-1; j >= 0; j--)
			item->removeChild(item->child(j));

		BuildTree(item, path);

		if( isExpanded )
			item->setExpanded(true);

		// �������� ������ � ���, ��� ���������� ��������� � ������, ����� ���������� ����� � �����
		Tree->itemSelectionChanged();
	}
}