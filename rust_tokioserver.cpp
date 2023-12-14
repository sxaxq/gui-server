#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QDebug>
#include <QDateTime>
#include <QThread>
#include <QMenuBar>
#include <QMenu>

#include <boost/asio.hpp>

#include <fstream>
#include <thread>
#include <string>
#include <iostream>
#include <vector>
#include <chrono>

#ifdef DEBUG
#include <Windows.h>
#endif

#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

namespace utils
{
    QString get_current_time()
    {
        const QDateTime& current_date_time = QDateTime::currentDateTime();
        return "[" + current_date_time.toString("hh:mm:ss") + "]: ";
    }
}

namespace console_tools
{
    class console
    {
    public:
        console();
        ~console();
        void write(const std::string& message);
        std::string read_line();
    private:
        FILE* console_output_stream_;
        FILE* console_input_stream_;
    };

    console::console()
    {
        AllocConsole();
        freopen_s(&console_output_stream_, "CONOUT$", "w", stdout);
        freopen_s(&console_input_stream_, "CONIN$", "r", stdin);
    }

    console::~console()
    {
        fclose(console_output_stream_);
        fclose(console_input_stream_);
        FreeConsole();
    }

    void console::write(const std::string& message)
    {
        std::cout << message << std::endl;
    }

    std::string console::read_line()
    {
        return "test";
    }
}

namespace network
{

    class server
    {
    public:
        server(int port = 12345) : port_(port), running_(false), socket_(INVALID_SOCKET) { }
        ~server() {  }

        bool start();
        void restart();
        void stop();

        void set_log_output_pointer(QTextEdit* log_pointer)
        {
            log_output = log_pointer;
        }

        void add_message(const QString& message)
        {
            log_output->moveCursor(QTextCursor::End);
            log_output->insertPlainText(message + "\n");
            log_output->moveCursor(QTextCursor::Start);
        }

    private:
        bool initialize_winsock();
        void cleanup_winsock();
        SOCKET create_socket();
        bool m_bind();
        bool m_listen();
        void accept_connections();
        void handle_client(SOCKET client_socket);
        void close_client_socket(SOCKET client_socket);
        void cleanup_socket();

    private:
        int port_;
        bool running_;
        std::thread accept_thread_;
        std::vector<SOCKET> client_sockets_;
        SOCKET socket_;
        console_tools::console console_;
        QTextEdit* log_output;
        
    };

    bool server::start()
    {
        if (running_)
        {
            return false;
        }

        if (!initialize_winsock())
        {
            return false;
        }

        socket_ = create_socket();
        if (socket_ == INVALID_SOCKET)
        {
            cleanup_winsock();
            return false;
        }

        if (!m_bind())
        {
            cleanup_socket();
            cleanup_winsock();
        }

        if (!m_listen())
        {
            cleanup_socket();
            cleanup_winsock();
            return false;
        }

        running_ = true;
        accept_thread_ = std::thread(&server::accept_connections, this);
        console_.write("Server is started.");
        return true;
    }

    void server::restart()
    {
        stop();
        start();
    }

    void server::stop()
    {
        console_.write("Trying off server");
        if (!running_)
        {
            return;
        }

        running_ = false;
#ifndef TEST
        if (accept_thread_.joinable())
        {
            accept_thread_.join();
        }
#endif

        cleanup_socket();
        cleanup_winsock();
    }

    bool server::initialize_winsock()
    {
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
        {
            std::cout << "Failed to initialize winsock." << std::endl;
            return false;
        }
        else
        {
            console_.write("Sucessfully initialize winsock.");
        }
        return true;
    }

    void server::cleanup_winsock()
    {
        WSACleanup();
    }

