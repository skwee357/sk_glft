/**
* @file skfont.hpp
* @author skwee
* @brief Realization of SkFont class, a class based on SDL_ttf and Freetype
* lib to render text in OpenGL.
*/

#include <iostream>
#include <cstdarg>

#include "skfont.hpp"

namespace sk{

  int SkFont::_nInstances = 0;

  //Ctor
  SkFont::SkFont(const char* fontFile, int size, int border):
    _textureFontId(0), _displayList(0), _textureWidth(0), _textureHeight(0),
    _border(border), _maxAdvance(0)
  {
    _fontInfo.height = 0;
    _fontInfo.ascent = 0;
    _fontInfo.descent = 0;
    _fontInfo.lineSkip = 0;

    //Init TTF if was not inited
    if((!TTF_WasInit()) && (TTF_Init() < 0))
    {
      //error
      return;
    }

    _nInstances++;

    //Open Font
    TTF_Font* font = TTF_OpenFont(fontFile, size);
    if(!font)
    {
      //error
      return;
    }

    //Build Font information
    buildGlyphMetrics(font);
    buildFontMap(font);
    buildDisplayLists();

    //Close font
    TTF_CloseFont(font);
  }

  //Dtor
  SkFont::~SkFont()
  {
    //Delete texture
    if(glIsTexture(_textureFontId))
      glDeleteTextures(1, &_textureFontId);

    //Delete disply lists
    glDeleteLists(_displayList, cMaxChars);

    _nInstances--;

    if(_nInstances == 0)
      TTF_Quit();
  }

  //outtext
  void SkFont::outText(const char* text, ...) const
  {
    char buffer[1024];
    memset(buffer, '\0', 1024);
    std::va_list arg;

    va_start(arg, text);
      std::vsnprintf(buffer, 1024, text, arg);
    va_end(arg);

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureFontId);

    char* ptr = buffer;

    do{
      int len;
      char* end = strchr(ptr, '\n');

      if(!end)
        len = strlen(ptr);
      else
      {
        end++;
        len = end - ptr - 1;
      }

      glPushMatrix();
        printOneLine(ptr, len);
      glPopMatrix();

      glTranslatef(0.0f, -_fontInfo.lineSkip, 0.0f);

      ptr = end;

    }while(ptr);

    glPopAttrib();
  }
  void SkFont::outText(int x, int y, const char* text, ...) const
  {
    //Set text position
    glLoadIdentity();
    glTranslatef(x, y, 0.0f);

    //format text
    char buffer[1024];
    std::va_list arg;

    va_start(arg, text);
      std::vsnprintf(buffer, 1024, text, arg);
    va_end(arg);

    outText(buffer);
  }

  //Get Line Skip
  int SkFont::getLineSkip() const
  {
    return _fontInfo.lineSkip;
  }

  //Print One Line
  void SkFont::printOneLine(const char* text, int lenght) const
  {
    glListBase(_displayList);
    glCallLists(lenght, GL_UNSIGNED_BYTE, text);
  }

  //Build Glyph Metrics
  void SkFont::buildGlyphMetrics(TTF_Font* font)
  {
    memset(_advance, 0, cMaxChars);
    _maxAdvance = 0;

    for(int i = 0; i < cMaxChars; ++i)
    {
      int dummy, advance;
      TTF_GlyphMetrics(font, static_cast<Uint16>(i), &dummy, &dummy, &dummy,
                       &dummy, &advance);
      _advance[i] = advance;

      if(advance > _maxAdvance)
        _maxAdvance = advance;
    }

    //Font height = fontAcsent - fontDescent + 1
    _fontInfo.height = TTF_FontHeight(font);
    _fontInfo.ascent = TTF_FontAscent(font);
    _fontInfo.descent = TTF_FontDescent(font);
    _fontInfo.lineSkip = TTF_FontLineSkip(font);
  }

  //Build font map
  void SkFont::buildFontMap(TTF_Font* font)
  {
    int width = (_maxAdvance + _border) * 16 + _border;
    int height = (_fontInfo.height + _border) * 16 + _border;

    _textureWidth = 1;
    _textureHeight = 1;

    //Find the minimum width and height in power of two
    while(_textureHeight < height)
      _textureHeight = _textureHeight << 1;

    while(_textureWidth < width)
      _textureWidth = _textureWidth << 1;

    //Create black texture with transparent alpha channel to store our font
    GLubyte* texels = new GLubyte[_textureHeight * _textureWidth * 4];
    memset(texels, 0, _textureHeight * _textureWidth * 4);

    glGenTextures(1, &_textureFontId);
    glBindTexture(GL_TEXTURE_2D, _textureFontId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, _textureWidth, _textureHeight, 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, texels);

    //we dont need it anymore
    delete []texels;

    int xOffset = _border;
    int yOffset = _border;

    //Insert each character into texture
    for(int i = 0; i < 16; ++i)
    {
      for(int j = 0; j < 16; ++j)
      {
        char letter[2];
        letter[0] = static_cast<char>(i * 16 + j);
        letter[1] = '\0';

        if(letter[0] != 0)
        {
          //Foreground white with full alpha
          SDL_Color fg = {255, 255, 255, 255};
          SDL_Surface* sLetter = TTF_RenderText_Blended(font, letter, fg);

          if(sLetter)
          {
            //Copy letter into texture
            glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, sLetter->w,
                            sLetter->h, GL_RGBA, GL_UNSIGNED_BYTE, sLetter->pixels);

            //Dont needed anymore
            SDL_FreeSurface(sLetter);
          }
        }

        xOffset += _maxAdvance + _border;
      }

      xOffset = _border;
      yOffset += _fontInfo.height + _border;
    }
  }

  //Build Display lists
  void SkFont::buildDisplayLists()
  {
    _displayList = glGenLists(cMaxChars);

    GLfloat oneOverTexWidth = 1.0f / static_cast<GLfloat>(_textureWidth);
    GLfloat oneOverTexHeight = 1.0f / static_cast<GLfloat>(_textureHeight);

    //S,T coordinates
    GLfloat s = _border * oneOverTexWidth;
    GLfloat t = _border * oneOverTexHeight;

    GLfloat tOffset = _fontInfo.ascent * oneOverTexHeight;

    for(int i = 0, c = 0; i < 16; ++i)
    {
      for(int j = 0; j < 16; ++j, ++c)
      {
        GLfloat sOffset = _advance[c] * oneOverTexWidth;

        glNewList(_displayList + c, GL_COMPILE);
        glBegin(GL_QUADS);
          glTexCoord2f(s, t + tOffset - (_fontInfo.descent * oneOverTexHeight));
          glVertex2i(0, _fontInfo.descent);

          glTexCoord2f(s + sOffset, t + tOffset - (_fontInfo.descent * oneOverTexHeight));
          glVertex2i(_advance[c], _fontInfo.descent);

          glTexCoord2f(s + sOffset, t);
          glVertex2i(_advance[c], _fontInfo.ascent);

          glTexCoord2f(s, t);
          glVertex2i(0, _fontInfo.ascent);
        glEnd();

        glTranslatef(_advance[c], 0.0f, 0.0f);
        glEndList();

        s += (_maxAdvance + _border) * oneOverTexWidth;
      }
      s = _border * oneOverTexWidth;
      t += (_fontInfo.height + _border) * oneOverTexHeight;
    }
  }

};
