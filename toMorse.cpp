#include <iostream>

using namespace std;

#define SIZE 80 //this many characters will it read at once

//translates one character to Morse
void charToMorse(char c)
{
	string morse[] = { ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "..._", ".--", "-..-", "-.--", "--..", "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."};

	//letters
	if(c >= 'a' && c <= 'z')
	{
		c = c - 'a';
	}
	else if (c >= 'A' && c <= 'Z')
	{
		c = c - 'A';
	}
	//numbers
	else if (c >= '0' && c <= '9')
	{
		c = c - '0' + 'Z' - 'A';
	}
	else
	{
		//special symbols
		switch(c)
		{
			case '+': cout << ".-.-./"; return;
			case '=': cout << "-...-/"; return;
			case '/': cout << "-..-./"; return;
			default: return;
		}
	}
	cout << morse[(int)c] << "/";
}
//goes through the chracters and then translates them one by one
void toMorse(char s[SIZE])
{
	for(int i = 0; i < SIZE; ++i)
	{
		if(s[i] == 0)
		{
			break;
		}
		charToMorse(s[i]);
	}
	cout << endl;
}

int main(void)
{
	cout << "Write something and it will be translated to morse" << endl;
	while(!cin.eof())
	{
		char s[SIZE] = {0};
		cin >> s;
		toMorse(s);
	}	
	return 0;
}
