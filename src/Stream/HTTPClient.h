#ifndef STREAM_HTTP_CLIENT_H_
#define STREAM_HTTP_CLIENT_H_


#include "StreamClient.h"

class HTTPClient : public StreamClient {
    Q_OBJECT
public:
    HTTPClient(const QString& URL);

    void process() override;
};

#endif // STREAM_HTTP_CLIENT_H_