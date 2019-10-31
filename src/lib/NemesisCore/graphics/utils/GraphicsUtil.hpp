#ifndef GraphicsUtilH
#define GraphicsUtilH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
//
//     This program is free software; you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation; either version 2 of the License, or
//     (at your option) any later version.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License along
//     with this program; if not, write to the Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// =============================================================================

#include <NemesisCoreMainHeader.hpp>
#include <Point.hpp>
#include <ElementColors.hpp>
#include <SimpleList.hpp>

// -----------------------------------------------------------------------------

/// sphere

class NEMESIS_CORE_PACKAGE CSphere {
public:
    CSphere(void);   // default tessellation=3
    ~CSphere(void);

    void Draw(const float radius);
    bool SetTessellationQuality(const unsigned int quality);

// section of private data ----------------------------------------------------
private:
    CSimplePoint<float>* Vertices;      // points
    unsigned int         NumOfVertices; // number of verticies
    unsigned int         Tessellation;  // quality
    unsigned int         DummyPos;

    void ComputeVertices(void);
    void ComputePartition(const CSimplePoint<float>& v1,
                          const CSimplePoint<float>& v2,
                          const CSimplePoint<float>& v3,
                          CSimplePoint<float> data[],
                          const unsigned int complexity);
};

// -----------------------------------------------------------------------------

/// half sphere

class NEMESIS_CORE_PACKAGE CHalfSphere {
public:
    CHalfSphere(void);   // default tessellation=3
    ~CHalfSphere(void);

    void Draw(const float radius);
    bool SetTessellationQuality(const unsigned int quality);

// section of private data ----------------------------------------------------
private:
    CSimplePoint<float>* Vertices;      // points
    unsigned int         NumOfVertices; // number of verticies
    unsigned int         Tessellation;  // quality
    unsigned int         DummyPos;

    void ComputeVertices(void);
    void ComputePartition(const CSimplePoint<float>& v1,
                          const CSimplePoint<float>& v2,
                          const CSimplePoint<float>& v3,
                          CSimplePoint<float> data[],
                          const unsigned int complexity);
};

// -----------------------------------------------------------------------------

/// cylinder

class NEMESIS_CORE_PACKAGE CCylinder {
public:
    CCylinder(void);   // default tessellation=3
    ~CCylinder(void);

    void Draw(const float radius,
              const float height);
    void Draw(const float radius,
              const float height,
              const CColor* color1,
              const CColor* color2);
    void DrawWithMaterialColors(const float radius,
                                const float height,
                                const CElementColors* color1,
                                const CElementColors* color2);
    void DrawWithUniformColors(const float radius,
                               const float height,
                               const CElementColors* color1,
                               const CElementColors* color2);
    void Draw(const float radius1,
              const float radius2,
              const float height);

    bool SetTessellationQuality(const unsigned int quality);

// section of private data ----------------------------------------------------
private:
    CSimplePoint<float>* Vertices;      // points
    unsigned int         NumOfFaces;    // number of verticies
    unsigned int         Tessellation;  // quality

    void ComputeVertices(void);
};

// -----------------------------------------------------------------------------

/// arc

class NEMESIS_CORE_PACKAGE CArc {
public:
    CArc(void);   // default tessellation=3
    ~CArc(void);

    void Draw(const CSimplePoint<float>& v1,
              const CSimplePoint<float>& v2,
              const CSimplePoint<float>& v3,
              float angle,
              CSimplePoint<float>& str);
    bool SetTessellationQuality(const unsigned int quality);

// section of private data ----------------------------------------------------
private:
    CSimplePoint<float>* Vertices;      // points
    unsigned int         NumOfFaces;    // number of verticies
    unsigned int         Tessellation;  // quality

    void ComputeVertices(void);
};

// -----------------------------------------------------------------------------

/// OpenGL 2D font

class NEMESIS_CORE_PACKAGE COpenGL2DFont {
public:
    COpenGL2DFont(void);
    ~COpenGL2DFont(void);

    /*    bool CreateFont(TFont* p_font,TWinControl* p_win);*/
    bool IsInitialized(void);
    void DestroyFont(void);

    void DrawText(const QString& text);

// section of private data ----------------------------------------------------
private:
    int   FirstGlyph;
    int   NumOfGlyph;
    int   ListBase;
};

// -----------------------------------------------------------------------------

#endif

