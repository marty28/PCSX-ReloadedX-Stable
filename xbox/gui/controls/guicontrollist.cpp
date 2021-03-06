#include "guicontrollist.h"
#include "..\xboxgui.h"
#include "..\guimessage.h"
#include "..\..\input\sysxboxinput.h"
#include "..\utils\stringutils.h"

using namespace std;

#define CONTROL_LEBEL_ITEM 2

CGUIListItem::CGUIListItem(std::string strName, std::string strFullPath, bool bIsDirectory)
{
	m_strName = strName;
	m_strFullPath = strFullPath;
	m_bIsDirectory = bIsDirectory;
}

CGUIListItem::~CGUIListItem()
{
}

CGUIControlList::CGUIControlList(int iControlID, int iWindowID, int iPosX, int iPosY, int iWidth, int iHeight, int iLineSpacing, int iItemsPerPage, std::string strFont, DWORD dwColor, unsigned int iSize, DWORD dwSelectedColor)
: CGUIControl(iControlID, iWindowID, iPosX, iPosY, iWidth, iHeight)
{
	m_iCursor = 0;
	m_iOffset = 0;
	m_iItemsPerPage = iItemsPerPage; // TODO: Calculate from item size and control height
	m_iPage = 0;
	m_iTotalPages = 0;
	m_iLineSpacing = iLineSpacing;
	m_pFont = g_XboxGUI.GetFontManager().GetFont(strFont);
	m_iFontSize = iSize;
	m_dwColor = dwColor;
	m_dwSelectedColor = dwSelectedColor;
}

CGUIControlList::~CGUIControlList()
{
}

void CGUIControlList::Render()
{
	if(m_vecItems.size() == 0)
		return;

	int iYPos = m_iPosY;
	int iSize = m_iItemsPerPage;

	if(m_vecItems.size() < (unsigned int)m_iItemsPerPage)
		iSize = m_vecItems.size();

	if((m_iCursor+m_iItemsPerPage) > (int)m_vecItems.size())
		iSize = m_vecItems.size() - m_iOffset;

	for(int i = m_iOffset; i < iSize+m_iOffset; i++)
	{
		CGUIListItem* pListItem = m_vecItems[i];
		if(pListItem)
		{
			if(m_pFont)
			{
				if(i == m_iCursor && HasFocus())
					m_pFont->Render(m_iPosX, iYPos, m_iFontSize, m_dwSelectedColor, pListItem->GetName());
				else
					m_pFont->Render(m_iPosX, iYPos, m_iFontSize, m_dwColor, pListItem->GetName());
			}
			iYPos += m_iLineSpacing;
		}
	}

	// TODO: Don't send this each frame, only when updated
	// Update our selection label
	string strLabel = "Item " + CStringUtils::IntToString(m_iCursor+1) + "/" + CStringUtils::IntToString(m_vecItems.size());
	strLabel += " - Page " + CStringUtils::IntToString(m_iPage+1) + "/" + CStringUtils::IntToString(m_iTotalPages+1);
	CGUIMessage msg(GUI_MSG_SET_LABEL, GetID(), CONTROL_LEBEL_ITEM, strLabel);
	g_XboxGUI.SendMessage(msg);

	CGUIControl::Render();
}

bool CGUIControlList::OnKey(int iKey)
{
	if(iKey == K_XBOX_DPAD_DOWN)
	{
		if(m_iCursor+1 < (int)m_vecItems.size())
		{
			m_iCursor++;

			if(m_iCursor >= m_iItemsPerPage+m_iOffset)
				m_iOffset++;
		}
		m_iPage = m_iCursor / m_iItemsPerPage;
		return true;
	}

	if(iKey == K_XBOX_DPAD_UP)
	{
		if((m_iCursor > 0))
		{
			m_iCursor--;

			if(m_iCursor < m_iOffset)
			{
				if(m_iOffset > 0)
					m_iOffset--;
			}
		}
		m_iPage = m_iCursor / m_iItemsPerPage;
		return true;
	}

	if(iKey == K_XBOX_DPAD_LEFT)
	{
		if(m_iPage != 0)
		{
			m_iCursor = (m_iPage-1)* m_iItemsPerPage;
			m_iOffset = m_iCursor;
		}
		m_iPage = m_iCursor / m_iItemsPerPage;
		return true;
	}

	if(iKey == K_XBOX_DPAD_RIGHT)
	{
		if(m_iPage < m_iTotalPages)
		{
			m_iCursor = (m_iPage+1) * m_iItemsPerPage;
			m_iOffset = m_iCursor;
		}
		m_iPage = m_iCursor / m_iItemsPerPage;
		return true;
	}

	if(iKey == K_XBOX_A)
	{
		CGUIMessage msg(GUI_MSG_CLICKED, GetParentID(), GetID());
		g_XboxGUI.SendMessage(msg);
		m_iPage = m_iCursor / m_iItemsPerPage;
		return true;
	}

	return CGUIControl::OnKey(iKey);
}

bool CGUIControlList::AddItem(CGUIListItem* pItem)
{
	m_vecItems.push_back(pItem);
	m_iTotalPages = m_vecItems.size() / m_iItemsPerPage;

	return true;
}

CGUIListItem* CGUIControlList::GetSelectedItem()
{
	CGUIListItem* pListItem = m_vecItems[m_iCursor];
	
	if(pListItem)
		return pListItem;

	return NULL;
}

int CGUIControlList::GetSize()
{
	return m_vecItems.size();
}

void CGUIControlList::Clear()
{
	for(int i = 0; i < (int)m_vecItems.size(); i++)
	{
		CGUIListItem* pListItem = NULL;
		pListItem = m_vecItems[i];

		if(pListItem)
			delete pListItem;
	}

	m_iCursor = m_iOffset = 0;

	m_vecItems.clear();
}

void CGUIControlList::FreeResources()
{
	Clear();

	CGUIControl::FreeResources();
}