#include <QCoreApplication>
#include <QFile>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>

struct ErrorInfo
{
    unsigned int id;
    std::string text;
};

void Help()
{
    std::cout<<"Xml2JSON"<<std::endl;
    std::cout<<"Usage: Xml2JSON source_file dest_file"<<std::endl;
}

void ParseElement(QXmlStreamReader &reader,QJsonObject &json_object)
{
    auto attributes=reader.attributes();

    for(auto i=0;i<attributes.size();i++)
    {
        auto &attr=attributes[i];

        json_object.insert(attr.name(),QJsonValue(attr.value().toString()));
    }

    for(;;)
    {
       auto token_type=reader.readNext();

       if(token_type==QXmlStreamReader::StartElement)
       {
           QString name=reader.name().toString();
           QJsonObject object;

           ParseElement(reader,object);

           if(json_object.find(name)!=json_object.end())
           {
               QJsonValue value=json_object.value(name);

               if(value.isArray())
               {
                   QJsonArray json_array=value.toArray();
                   json_array.append(object);
                   json_object.insert(name,json_array);
               }
               else
               {
                   QJsonArray json_array;

                   json_array.append(value);
                   json_array.append(object);
                   json_object.insert(name,json_array);
               }
           }
           else
               json_object.insert(name,object);
       }
       else if(token_type==QXmlStreamReader::EndElement ||
               token_type==QXmlStreamReader::EndDocument)
           break;
    }
}

void Convert(const QString &source_file,const QString &dest_file)
{
    std::cout<<"Processing..."<<std::endl;

    QFile xml_file(source_file);
    QFile json_file(dest_file);

    if(!xml_file.open(QIODevice::ReadOnly))
    {
        ErrorInfo error;

        error.id=1;
        error.text="Open xml file failed!";

        throw error;
    }

    if(!json_file.open(QIODevice::WriteOnly))
    {
        ErrorInfo error;

        error.id=2;
        error.text="Open json file failed!";

        throw error;
    }

    QXmlStreamReader reader(&xml_file);

    QJsonDocument json_doc;
    QJsonObject json_object;

    ParseElement(reader,json_object);
    json_doc.setObject(json_object);

    json_file.write(json_doc.toJson());
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(argc!=3)
    {
        Help();
        return 1;
    }
    else
    {
        try{
            Convert(argv[1],argv[2]);
        }
        catch (ErrorInfo error) {
            std::cout<<"Error:"<<error.text<<std::endl;
            return 1;
        }
    }

    return 0;
}
