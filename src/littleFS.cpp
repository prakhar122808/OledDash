#include "header.h"

void startLittleFS()
{
    if (!LittleFS.begin(false))
    {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
    Serial.println("LittleFS mounted successfully");
}

String loadFile(const char *path)
{
    File file = LittleFS.open(path, "r");
    if (!file)
    {
        Serial.println("Unable to open the file.");
        return "";
    }

    String content = file.readString();
    file.close();
    return content;
}

void loadCerts()
{
    rootCA = loadFile("/AmazonRootCA1.pem");
    deviceCert = loadFile("/certificate.pem.crt");
    privateKey = loadFile("/private.pem.key");
}