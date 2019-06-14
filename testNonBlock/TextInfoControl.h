#pragma once
#include <hash_map>
#include <string>
#include "GameText.h"

using namespace std;
#define TEXT_MAP std::hash_map<std::string, GameText> 

enum TYPE {
	SCORE_UP,
	ADD_TEXT,
};

class TextInfoControl
{
private:
	TEXT_MAP texts;
	void Add(GameText gameText);
public:
	TYPE ControlText(string Text, DWORD userId);
};

