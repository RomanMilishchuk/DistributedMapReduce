// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include "Node.h"

Node::Node(const Node &node) {
    this->session.optionsCopy(node.session);
    this->is_connected = node.is_connected;
}

Node::Node(const std::string& node_name) : session(), is_connected(false) {
    int verbosity = SSH_LOG_NOLOG;
    session.setOption(SSH_OPTIONS_HOST, node_name.c_str());
    session.setOption(SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    session.setOption(SSH_OPTIONS_USER, "midren");
};

Node::~Node() {
    if (is_connected) {
        session.disconnect();
    }
}

void Node::connect() {
    session.connect();
    is_connected = true;
    if (session.isServerKnown() != SSH_SERVER_KNOWN_OK) {
        if (session.writeKnownhost() != SSH_OK) {
            std::cerr << "writeKnownHost failed" << std::endl;
        } else {
            session.connect();
        }
    }
    if (session.userauthPublickeyAuto() != SSH_AUTH_SUCCESS) {
        throw std::runtime_error("Didn't auth");
    }
}


std::string Node::execute_command(const std::string &cmd, bool is_output) {
    ssh::Channel channel(session);
    channel.openSession();
    channel.requestExec(cmd.c_str());
    std::string ret;
    if (is_output) {
        std::array<char, 1024> buffer{};
        int nbytes = channel.read(buffer.data(), buffer.max_size(), true, -1);
        while (nbytes > 0) {
            ret += std::string(buffer.data(), nbytes);
            nbytes = channel.read(buffer.data(), buffer.max_size(), true, -1);
        }
    }
    channel.sendEof();
    channel.close();
    return ret;
}

void Node::scp_write_file(const std::filesystem::path& path_to_file, const std::string &text) {
    Scp scp(session, SSH_SCP_WRITE, path_to_file.parent_path());
    scp.push_file(path_to_file.filename().c_str(), text.length(), S_IRUSR | S_IWUSR);
    scp.write(text);
}

std::string Node::scp_read_file(const std::filesystem::path& path_to_file) {
    Scp scp(session, SSH_SCP_READ, path_to_file.c_str());

    if (scp.pull_request() != SSH_SCP_REQUEST_NEWFILE) {
        throw std::runtime_error("Invalid request from scp");
    }

    scp.accept_request();
    scp.pull_request();
    std::string file = scp.read();

    return file;
}

void Node::scp_send_file(const std::filesystem::path& from, const std::filesystem::path& to) {
    std::ifstream input(from, std::ifstream::binary);
    if (input.is_open()) {
        std::string data = dynamic_cast<std::ostringstream &>(std::ostringstream{} << input.rdbuf()).str();
        Node::scp_write_file(to, data);
    }
    input.close();
};

void Node::scp_download_file(const std::filesystem::path& from, const std::filesystem::path& to) {
    std::string input = scp_read_file(from);
    std::ofstream(to, std::ios::binary).write(input.c_str(), input.length());
}


