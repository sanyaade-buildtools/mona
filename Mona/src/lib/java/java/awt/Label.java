/*
Copyright (c) 2005 bayside

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation files 
(the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

package java.awt;

/**
 ラベルクラス
*/
public class Label extends Component {
	/** 左寄せ */
	public static final int LEFT   = 1;
	/** 中央寄せ */
	public static final int CENTER = 2;
	/** 右寄せ */
	public static final int RIGHT  = 3;

	/** 表示位置（左寄せ、中央寄せ、右寄せ）*/
	private int align;
	/** 表示文字列 */
	private String text;
	
	/** デフォルトコンストラクタ */
	public Label() {
		this.align = Label.LEFT;
		this.text = "Label";
	}
	
	/**
	 コンストラクタ.
	 描画位置は ALIGN_LEFT。
	 @param text ラベル
	 */
	public Label(String text) {
		this.align = Label.LEFT;
		this.text = text;
	}
	
	/**
	 コンストラクタ
	 @param text ラベル
	 @param align 描画位置 (ALIGN_LEFT / ALIGN_CENTER / ALIGN_RIGHT)
	 */
	public Label(String text, int align) {
		this.align = align;
		this.text = text;
	}
	
	/**
	 テキスト設定
	 @param text
	 */
	public void setText(String text) {
		this.text = text;
		repaint();
	}
	
	/** テキストを得る */
	public String getText() { return this.text; }
	
	/** 描画ハンドラ */
	public void paint(Graphics g) {
		int w = getWidth();
		int h = getHeight();
		
		// 塗りつぶし
		g.setColor(getBackground());
		g.fillRect(0, 0, w, h);

		// 文字
		int fw = getFontMetrics().getWidth(getText());
		int fh = getFontMetrics().getHeight(getText());
		if (getEnabled() == true) {
			g.setColor(getForeground());
		} else {
			g.setColor(Color.gray);
		}
		if (this.align == Label.RIGHT) {
			g.drawString(getText(), (w - fw), (h - fh) / 2);
		} else if (this.align == Label.CENTER) {
			g.drawString(getText(), (w - fw) / 2, (h - fh) / 2);
		} else {
			g.drawString(getText(), 0, (h - fh) / 2);
		}
	}
}
