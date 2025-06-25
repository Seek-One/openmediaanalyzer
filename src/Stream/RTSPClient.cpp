#include <curl/curl.h>

#include "RTSPClient.h"

RTSPClient::RTSPClient(const QString& URL):
    StreamClient(URL)
{}

void RTSPClient::process(){
    
}