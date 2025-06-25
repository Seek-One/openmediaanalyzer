#ifndef STREAM_RTSP_CLIENT_H_
#define STREAM_RTSP_CLIENT_H_


#include "StreamClient.h"

class RTSPClient : public StreamClient {
    Q_OBJECT
public: 
    RTSPClient(const QString& URL);

    void process() override;
    
};

#endif // STREAM_RTSP_CLIENT_H_