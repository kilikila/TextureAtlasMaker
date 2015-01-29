#include "generator.hpp"

#include <QFileInfo>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <set>

QJsonObject FileInformation(const QString& filename, int tileSize);

void generateTexture(const QString& filename, unsigned int widthAndHeight, unsigned int tileSize, const std::vector<std::tuple<QString, QImage>>& textures)
{
    if (QFileInfo::exists(filename))
    {
        throw InvalidArgument("file does already exist");
    }
    //check for duplicate descriptions
    std::set<QString> keys;
    for (const auto& i : textures)
    {
        if (keys.find(std::get<0>(i))==keys.end())
        {
            keys.insert(std::get<0>(i));
        }
        else
        {
            throw std::logic_error("multiple textures with same id detected");
        }
    }
    QImage texture(widthAndHeight,widthAndHeight,QImage::Format_ARGB32);
    texture.save(filename,"PNG");
    QFile descriptionFile(DescriptionFilename(filename));
    descriptionFile.open(QFile::WriteOnly);
    QJsonObject description;
    description["fileinfo"]=FileInformation(filename, tileSize);
    QJsonArray textureDescriptions;
    for (const auto& i : textures)
    {
        QJsonObject textureJson;
        textureJson["ID"] = std::get<0>(i);
        textureDescriptions.push_back(textureJson);
    }
    description["textures"]=textureDescriptions;
    QJsonDocument descriptionDoc(description);
    descriptionFile.write(descriptionDoc.toJson());
}

QJsonObject FileInformation(const QString& filename, int tileSize)
{
    QJsonObject result;
    result["version"]=1;
    result["texturefile"]=filename;
    result["tilesize"]=tileSize;
    return result;
}

QString DescriptionFilename(const QString& file)
{
    QFileInfo fi(file);
    return fi.baseName()+".json";
}