#include "qt_002_dirtree.h"
#include "__helpers.h"
#include <QtWidgets/QApplication>



int main(int argc, char *argv[])
{
	QString path("");

	// set path from the argument, if present
	for (int i = 0; i < argc; i++)
	{
		std::string param(argv[i]);

		if( param.substr(0, 6) == "/path=" )
		{
			param = param.substr(6, param.length());

			std::wstring ws;
			convert_from_MultiByte_to_WideChar(ws, param.c_str());

			while (ws.back() == '"')
				ws.erase(ws.length()-1);

			path = QString::fromWCharArray(ws.c_str());

			if( path[path.length()] != '\\' )
				path += '\\';
		}
	}

	QApplication a(argc, argv);

	qt_002_dirTree w(path);

	w.show();

	return a.exec();
}
