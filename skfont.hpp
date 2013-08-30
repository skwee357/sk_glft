/**
* @file skfont.hpp
* @author skwee
* @brief Declaration of SkFont class, a class based on SDL_ttf and Freetype
* lib to render text in OpenGL.
*/

#ifndef _SKFONT_HPP_
#define _SKFONT_HPP_

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <SDL/SDL_ttf.h>

namespace sk{

  const int cMaxChars = 256; /**< We can have maximum 256 characters. */

  /** Font Info struct. */
  typedef struct{
    int height;
    int ascent;
    int descent;
    int lineSkip;
  }FontInfo_t;

  /**
  * SkFont class.
  * Initialize and perfrom text rendering.
  */
  class SkFont{
    public:
      /**
      * Constructor.
      * Initialize font.
      * @param fontFile Path to font.
      * @param size Font's size.
      * @param border Border size (default 0 - no border).
      */
      SkFont(const char* fontFile, int size, int border = 0);

      /**
      * Destructor.
      * Clean data.
      */
      ~SkFont();

      /**
      * Out Text.
      * Print text on screen.
      * @note Function overloaded.
      * @param x x-coord.
      * @param y y-coord.
      * @param text Text to print.
      */
      void outText(const char* text, ...) const;
      void outText(int x, int y, const char* text, ...) const;

      /**
      * Get Line Skip.
      * @return Amount of pixel to skip to next line.
      */
      int getLineSkip() const;

    private:
      /**
      * Print one line of text.
      */
      void printOneLine(const char* text, int lenght) const;

      /**
      * Build glyph metrics.
      * @param font Font to generate glyph metrics for.
      */
      void buildGlyphMetrics(TTF_Font* font);

      /**
      * Build Font map.
      * @param font Font to build map for.
      */
      void buildFontMap(TTF_Font* font);

      /**
      * Build Display lists for each letter.
      */
      void buildDisplayLists();

      GLuint _textureFontId; /**< Texture Id. */
      GLuint _displayList; /**< Display list. */

      int _textureWidth; /**< Texture Width. */
      int _textureHeight; /**< Texture Height. */

      FontInfo_t _fontInfo; /**< Font Information. */

      int _border; /**< Font border. */

      int _advance[cMaxChars]; /**< Store advance for each character. */
      int _maxAdvance; /**< Maximum advnace value. */

      static int _nInstances; /**< Number of instances. */
  };

};

#endif // _SKFONT_HPP_
