#ifndef __RENAME_H
#define __RENAME_H

#define CONSOLE_MODE

static char* shortWords[] = {"at", "am", "i", "this", "the"};
static char* LETTERS_ENG  = "abcdefghijklmnopqrstuvwxyz";
static char* LETTERS_RUS  = "��������������������������������";
static char *SYMBOLS	  = " !?().,;'\+-*/_";
static char *PUNCTUATION  = "!?.,;:";
static char *DIGITS		  = "0123456789";



class Rename {
 public:
	Rename(std::vector<std::string> *v, bitSet *opt) : vec(v), option(opt) {
		beg = vec->begin();
		end = vec->end();

		setlocale(LC_CTYPE, "Russian");
	}

	int Process() {
		for(int bit = 0; bit < 64; bit++)				// ������� ������� ������
		{
			if( option->GetBit(bit) )					// ���� ��������� ��� ��������� � 1...
			{
				for(iter = beg; iter != end; ++iter)	// ... �� ��������� ���� �� ����� ���������� �������; � ����� ��������� �����, ������� ���������� ���� �����
				{
					switch( bit )
					{
						case 0:
							firstSymbolOfEveryWordToUpperCase(*iter, true);
							break;
						case 1:
							removeExtraSpaces(*iter);
							break;
						case 2:
							fixSpacesAroundPunctuationSigns(*iter);
							break;
					}
				}
			}
		}

		return 0;
	}

	void Print() {
		for(iter = beg; iter != end; ++iter) {
			#ifdef CONSOLE_MODE
				std::cout << *iter << std::endl;
			#else
				richEdit->...
			#endif
		}
	}


 private:
	// ���������, ���� �� ����� � ������ �������� ����, ������� � ����� �� ������� ������ � ������� �����
	bool checkShortWordsList(std::string str) {
		bool res = false;
		toLowerCase(str);

		for(int i = 0; i < sizeof(shortWords)/sizeof(shortWords[0]); i++) {
			if( str == shortWords[i] ) {
				res = true;
				break;
			}
		}
		
		return res;
	}

	// ��� ������ � ������ �������
	void toLowerCase(std::string &str) {
		std::transform(str.begin(),
                 str.end(),
                 str.begin(),
                 ::tolower);
	}

	// ��� ������ � ������� �������
	void toUpperCase(std::string &str) {
		std::transform(str.begin(),
				str.end(),
				str.begin(),
				::toupper);
	}

	// ������ ����� ������ ���������� � ��������� �����
	void firstSymbolToUpperCase(std::string &str) {
		toLowerCase(str);

		std::transform(str.begin(),
				str.begin()+1,
				str.begin(),
				::toupper);
	}

	// ������ ����� ������ ���������� � ��������� �����;
	// ���� �����, ����� �������� ����� ���������� � ��������� �����, ������ useList = true
	void firstSymbolOfEveryWordToUpperCase(std::string &str, bool useList = false) {
		std::string word, SPACE;
		SPACE = SYMBOLS[0];
		int oldPos = 0, newPos, count = 0;

		toLowerCase(str);

		do {
			newPos = str.find(SYMBOLS[0], oldPos);			// ���� ������� �������
			word = str.substr(oldPos, newPos - oldPos);		// ��������� ��������� �����

			if( (word != SPACE) && (word != "") )
			{	
				if( count == 0 )							// ������� ������ ���������, ������������ � ����� (������ ����� � ������ ("01 - Let it be.mp3") ):
				{											// ���� ��� ����� �� ������ �������� ����, ��� ������ ����� ��� ����� ������ ���� ���������
					char ch = word.at(0);					// �������, ���� count ����� 1, �� ���� ����� �������� � ��������� �����

					if( strchr(LETTERS_ENG, tolower(ch)) || strchr(LETTERS_RUS, tolower(ch)) )	// �������, �������� �� ������ ������ ������ �����
						count++;
				}

				if( useList )
					if( count > 1 && checkShortWordsList(word) )
					{
						oldPos = newPos + 1;
						continue;
					}

				firstSymbolToUpperCase(word);
				str.replace(oldPos, word.length(), word);
			}

			oldPos = newPos + 1;

		} while( newPos != std::string::npos );
	}

	// ������� ������� ������� � ������� � ������ � � ����� ������
	void removeExtraSpaces(std::string &str) {
		int  count;
		std::string delimiter;
		delimiter = SYMBOLS[0];						// delimiter = ������
		delimiter.append(delimiter);				// delimiter = ������� ������

		while( str.at(0) == SYMBOLS[0] )
			str.erase(0, 1);

		while( str.at(str.length()-1) == SYMBOLS[0] )
			str.erase(str.length()-1, 1);

		int pos = str.find(delimiter, 0);			// ���� ��� ������� ������

		while( pos != std::string::npos ) {
			count = 0;

			while( str.at(pos) == SYMBOLS[0] ) {
				pos++;
				count++;
			}

			str.erase(pos-count, count-1);
			pos = str.find(delimiter, pos+1);
		}
	}

	// ������� ������ ������� ����� ������� ���������� � ������ ����������� ������� ����� ������ ����������
	void fixSpacesAroundPunctuationSigns(std::string &str) {
		std::string SPACE;
		SPACE = SYMBOLS[0];

		for(unsigned int i = 0; i < strlen(PUNCTUATION); i++)
		{
			char ch  = PUNCTUATION[i];
			int  pos = str.find(ch, 0);

			while( pos != std::string::npos ) {

				char ch1 = (pos == 0)				? '.' : str.at(pos-1);
				char ch2 = (pos == str.length()-1)  ? '.' : str.at(pos+1);

				if( ch1 == SPACE[0] )			// ������� ��� ������� ����� ������ ����������
				{
					do {
						str.erase(--pos, 1);

						if( pos == 0 )
							break;
						else
							ch1 = str.at(pos-1);

					} while( ch1 == SPACE[0] );
				}

				if( ch2 == SPACE[0] )
				{
					do {
						str.erase(pos+1, 1);

						if( pos+1 < str.length()-1 )
							ch2 = str.at(pos+1);
						else
							break;

					} while( ch2 == SPACE[0] );
				}

				if( pos < str.length()-1 )
				{
					for(int i = 0; i < strlen(PUNCTUATION); i++)
						if( str.at(pos+1) == PUNCTUATION[i] )
							break;

					str.insert(pos+1, SPACE);
				}

				if( pos < str.length() )
					pos++;

				pos = str.find(ch, pos);
			}
		}
	}

 private:
	std::vector<std::string> *vec;
	bitSet *option;

	std::vector<std::string>::iterator
		beg, end, iter;
};

#endif
