#include "ws_client.hpp"

void wait_a_bit()
{
#ifdef WIN32
    Sleep(1000);
#else
    sleep(1);
#endif
}

wsClient::wsClient() : m_open(false), m_done(false)
{
    // set up access channels to only log interesting things
    m_client.clear_access_channels(websocketpp::log::alevel::all);
    m_client.set_access_channels(websocketpp::log::alevel::connect);
    m_client.set_access_channels(websocketpp::log::alevel::disconnect);
    //m_client.set_access_channels(websocketpp::log::alevel::app);
    // Initialize the Asio transport policy
    m_client.init_asio();
    // Bind the handlers we are using
    using websocketpp::lib::bind;
    using websocketpp::lib::placeholders::_1;
    m_client.set_open_handler(bind(&wsClient::on_open, this, _1));
    m_client.set_close_handler(bind(&wsClient::on_close, this, _1));
    m_client.set_fail_handler(bind(&wsClient::on_fail, this, _1));
}

// This method will block until the connection is complete
void wsClient::connect(const std::string &uri)
{
    websocketpp::lib::error_code ec;
    client::connection_ptr con = m_client.get_connection(uri, ec);
    if (ec)
    {
        m_client.get_alog().write(websocketpp::log::alevel::app,
                                  "Get Connection Error: " + ec.message());
        return;
    }
    // Grab a handle for this connection so we can talk to it in a thread
    // safe manor after the event loop starts.
    m_hdl = con->get_handle();
    // Queue the connection. No DNS queries or network connections will be
    // made until the io_service event loop is run.
    m_client.connect(con);
    // Create a thread to run the ASIO io_service event loop
    m_asio_thread = websocketpp::lib::thread(&client::run, &m_client);
}

void wsClient::join_thread() {
    m_asio_thread.join();
}

int wsClient::send_message(const std::string &strmessage)
{
    uint64_t count = 0;
    std::stringstream val;
    websocketpp::lib::error_code ec;
    bool wait = false;
    {
        scoped_lock guard(m_lock);
        // If the connection has been closed, stop generating telemetry
        if (m_done)
        {
            return 0;
        }

        // If the connection hasn't been opened yet wait a bit and retry
        if (!m_open)
        {
            wait = true;
        }
    }
    if (wait)
    {
        wait_a_bit();
        return 0;
    }
    val.str("");
    val << strmessage;
    m_client.get_alog().write(websocketpp::log::alevel::app, val.str());
    m_client.send(m_hdl, val.str(), websocketpp::frame::opcode::text, ec);
    // The most likely error that we will get is that the connection is
    // not in the right state. Usually this means we tried to send a
    // message to a connection that was closed or in the process of
    // closing. While many errors here can be easily recovered from,
    // in this simple example, we'll stop the telemetry loop.
    if (ec)
    {
        m_client.get_alog().write(websocketpp::log::alevel::app,
                                  "Send Error: " + ec.message());
        return 0;
    }
    return 0;
}

// The open handler will signal that we are ready to start sending telemetry
void wsClient::on_open(websocketpp::connection_hdl)
{
    m_client.get_alog().write(websocketpp::log::alevel::app,
                              "Connection opened, starting telemetry!");
    scoped_lock guard(m_lock);
    m_open = true;
}

// The close handler will signal that we should stop sending telemetry
void wsClient::on_close(websocketpp::connection_hdl)
{
    m_client.get_alog().write(websocketpp::log::alevel::app,
                              "Connection closed, stopping telemetry!");
    scoped_lock guard(m_lock);
    m_done = true;
}

// The fail handler will signal that we should stop sending telemetry
void wsClient::on_fail(websocketpp::connection_hdl)
{
    m_client.get_alog().write(websocketpp::log::alevel::app,
                              "Connection failed, stopping telemetry!");
    scoped_lock guard(m_lock);
    m_done = true;
}
