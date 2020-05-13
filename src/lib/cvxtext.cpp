#include <wchar.h>
#include <assert.h>
#include <locale.h>
#include <ctype.h>
#include <cmath>
#include "lib/logger.h"
#include "cvxtext.h"
#include <signal.h>

static int get_utf8_size(unsigned char ch) {
    unsigned char c = ch;
    // 0xxxxxxx 返回0
    // 10xxxxxx 不存在
    // 110xxxxx 返回2
    // 1110xxxx 返回3
    // 11110xxx 返回4
    // 111110xx 返回5
    // 1111110x 返回6
    if(c< 0x80) return 0;
    if(c>=0x80 && c<0xC0) return -1;
    if(c>=0xC0 && c<0xE0) return 2;
    if(c>=0xE0 && c<0xF0) return 3;
    if(c>=0xF0 && c<0xF8) return 4;
    if(c>=0xF8 && c<0xFC) return 5;
    if(c>=0xFC) return 6;
    return -1;
}


static int utf8_to_unicode_one(const unsigned char *pInput, unsigned long *pUnicode) {
    assert(pInput != NULL && pUnicode != NULL);
    // b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...
    char b1, b2, b3, b4, b5, b6;
    int utfbytes = get_utf8_size(*pInput);
    *pUnicode = 0;
    unsigned char *pOutput = (unsigned char *)pUnicode;
    switch (utfbytes) {
        case 0:
            *pOutput = *pInput;
            *(pOutput + 1) = 0x00;
            utfbytes = 1;
            break;
        case 2:
            b1 = *pInput;
            b2 = *(pInput + 1);
            if ((b2 & 0xE0) != 0x80)
                return 0;
            *pOutput     = (b1 << 6) + (b2 & 0x3F);
            *(pOutput+1) = (b1 >> 2) & 0x07;
            break;

        case 3:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            if (((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80))
                return 0;
            *pOutput = (b2 << 6) + (b3 & 0x3F);
            *(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);
            break;

        case 4:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            b4 = *(pInput + 3);
            if (((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) || ((b4 & 0xC0) != 0x80))
                return 0;
            *pOutput = (b3 << 6) + (b4 & 0x3F);
            *(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F);
            *(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);
            break;

        case 5:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            b4 = *(pInput + 3);
            b5 = *(pInput + 4);
            if (((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80))
                return 0;
            *pOutput     = (b4 << 6) + (b5 & 0x3F);
            *(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F);
            *(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03);
            *(pOutput+3) = (b1 << 6);
            break;

        case 6:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            b4 = *(pInput + 3);
            b5 = *(pInput + 4);
            b6 = *(pInput + 5);
            if (((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)
                    || ((b6 & 0xC0) != 0x80))
                return 0;
            *pOutput  = (b5 << 6) + (b6 & 0x3F);
            *(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F);
            *(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03);
            *(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F);
            break;

        default:
            return 0;
            break;
    }
    return utfbytes;
}

static int utf8_to_unicode(const char *pInBuf, int inLen, char *pOutbuf) {
    assert(pInBuf != NULL && pOutbuf != NULL);
    int inOffset = 0; 
    int outOffset = 0;
	int retLen = 0;
    while(inOffset < inLen) {
        int tmpOutLen = 0;
        int tmpInLen = utf8_to_unicode_one((const unsigned char *)&pInBuf[inOffset], (unsigned long *)&pOutbuf[outOffset]);
		retLen++;
        inOffset += tmpInLen;
        outOffset += 4;
		if (tmpInLen == 0)
            break;
    }
	return retLen;
}

CvxText *CvxText::getInstance() {
    static CvxText self("Adobe-SongTi-Std-L-2.otf");
    return &self;
}

// 打开字库
CvxText::CvxText(const char* freeType)
{
    assert(freeType != NULL);

    // 打开字库文件, 创建一个字体
    if(FT_Init_FreeType(&m_library)) {
	SZ_LOG_ERROR("FT_Init_FreeType failed! {}", freeType);
	raise(SIGSEGV);
    }
    if(FT_New_Face(m_library, freeType, 0, &m_face)) {    
	SZ_LOG_ERROR("FT_New_Face failed! {}", freeType);
  	raise(SIGSEGV);
    }

    // 设置字体输出参数
    restoreFont();

    // 设置C语言的字符集环境
    //setlocale(LC_ALL, "");
}

// 释放FreeType资源
CvxText::~CvxText()
{
    FT_Done_Face(m_face);
    FT_Done_FreeType(m_library);
}

// 设置字体参数:
//
// font         - 字体类型, 目前不支持
// size         - 字体大小/空白比例/间隔比例/旋转角度
// underline   - 下画线
// diaphaneity   - 透明度
void CvxText::getFont(int *type, cv::Scalar *size, bool *underline, float *diaphaneity)
{
    if (type) *type = m_fontType;
    if (size) *size = m_fontSize;
    if (underline) *underline = m_fontUnderline;
    if (diaphaneity) *diaphaneity = m_fontDiaphaneity;
}

void CvxText::setFont(cv::Size &fontSize)
{
    // 参数合法性检查
    m_fontSize.val[0] = fontSize.width;
    FT_Set_Pixel_Sizes(m_face, (int)m_fontSize.val[0], fontSize.height);
}


// 恢复原始的字体设置
void CvxText::restoreFont() {
    m_fontType = 0;            // 字体类型(不支持)

    m_fontSize.val[0] = 20;      // 字体大小
    m_fontSize.val[1] = 0.5;   // 空白字符大小比例0.5
    m_fontSize.val[2] = 0.1;   // 间隔大小比例
    m_fontSize.val[3] = 0;      // 旋转角度(不支持)
    m_fontUnderline   = false;   // 下画线(不支持)
    m_fontDiaphaneity = 1.0;   // 色彩比例(可产生透明效果)
    // 设置字符大小
    FT_Set_Pixel_Sizes(m_face, (int)m_fontSize.val[0], 0);
}

int CvxText::putText(cv::Mat &img, const char *text, cv::Point pos, cv::Scalar color, cv::Size fontSize) {
    if (fontSize.width > 0) {
        setFont(fontSize);
    }

    int len = strlen(text);
	if (len <= 1)
		return 0;
    wchar_t ucs4Buf[len] = {'\0'};
    len = utf8_to_unicode(text, len, (char *)ucs4Buf);
    return putText(img, (const wchar_t *)ucs4Buf, len, pos, color);
}


int CvxText::putText(cv::Mat& img, const wchar_t* text, int len, cv::Point pos, cv::Scalar color) {
    if (img.data == nullptr) return -1;
    if (text == nullptr) return -1;  

    int i;
    for(i = 0; i < len; ++i) {
        // 输出当前的字符
        putWChar(img, text[i], pos, color);
    }
    return i;
}

// 输出当前字符, 更新m_pos位置
void CvxText::putWChar(cv::Mat& img, wchar_t wc, cv::Point &pos, cv::Scalar &color)
{
    // 根据unicode生成字体的二值位图
    FT_UInt glyph_index = FT_Get_Char_Index(m_face, wc);
    FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT);
    FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_MONO);

    FT_GlyphSlot slot = m_face->glyph;

    // 行列数
    int rows = slot->bitmap.rows;
    int cols = slot->bitmap.width;
    int alignRow = 0;
	if (wc == 'j') {
		alignRow = -rows / 4 + 1;
	}
	else if (wc == 'g' || wc == 'p' || wc == 'q' || wc == 'y') {
		alignRow = -rows / 3 + 1;
	}
    else if (wc >=0 && wc <= 127)
        alignRow = 1;
    else
        alignRow = (m_fontSize.val[0] - rows) / 2;
    for (int i = 0; i < rows; ++i) {
        for(int j = 0; j < cols; ++j) {
            int off  = i * slot->bitmap.pitch + j / 8;
            if (slot->bitmap.buffer[off] & (0xC0 >> (j % 8))) {
                int r = pos.y - (rows - 1 - i) - alignRow;
                int c = pos.x + j;

                if(r >= 0 && r < img.rows && c >= 0 && c < img.cols) {
                    cv::Vec3b pixel = img.at<cv::Vec3b>(cv::Point(c, r));
                    cv::Scalar scalar = cv::Scalar(pixel.val[0], pixel.val[1], pixel.val[2]);

                    // 进行色彩融合
                    float p = m_fontDiaphaneity;
                    for (int k = 0; k < 4; ++k) {
                        scalar.val[k] = scalar.val[k]*(1-p) + color.val[k]*p;
                    }

                    img.at<cv::Vec3b>(cv::Point(c, r))[0] = (unsigned char)(scalar.val[0]);
                    img.at<cv::Vec3b>(cv::Point(c, r))[1] = (unsigned char)(scalar.val[1]);
                    img.at<cv::Vec3b>(cv::Point(c, r))[2] = (unsigned char)(scalar.val[2]);
                }
            }
        }
    }

    // 修改下一个字的输出位置
    double space = m_fontSize.val[0] * m_fontSize.val[1];
    double sep = m_fontSize.val[0] * m_fontSize.val[2];
    pos.x += (int)((cols ? cols: space) + sep);
}
