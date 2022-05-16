#include "ClientParameters.h"


namespace po = boost::program_options;

// A helper function to simplify the main part.
template<class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
	copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
	return os;
}

void exit_program(int status) {
	if (status) {
		std::cerr << "Consider using -h [--help] command" << std::endl;
	}
	exit(status);
}

void split_ip(std::string &ip, std::string &address, std::string &port) {
	size_t pos = ip.find_last_of(':');
	if (pos == std::string::npos) {
		return;
	} else {
		address = ip.substr(0, pos);
		port = ip.substr(pos + 1);
	}
}


static inline void check_port(int possible_port) {
	if (possible_port < 0 || possible_port > 65535) {
		std::cerr << "Invalid port" << std::endl;
		throw po::validation_error(po::validation_error::invalid_option_value);
	}
}
//todo może trzeba spradzić, czy porty takie same
static inline void check_address(const std::string &address) {
	if (address.empty()) {
		throw po::validation_error(po::validation_error::invalid_option_value);
	}
	boost::system::error_code ec;
	boost::asio::ip::address::from_string(address, ec);
	if (ec) {
		std::cerr << "Invalid ip address" << std::endl;
		throw po::validation_error(po::validation_error::invalid_option_value);
	}
}

static inline void check_ip(std::string ip) {
	int possible_port;
	std::string address, port;
	split_ip(ip, address, port);
	check_address(address);
	try {
		possible_port = boost::lexical_cast<int>(port);
	} catch (boost::bad_lexical_cast &) {
		std::cerr << "Invalid ip port" << std::endl;
		throw po::validation_error(po::validation_error::invalid_option_value);
	}
	check_port(possible_port);
}


void ClientParameters::check_parameters() {
	int possible_port = -1;
	const po::positional_options_description p; // empty positional options
	po::options_description desc("Program Usage", 1024, 512);
	try {
		desc.add_options()
				("help,h", "produce help message")
				("gui-address,d",
				 po::value<std::string>(&display_address)->required()->notifier(
						 &check_ip),
				 "set the gui address")
				("player-name,n",
				 po::value<std::string>(&player_name)->required(),
				 "set the player name")
				("port,p", po::value<int>(&possible_port)->required()->notifier(
						 &check_port),
				 "set the port number")
				("server-address,s",
				 po::value<std::string>(&server_address)->required()->notifier(
						 &check_ip),
				 "set the server address");

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).
				options(desc).
				positional(p).
				run(), vm);

		if (vm.count("help")) {
			std::cout << desc << "\n";
			exit_program(0);
		}

		po::notify(vm);
	}
	catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << "\n";
		exit_program(1);
	}
	catch (...) {
		std::cerr << "Unknown error!" << "\n";
		exit_program(1);
	}
	port = (uint16_t) possible_port;
}
//todo: czy program może przyjmować więcej losowych parametrów?
//todo: czy --gui-address może być skracane do postaci --gui?