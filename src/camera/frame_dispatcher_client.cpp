#include "camera/frame_dispatcher_client.h"

std::string FrameDispatcherClient::return_msg;

FrameDispatcherClient::FrameDispatcherClient(Logger& logger, const std::string& serverURL) : logger(logger), serverURL(serverURL) {
    ws = easywsclient::WebSocket::from_url(serverURL.c_str());
    if (!ws) {
        logger.error("FAILED TO CREATE WEBSOCKET CONNECTION FOR FRAME DISPATCHER");
    }
}

FrameDispatcherClient::~FrameDispatcherClient() {
    if (ws) {
        ws->close();
        delete ws;
    }
}

void FrameDispatcherClient::sendFrame(const cv::Mat& frame) {
    if (isClosed()) {
        logger.error("FRAME DISPATCHER WEBSOCKET IS CLOSED");
    } else {
        std::vector<uchar> buffer;
        cv::imencode(".jpg", frame, buffer);
        std::string image_data(buffer.begin(), buffer.end());
        ws->sendBinary(image_data);
        pollAndDispatch();
    }
}

bool FrameDispatcherClient::isClosed() {
    return ws && ws->getReadyState() == easywsclient::WebSocket::CLOSED;
}

void FrameDispatcherClient::pollAndDispatch() {
    if (ws) {
        ws->poll();
        ws->dispatch(handleMessage);
    }
}

void FrameDispatcherClient::handleMessage(const std::string& message) {
    return_msg = message;
}
