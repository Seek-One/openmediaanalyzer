#include "StreamClient.h"

StreamClient::StreamClient(const QString& URL):
    m_codec(Codec_Unsupported), m_contentType(ContentType_Other),
    m_URL(URL), m_running(true)
{}

void StreamClient::process(){

}

void StreamClient::streamStopped(){
    m_running = false;
}