#include "utils.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPE1_TABLES_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H

#include <fontconfig/fontconfig.h>

#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QFontDatabase>
#include <glib.h>

QString Utils::getQssContent(const QString &filePath)
{
    QFile file(filePath);
    QString qss;

    if (file.open(QIODevice::ReadOnly)) {
        qss = file.readAll();
    }

    return qss;
}

bool Utils::fontIsExists(const QString &fontName)
{
    const QStringList families = getAllFontName();

    for (const QString &name : families) {
        if (name == fontName)
            return true;
    }

    return false;
}

bool Utils::suffixIsFont(const QString &suffix)
{
    if (suffix == "ttf" || suffix == "ttc" || suffix == "otf") {
        return true;
    } else {
        return false;
    }
}

QString Utils::suffixList()
{
    return QString("Font Files (*.ttf *.ttc *.otf)");
}

QString Utils::getFontType(const QString &suffix)
{
    if (suffix == "ttf" || suffix == "ttc") {
        return "TrueType";
    } else if (suffix == "otf") {
        return "OpenType";
    } else {
        return "Unknow";
    }
}

QString Utils::getSfntName(const QString &filePath)
{
    FT_Library library = 0;
    FT_Face face = 0;
    FT_Error error = FT_Err_Ok;
    error = FT_Init_FreeType(&library);

    if (!error) {
        error = FT_New_Face(library, filePath.toLatin1().data(), 0, &face);
    }

    return QString::fromLatin1(face->family_name);
}

QStringList Utils::getFontName(const QString &filePath)
{
    QStringList data;        // save font name and style name.
    FT_Library library = 0;  // handle to library
    FT_Face face = 0;        // handle to face object
    FT_Error error = FT_Err_Ok;
    error = FT_Init_FreeType(&library);

    if (!error) {
        error = FT_New_Face(library, filePath.toLatin1().data(), 0, &face);
    }

    data << QString::fromLatin1(face->family_name);
    data << QString::fromLatin1(face->style_name);

    free(library);
    free(face);

    return data;
}

void Utils::getFontInfo(const QString &filePath, QString &familyName, QString &styleName, QString &type, QString &version, QString &copyright, QString &description)
{
    FT_Library m_library = 0;
    FT_Face face = 0;
    FT_Error error = FT_Err_Ok;
    error = FT_Init_FreeType(&m_library);

    if (!error) {
        error = FT_New_Face(m_library, filePath.toLatin1().data(), 0, &face);
    }

    familyName = QString::fromLatin1(face->family_name);
    styleName = QString::fromLatin1(face->style_name);

    const int len = FT_Get_Sfnt_Name_Count(face);
    for (int i = 0; i < len; ++i) {
        FT_SfntName sname;
        if (FT_Get_Sfnt_Name(face, i, &sname) != 0)
            continue;

        // only handle the unicode names for US langid.
        if (!(sname.platform_id == TT_PLATFORM_MICROSOFT &&
              sname.encoding_id == TT_MS_ID_UNICODE_CS &&
              sname.language_id == TT_MS_LANGID_ENGLISH_UNITED_STATES))
            continue;

        QString str = nullptr;
        for(int i  = 0; i != sname.string_len; ++i){
            char ch = static_cast<char>(sname.string[i]);
            str.push_back(ch);
        }

        switch (sname.name_id) {
        case TT_NAME_ID_COPYRIGHT:
            copyright = g_convert((char *)sname.string, sname.string_len, "UTF-8", "UTF-16BE", NULL, NULL, NULL);
            break;
        case TT_NAME_ID_VERSION_STRING:
            version = g_convert((char *)sname.string, sname.string_len, "UTF-8", "UTF-16BE", NULL, NULL, NULL);
            break;
        case TT_NAME_ID_DESCRIPTION:
            description = g_convert((char *)sname.string, sname.string_len, "UTF-8", "UTF-16BE", NULL, NULL, NULL);
            break;
        }
    }

    free(m_library);
    free(face);
}

void Utils::fontInstall(const QStringList &files)
{
    QProcess *process = new QProcess;
    QString cmd = "pkexec cp -r ";
    for (auto const file : files) {
        cmd.append(file + " ");
    }
    cmd.append("/usr/share/fonts/");

    qDebug() << cmd;

    process->start(cmd);
    process->waitForFinished();
    process->kill();
    process->close();
}

QStringList Utils::getAllFontName()
{
    QStringList families;
    FcConfig *config = FcConfigGetCurrent();
    FcStrList *strList = FcConfigGetFontDirs(config);
    FcChar8 *path;

    while ((path = FcStrListNext(strList)) != NULL) {
        const QString pathStr = QString::fromLatin1((char *)path);
        const QDir dir(pathStr);
        const QFileInfoList infoList = dir.entryInfoList(QDir::Files);

        for (const QFileInfo &info : infoList) {
            const QString filePath = info.absoluteFilePath();

            // filter other font files.
            if (suffixIsFont(info.suffix())) {
                families << getSfntName(info.absoluteFilePath());
            }
        }
    }

    FcStrListDone(strList);
    return families;
}