    SOCKET server::create_socket()
    {
        SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_socket == INVALID_SOCKET)
        {
            std::cout << "failed to create socket: " << WSAGetLastError() << std::endl;
            return INVALID_SOCKET;
        }
        else
        {
            console_.write("Sucessfully create socket.");
        }
        return listen_socket;
    }

    bool server::m_bind()
    {
        sockaddr_in server_address{};
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_port = htons(port_);

        if (bind(socket_, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) ==
            SOCKET_ERROR)
        {
            std::cout << "failed to bind socket: " << WSAGetLastError() << std::endl;
            return false;
        }
        else
        {
            console_.write("Sucessfully to bind socket.");
        }
        return true;
    }

    bool server::m_listen()
    {
        if (listen(socket_, SOMAXCONN) == SOCKET_ERROR)
        {
            std::cout << "failed to listen on socket: " << WSAGetLastError() << std::endl;
            return false;
        }
        else
        {
            console_.write("Sucessfully listen on socket.");
        }

        return true;
    }

    void server::accept_connections()
    {
        while (running_)
        {
            console_.write("Starting listing clients.");

            if (!running_) {
                break;
            }

            SOCKET client_socket = accept(socket_, nullptr, nullptr);
            if (client_socket == INVALID_SOCKET)
            {
                std::cout << "failed to accept client connection: " << WSAGetLastError() << std::endl;
                continue;
            }
#ifndef DEBUG
            console_.write("New client connection");
            add_message(utils::get_current_time() + "New client connection");
#endif
            client_sockets_.push_back(client_socket);
            std::thread client_thread(&server::handle_client, this, client_socket);
            client_thread.detach();
        }

        console_.write("Closed session listing new clients.");
    }

    void server::handle_client(SOCKET client_socket)
    {
        while (running_)
        {
            char buffer[1024];
            int bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_read > 0)
            {
                std::string message(buffer, bytes_read);
                std::cout << "received from client: " << client_socket << ": " << message
                    << std::endl;

            }
            else if (bytes_read == 0)
            {
                std::cout << "client: " << client_socket << " disconnected." << std::endl;
                close_client_socket(client_socket);
                break;
            }
            else
            {
                std::cout << "Error reading from client " << client_socket << ": "
                    << WSAGetLastError() << std::endl;
                close_client_socket(client_socket);
                break;
            }
        }
    }

    void server::close_client_socket(SOCKET client_socket)
    {
        closesocket(client_socket);
        auto it = std::find(client_sockets_.begin(), client_sockets_.end(), client_socket);
        if (it != client_sockets_.end())
        {
            client_sockets_.erase(it);
        }
    }
   
    void server::cleanup_socket()
    {
        closesocket(socket_);
        for (SOCKET client_socket : client_sockets_)
        {
            closesocket(client_socket);
        }
        client_sockets_.clear();
    }
}

namespace UI
{
    class server_interface : public QWidget {
    public:
        server_interface();

        void add_message(const QString& message);
    private slots:
        void off_server();
        void on_server();
        void restart_server();
        void show_config_dialog();

    private:
        QTextEdit* log_output;
        std::thread server_thread;
        std::thread off_server_thread;

        network::server local_server;


        QMenuBar* menu_bar;
        QMenu* menu_config;
    };

    server_interface::server_interface() 
        : QWidget(nullptr)
    {
        setWindowTitle("NotRustTokioServer");
        setFixedSize(800, 600);

#if 0
        menu_bar = new QMenuBar(this);
        menu_config = new QMenu("Config", this);
        menu_bar->addMenu(menu_config);

        QAction* action_config = new QAction("Open Config", this);
        menu_config->addAction(action_config);
        connect(action_config, &QAction::triggered, this, &server_interface::show_config_dialog);
#endif
        

        QPushButton* red_button = new QPushButton("Off Server", this);
        red_button->setGeometry(600, 20, 160, 40);
        red_button->setStyleSheet("background-color: red; color: white;");
        connect(red_button, &QPushButton::clicked, this, &server_interface::off_server);

        QPushButton* green_button = new QPushButton("On Server", this);
        green_button->setGeometry(600, 80, 160, 40);
        green_button->setStyleSheet("background-color: green; color: white;");
        connect(green_button, &QPushButton::clicked, this, &server_interface::on_server);

        QPushButton* restart_button = new QPushButton("Restart", this);
        restart_button->setGeometry(600, 140, 160, 40);
        restart_button->setStyleSheet("background-color: blue; color: white;");
        connect(restart_button, &QPushButton::clicked, this, &server_interface::restart_server);

        log_output = new QTextEdit(this);
        log_output->setReadOnly(true);
        local_server.set_log_output_pointer(log_output);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(log_output);
        layout->addWidget(red_button);
        layout->addWidget(green_button);
        layout->addWidget(restart_button);
    }

    void server_interface::show_config_dialog()
    {

    }

    void server_interface::off_server()
    {
        off_server_thread = std::thread([&]()
            {
                local_server.stop();
            });

        add_message(utils::get_current_time() + "Server is turned off.");
        off_server_thread.detach();
    }

    void server_interface::on_server()
    {
        server_thread = std::thread([&]()
            {
                local_server.start();
            });

        server_thread.detach();

        add_message(utils::get_current_time() + "Server is turned on.");
    }

    void server_interface::restart_server()
    {
        local_server.restart();
        add_message(utils::get_current_time() + "Server is restarted");
    }

    void server_interface::add_message(const QString& message)
    {
        log_output->moveCursor(QTextCursor::End);
        log_output->insertPlainText(message + "\n");
        log_output->moveCursor(QTextCursor::Start);
    }
}

int main(int argc, char* argv[]) 
{
    QApplication app(argc, argv);

    UI::server_interface program;
    program.show();

    return app.exec();
}