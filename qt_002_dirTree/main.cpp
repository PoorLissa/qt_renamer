#include "qt_002_dirtree.h"
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

			for(size_t i = 0; i < param.length(); i++)
			{
				if( param[i] != '"' )
					path += static_cast<char>(param[i]);
			}

			if( path[path.length()] != '\\' )
				path += '\\';
		}
	}

	QApplication a(argc, argv);

	qt_002_dirTree w(path);

	w.show();

	return a.exec();
}
