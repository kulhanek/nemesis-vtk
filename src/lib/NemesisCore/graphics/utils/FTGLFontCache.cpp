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

#include <FTGLFontCache.hpp>
#include <GlobalSetup.hpp>

//------------------------------------------------------------------------------

CFTGLFontCache  FTGLFontCache;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFTGLFontCache::CFTGLFontCache(void)
{

}

//------------------------------------------------------------------------------

FTPolygonFont* CFTGLFontCache::GetFont(const QString& name,int size)
{
    // do we have active context?
    if( QGLContext::currentContext() == NULL ) return(NULL);

    // is font already cached
    FTPolygonFontPtr font_ptr;
    font_ptr = Cache[QGLContext::currentContext()][name][size];
    if( font_ptr != NULL ){
        return(font_ptr.get());
    }

    // create new font
    CFileName full_name = GlobalSetup->GetFontPath() / name + ".ttf";
    font_ptr = FTPolygonFontPtr(new FTPolygonFont(full_name));
    if( ! font_ptr->Error() ){
        font_ptr->CharMap(ft_encoding_unicode);
        font_ptr->FaceSize(size);

        // put it to cache
        Cache[QGLContext::currentContext()][name][size] = font_ptr;
        return(font_ptr.get());
    } else {
        return(NULL); // unable to load the font
    }
}

//------------------------------------------------------------------------------

void CFTGLFontCache::DestroyFonts(void)
{
    // do we have active context?
    if( QGLContext::currentContext() == NULL ) return;

    // remove all cached fonts for given context
    Cache.remove(QGLContext::currentContext());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

