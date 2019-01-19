/***
*
*	Copyright (c) 2016-2019 Marco 'eukara' Hladik. All rights reserved.
*
*	See the file LICENSE attached with the sources for usage details.
*
****/

enumflags
{
	TEXTBOX_VISIBLE,
	TEXTBOX_HOVER,
	TEXTBOX_DOWN,
	TEXTBOX_FOCUS,
};

class CTextBox:CWidget
{
	string m_text;
	int m_length;
	int m_flags;

	void() CTextBox;
	virtual void() Draw;
	virtual void(float type, float x, float y, float devid) Input;
	virtual void(string txt) SetText;
};

void CTextBox::CTextBox(void)
{
	m_length = 184;
}

void CTextBox::Draw(void)
{
	if (g_focuswidget == this) {
		drawfill([g_menuofs[0]+m_x,g_menuofs[1]+m_y], [m_length,24], [0.5,0.5,0.5], 1.0f);
	} else {
		drawfill([g_menuofs[0]+m_x,g_menuofs[1]+m_y], [m_length,24], [0.25,0.25,0.25], 1.0f);
	}
	drawfill([g_menuofs[0]+m_x+3,g_menuofs[1]+m_y+3], [m_length-6,18], [0,0,0], 1.0f);
	drawfont = font_label;

	if (m_flags & TEXTBOX_FOCUS) {
		if (rint(time*4) & 1) {
			drawstring([g_menuofs[0]+m_x+6,g_menuofs[1]+m_y+6], 
				sprintf("%s|", m_text), [12,12], col_input_text, 1.0f, 0);
			return;
		}
	}

	if (m_text) {
		drawstring([g_menuofs[0] + m_x + 6, g_menuofs[1] + m_y + 6], m_text,
					[12,12], col_input_text, 1.0f, 0);
	}
}

void CTextBox::Input(float type, float x, float y, float devid)
{
	if (type == IE_KEYDOWN) {
		switch (x) {
		case K_MOUSE1:
			if (Util_CheckMouse(m_x,m_y,m_length,24)) {
				g_focuswidget = this;
				m_flags |= TEXTBOX_DOWN;
			}
			break;
		case K_BACKSPACE:
			if (m_flags & TEXTBOX_FOCUS) {
				m_text = substring(m_text, 0, strlen(m_text) - 1);
			}
			break;
		default:
			if (m_flags & TEXTBOX_FOCUS) {
				m_text = sprintf("%s%s", m_text, chr2str(y));
			}
		}
	} else if (type == IE_KEYUP) {
		if (x == K_MOUSE1) {
			if (m_flags & TEXTBOX_DOWN && Util_CheckMouse(m_x,m_y,m_length,24)) {
				m_flags |= TEXTBOX_FOCUS;
			} else {
				m_flags -= (m_flags & TEXTBOX_FOCUS);
			}
			m_flags -= (m_flags & TEXTBOX_DOWN);
		}
	}
}

void CTextBox::SetText(string txt)
{
	m_text = txt;
}