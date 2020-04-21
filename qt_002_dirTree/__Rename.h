#ifndef __RENAME_H
#define __RENAME_H

#define CONSOLE_MODE

static char* shortWords[] = {"at", "am", "i", "this", "the"};
static char* LETTERS_ENG  = "abcdefghijklmnopqrstuvwxyz";
static char* LETTERS_RUS  = "абвгдеЄжзийклмнопрстуфхцчшщъыьэю€";
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
		for(int bit = 0; bit < 64; bit++)				// обходим битовый вектор
		{
			if( option->GetBit(bit) )					// если некоторый бит выставлен в 1...
			{
				for(iter = beg; iter != end; ++iter)	// ... то запускаем цикл по всему строковому вектору; в цикле выполн€ем метод, который кодируетс€ этим битом
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
	// провер€ем, есть ли слово в списке коротких слов, которые в норме не следует писать с большой буквы
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

	// всю строку в нижний регистр
	void toLowerCase(std::string &str) {
		std::transform(str.begin(),
                 str.end(),
                 str.begin(),
                 ::tolower);
	}

	// всю строку в верхний регистр
	void toUpperCase(std::string &str) {
		std::transform(str.begin(),
				str.end(),
				str.begin(),
				::toupper);
	}

	// первое слово строки начинаетс€ с заглавной буквы
	void firstSymbolToUpperCase(std::string &str) {
		toLowerCase(str);

		std::transform(str.begin(),
				str.begin()+1,
				str.begin(),
				::toupper);
	}

	// каждое слово строки начинаетс€ с заглавной буквы;
	// если хотим, чтобы короткие слова начинались с маленькой буквы, ставим useList = true
	void firstSymbolOfEveryWordToUpperCase(std::string &str, bool useList = false) {
		std::string word, SPACE;
		SPACE = SYMBOLS[0];
		int oldPos = 0, newPos, count = 0;

		toLowerCase(str);

		do {
			newPos = str.find(SYMBOLS[0], oldPos);			// ищем позицию пробела
			word = str.substr(oldPos, newPos - oldPos);		// вычленили отдельное слово

			if( (word != SPACE) && (word != "") )
			{	
				if( count == 0 )							// находим первую подстроку, начинающуюс€ с буквы (первое слово в строке ("01 - Let it be.mp3") ):
				{											// если это слово из списка коротких слов, его перва€ буква все равно должна быть заглавной
					char ch = word.at(0);					// поэтому, если count равен 1, не даем слову начатьс€ с маленькой буквы

					if( strchr(LETTERS_ENG, tolower(ch)) || strchr(LETTERS_RUS, tolower(ch)) )	// смотрим, €вл€етс€ ли буквой первый символ слова
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

	// убираем двойные пробелы и пробелы в начале и в конце строки
	void removeExtraSpaces(std::string &str) {
		int  count;
		std::string delimiter;
		delimiter = SYMBOLS[0];						// delimiter = пробел
		delimiter.append(delimiter);				// delimiter = двойной пробел

		while( str.at(0) == SYMBOLS[0] )
			str.erase(0, 1);

		while( str.at(str.length()-1) == SYMBOLS[0] )
			str.erase(str.length()-1, 1);

		int pos = str.find(delimiter, 0);			// ищем два пробела подр€д

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

	// убираем лишние пробелы перед знаками препинани€ и ставим недостающие пробелы после знаков препинани€
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

				if( ch1 == SPACE[0] )			// удал€ем все пробелы перед знаком препинани€
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
