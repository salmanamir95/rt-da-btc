#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdio> 
#include <memory>

#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>

// Include your object architecture headers here
#include "EventMeta.h"
#include "BinanceFactory.h"

#include "run_ingestor.h"

using json = nlohmann::json;

void run_ingestor(std::string url, EventQueue& queue)
{
   
    ix::WebSocket ws;
    ws.setUrl(url);

    ix::SocketTLSOptions tlsOptions;
    tlsOptions.tls = true; 
    ws.setTLSOptions(tlsOptions);

    ws.setPingInterval(20); 

    ws.setOnMessageCallback(
        [&queue](const ix::WebSocketMessagePtr& msg)
        {
            if (msg->type == ix::WebSocketMessageType::Open)
            {
                std::cout << "[+] Handshake OK! Direct Raw Pipeline Established." << std::endl;
                std::cout << "[+] Streaming real-time 1-Minute updates..." << std::endl;
            }
            else if (msg->type == ix::WebSocketMessageType::Message)
            {
                try 
                {
                    // 1. Parse the incoming string frame safely into a JSON object
                    auto jsonPayload = json::parse(msg->str);

                    // 2. Feed it to the Factory to construct the specialized class instance
                    std::unique_ptr<EventMeta> event = BinanceFactory::createEvent(jsonPayload);

                    // 3. Polymorphic execution! Triggers specialized logic depending on if it's Kline or B0
                    if (event) {
                        event->process();
                        queue.push(std::move(event));
                    }
                    
                }
                catch (const json::parse_error& e) 
                {
                    std::cerr << "[-] JSON Parse Error: " << e.what() << std::endl;
                }
                catch (const std::exception& e) 
                {
                    std::cerr << "[-] Exception during processing: " << e.what() << std::endl;
                }
            }
            else if (msg->type == ix::WebSocketMessageType::Error)
            {
                std::cerr << "[-] Error: " << msg->errorInfo.reason << std::endl;
            }
            else if (msg->type == ix::WebSocketMessageType::Close)
            {
                std::cout << "[-] Disconnected: " << msg->closeInfo.reason << std::endl;
            }
        });

    std::cout << "[+] Launching Live Ingestor..." << std::endl;
    ws.start();

    // Main execution keep-alive loop
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    ws.stop();
}