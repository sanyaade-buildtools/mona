// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifdef MONA
#include <monapi.h>
#include <monapi/messages.h>

extern dword __gui_server;
#endif
#include <gui/messages.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/ControlPaint.h>
#include <gui/System/Drawing/Font.h>
#include <gui/System/Math.h>

#define BASE Control

using namespace System;
using namespace System::Drawing;

namespace System { namespace Mona { namespace Forms
{
	Form::Form() : isCloseButtonPushed(false), ncState(NCState_None)
	{
		this->offset = Point(2, Control::get_DefaultFont()->get_Height() + 8);
	}
	
	Form::~Form()
	{
		this->Dispose();
	}
	
	void Form::Hide()
	{
		if (!this->get_Visible()) return;
		
		BASE::Hide();
		this->Erase();
	}
	
	void Form::Create()
	{
		BASE::Create();
		Application::AddForm(this);
		this->isCloseButtonPushed = false;
		this->ncState = NCState_None;
	}
	
	void Form::Dispose()
	{
		BASE::Dispose();
		Application::RemoveForm(this);
	}
	
	void Form::Erase()
	{
		if (this->buffer == NULL) return;
		
#ifdef MONA
		Rectangle r = this->get_Bounds();
		MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_DRAWWALLPAPER, r.X, r.Y, MAKE_DWORD(r.Width, r.Height));
#else
		Size sz = this->get_Size();
		_P<Bitmap> bmp = new Bitmap(sz.Width, sz.Height);
		int len = sz.Width * sz.Height;
		Color white = Color::get_White();
		Color empty = Color::get_Empty();
		for (int i = 0; i < len; i++)
		{
			(*bmp.get())[i] = (*this->buffer.get())[i].get_A() != 0 ? white : empty;
		}
		this->DrawImage(bmp);
#endif
	}
	
	bool Form::CheckPoint(int x, int y)
	{
		return this->get_Bounds().Contains(x, y)
			&& this->buffer->GetPixel(x - this->get_X(), y - this->get_Y()).get_A() != 0;
	}
	
	void Form::OnPaint()
	{
		if (this->buffer == NULL) return;
		
		_P<Graphics> g = Graphics::FromImage(this->buffer);
		int w = this->get_Width(), h = this->get_Height(), oy = this->offset.Y;
		_P<Font> f = Control::get_DefaultFont();
		Size sz = g->MeasureString(this->get_Text(), f);
		int tx = 2 + 2 + (oy - 8) + 2, tw = Math::Min(tx + sz.Width + 1 + 2 + 2, w);
		Color tf = Color::get_Black(), tb = Color::FromArgb(0xff, 0xe0, 0);
		
		// Title Bar
		ControlPaint::DrawEngraved(g, 0, 0, tw, oy - 2);
		if (tw < w) g->FillRectangle(Color::get_Empty(), tw, 0, w - tw, oy - 2);
		g->FillRectangle(tb, 2, 2, tw - 4, oy - 4);
		
		// Close Button
		ControlPaint::DrawEngraved(g, 4, 4, oy - 8, oy - 8);
		if (this->isCloseButtonPushed)
		{
			g->FillRectangle(Color::FromArgb(0x80, 0x80, 0), 6, 6, oy - 12, oy - 12);
		}
		
		// Border
		ControlPaint::DrawEngraved(g, 0, oy - 2, w, h - (oy - 2));
		
		// Caption
		g->set_ClientRectangle(Rectangle(tx, 4, w - tx - 4, oy - 8));
		g->DrawString(this->get_Text(), f, tf, 0, 0);
		g->DrawString(this->get_Text(), f, tf, 1, 0);
		
		g->Dispose();
	}
	
	Form::NCState Form::NCHitTest(int x, int y)
	{
		int oy = this->offset.Y, xx = x + this->offset.X, yy = y + oy;
		if (Rectangle(4, 4, oy - 8, oy - 8).Contains(xx, yy)) return NCState_CloseButton;
		if (yy < oy) return NCState_TitleBar;
		return NCState_None;
	}
	
	void Form::DrawReversibleRectangle()
	{
		Rectangle r = this->get_Bounds();
		r.X += this->ptRevRect.X - this->clickPoint.X;
		r.Y += this->ptRevRect.Y - this->clickPoint.Y;
		//r.X--; r.Y--; r.Width += 2; r.Height += 2;
		for (int i = 0; i < 1; i++)
		{
			ControlPaint::DrawReversibleRectangle(r);
			r.X++; r.Y++; r.Width -= 2; r.Height -= 2;
		}
	}
	
	void Form::OnNCMouseMove(_P<MouseEventArgs> e)
	{
		switch (this->ncState)
		{
			case NCState_CloseButton:
			{
				bool pushed = this->NCHitTest(e->X, e->Y) == NCState_CloseButton;
				if (this->isCloseButtonPushed != pushed)
				{
					this->isCloseButtonPushed = pushed;
					this->OnPaint();
					this->Refresh();
				}
				break;
			}
			case NCState_TitleBar:
			{
				this->DrawReversibleRectangle();
				this->ptRevRect = Point(e->X, e->Y);
				this->DrawReversibleRectangle();
				break;
			}
			default:
				break;
		}
		
		BASE::OnNCMouseMove(e);
	}
	
	void Form::OnNCMouseDown(_P<MouseEventArgs> e)
	{
		switch (this->ncState = this->NCHitTest(e->X, e->Y))
		{
			case NCState_CloseButton:
				this->isCloseButtonPushed = true;
				this->OnPaint();
				this->Refresh();
				break;
			case NCState_TitleBar:
				this->ptRevRect = Point(e->X, e->Y);
				this->DrawReversibleRectangle();
				break;
			default:
				break;
		}
		
		BASE::OnNCMouseDown(e);
	}
	
	void Form::OnNCMouseUp(_P<MouseEventArgs> e)
	{
		bool destroy = this->NCHitTest(e->X, e->Y) == NCState_CloseButton && this->ncState == NCState_CloseButton;
		
		switch (this->ncState)
		{
			case NCState_CloseButton:
			{
				this->isCloseButtonPushed = false;
				if (destroy) this->Hide();
				break;
			}
			case NCState_TitleBar:
			{
				this->DrawReversibleRectangle();
				this->Erase();
				Point p = this->get_Location();
				p.X += e->X - this->clickPoint.X;
				p.Y += e->Y - this->clickPoint.Y;
				this->set_Location(p);
				this->Refresh();
				break;
			}
			default:
				break;
		}
		this->Refresh();
		
		BASE::OnNCMouseUp(e);
		
		if (destroy) this->Dispose();
		this->ncState = NCState_None;
	}
}}}
