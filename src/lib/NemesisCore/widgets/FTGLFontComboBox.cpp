// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <FTGLFontComboBox.hpp>
#include <ErrorSystem.hpp>
#include <DirectoryEnum.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QFTGLFontComboBox::QFTGLFontComboBox(QWidget* p_parent)
    : QComboBox(p_parent)
{
}

//------------------------------------------------------------------------------

void QFTGLFontComboBox::setFontPath(const QString& path)
{
    clear();

    QStringList     fonts;
    CDirectoryEnum  enum_dir;

    enum_dir.SetDirName(CFileName(path));
    enum_dir.StartFindFile("*.ttf");

    CFileName file;

    while( enum_dir.FindFile(file) ){
        CFileName text = file.GetFileNameWithoutExt();
        fonts << QString(text);
    }

    enum_dir.EndFindFile();

    fonts.sort();
    addItems(fonts);
}

//------------------------------------------------------------------------------

void QFTGLFontComboBox::setFontName(const QString& name)
{
    int index = findText(name);
    setCurrentIndex(index);
}

//------------------------------------------------------------------------------

QString QFTGLFontComboBox::getFontName(void) const
{
    if( currentText().isEmpty() ){
        return("LiberationSans-Regular");
    }
    return(currentText());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
