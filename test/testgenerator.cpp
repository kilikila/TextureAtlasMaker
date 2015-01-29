
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "generator.hpp"

#include <QFileInfo>
#include <QFile>
#include <QImage>
#include <QJsonDocument>
#include <QJsonArray>

namespace {
    
    void Cleanup(const QString& file)
    {
        QFile::remove(file);
        QFile::remove(DescriptionFilename(file));
    }

}

TEST_CASE("generated texture has the desired filename","[functional],[texture]")
{
    QString testfile = "testfile.png";
    Cleanup(testfile);
    generateTexture(testfile,128,{});
    REQUIRE(QFileInfo::exists(testfile)==true);
}

TEST_CASE("generated texture is png file","[functional],[texture]")
{
    QString testfile("testfile.png");
    Cleanup(testfile);
    generateTexture(testfile,128,{});
    QImage image(testfile,"PNG");
    REQUIRE(image.isNull() == false);
}

TEST_CASE("generated texture has the desired size","[functional],[texture]")
{
    QString testfile("testfile.png");
    Cleanup(testfile);
    unsigned int width = 128;
    generateTexture(testfile, width,{});
    QImage image(testfile,"PNG");
    CHECK(image.width()==width);
    CHECK(image.height()==width);
}

TEST_CASE("generator throws IllegalArgument if file already exists","[functional][generator]")
{
    QString testfile("testfile.png");
    Cleanup(testfile);
    unsigned int width = 128;
    generateTexture(testfile, width,{});
    CHECK_THROWS_AS(generateTexture(testfile, width,{}), InvalidArgument);
}

TEST_CASE("generator generates description file","[functional][description]")
{
    QString testfile("testfile.png");
    Cleanup(testfile);
    generateTexture(testfile, 128,{});
    REQUIRE(QFileInfo::exists(DescriptionFilename(testfile)));
}

TEST_CASE("generated description file is json","[functional],[description]")
{
    QString testfile("testfile.png");
    Cleanup(testfile);
    generateTexture(testfile, 128,{});
    QFile descriptionFile(DescriptionFilename(testfile));
    descriptionFile.open(QFile::ReadOnly);
    auto data = descriptionFile.readAll();
    auto doc = QJsonDocument::fromJson(data);
    REQUIRE(doc.isNull()==false);
}

TEST_CASE("generated description file contains one entry per texture in atlas","[functional][description]")
{
    QString testfile("testfile.png");
    Cleanup(testfile);
    const int numTextures = 10;
    std::vector<std::tuple<QString,QImage>> textures;
    for (int i=0; i<numTextures; ++i)
    {
        textures.push_back(std::make_tuple<QString,QImage>("test",QImage(64,64,QImage::Format_ARGB32)));
    }
    generateTexture(testfile, 128, textures);
    QFile tmp(DescriptionFilename(testfile));
    tmp.open(QFile::ReadOnly);
    auto description = QJsonDocument::fromJson(tmp.readAll());
    CHECK(description.isNull()==false);
    auto textureDescriptions = description.array();
    CHECK(textureDescriptions.count()==numTextures);
}