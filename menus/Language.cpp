/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "Framework.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "Table.h"
#include "keydefs.h"

#define ART_BANNER		"gfx/shell/head_language"

enum
{
	COLUMN_NAME = 0,
	COLUMN_STATUS,
};

struct lang_t
{
	char code[16];   // Código do idioma (ex: "spanish", "english")
	char name[32];   // Nome do idioma (ex: "Español", "English")
	bool active;     // Se o idioma está ativo

	int NameCmp(const lang_t &other) const
	{
		return stricmp(name, other.name);
	}

	static int NameCmpAscend(const lang_t *a, const lang_t *b)
	{
		return a->NameCmp(*b);
	}

	static int NameCmpDescend(const lang_t *a, const lang_t *b)
	{
		return b->NameCmp(*a);
	}
};

class CMenuLangListModel : public CMenuBaseModel
{
public:
	CMenuLangListModel() {}

	void Update() override;
	int GetColumns() const override { return 2; } // Nome do idioma e status
	int GetRows() const override { return langs.Count(); }
	const char *GetCellText(int line, int column) override
	{
		switch (column)
		{
		case COLUMN_NAME: return langs[line].name;
		case COLUMN_STATUS: return langs[line].active ? "Ativo" : "Available";
		default: return NULL;
		}
	}

	CUtlVector<lang_t> langs;
};

void CMenuLangListModel::Update()
{
	langs.RemoveAll();

	// Idiomas disponíveis
	const char *availableLangs[][2] = {
		{"english", "English"},
		{"portuguese", "Português"},
		{"spanish", "Español"},
		{"french", "Français"}
	};

	for (int i = 0; i < 4; i++)
	{
		lang_t lang;
		Q_strncpy(lang.code, availableLangs[i][0], sizeof(lang.code));
		Q_strncpy(lang.name, availableLangs[i][1], sizeof(lang.name));

		// Define o idioma ativo baseado em gamefolder
		lang.active = !stricmp(lang.code, gMenu.m_gameinfo.gamefolder);

		langs.AddToTail(lang);
	}
}

class CMenuLanguage: public CMenuFramework
{
public:
	CMenuLanguage() : CMenuFramework("CMenuLanguage") { }

private:
	void ActivateLanguage(void *pExtra);
	virtual void _Init() override;

	CMenuPicButton *activate;
	CMenuTable langList;
	CMenuLangListModel langListModel;
};

void CMenuLanguage::ActivateLanguage(void *pExtra)
{
	// Obtém o idioma selecionado
	int index = langList.GetCurrentIndex();
	if (!langListModel.langs.IsValidIndex(index))
		return;

	const char *selectedLang = langListModel.langs[index].code;

	// Aplica a mudança de idioma e reinicia a engine
	char cmd[128];
	snprintf(cmd, sizeof(cmd), "ui_language \"%s\"; quit\n", selectedLang);
	EngFuncs::ClientCmd(FALSE, cmd);
}

void CMenuLanguage::_Init()
{
	banner.SetPicture(ART_BANNER);
	AddItem(banner);

	langList.SetupColumn(0, L("Language"), 0.70f);
	langList.SetupColumn(1, L("Status"), 0.30f);
	langList.SetModel(&langListModel);
	langList.SetRect(360, 230, -20, 400);
	AddItem(langList);

	activate = AddButton(L("Activate"), nullptr, PC_ACTIVATE, MenuCb(&CMenuLanguage::ActivateLanguage));
}

ADD_MENU(menu_language, CMenuLanguage, UI_Language_Menu)
