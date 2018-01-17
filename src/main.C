#include <iostream>
#include <boost/program_options.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/thread/thread.hpp>

#include <iostream>
#include <thread>
#include <vector>

#include "config.h"

namespace po = boost::program_options;
namespace ba=boost::asio;
namespace bs=boost::system;

typedef boost::shared_ptr<ba::ip::tcp::socket> socket_ptr;
typedef boost::shared_ptr<ba::io_service> io_service_ptr;

class connetion
{
	struct hide_me {};
public:
	typedef boost::shared_ptr<connection> pointer;
	connection(ba::io_service& io_service, hide_me);

	static pointer 
	create(ba::io_service& io_service) 
	{
		return boost::make_shared<connection>(boost::ref(io_service), hide_me());
	}

	ba::ip::tcp::socket& 
	socket() 
	{
		return socket_;
	}

	void run();

private:
	ba::io_service& io_service_;
	ba::ip::tcp::socket socket_;
	ba::streambuf buf;
	const static std::string message_;
};

const std::string connection::message_="HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"
	"<html><head><title>test</title>"
	"</head><body><h1>Test</h1><p>This is a test!</p></body></html>";

connection::connection(ba::io_service& io_service, hide_me) :
	io_service_(io_service), socket_(io_service) i
{
}

void
connection::run()
{
	try {
		ba::read_until(socket_, buf, boot::regex("\r\n\r\n"));
		ba::write(socket, ba::buffer(message_), ba::transfer_all());
		socket_.close();
	} catch(std::exception& x) {
		std::cerr << "Exception: " << x.what() << std::endl;
	}
}

class server : private boost::noncopyable
{
public:
	server(
		ba::io_service& io_service_accept, 
		ba::io_service& io_service_execute,
		unsigned int thread_num_acceptors,
		unsigned int thread_num_executors,
		unsigned int port = 10001,
		std::string interface_address = ""
	);
	~server();

private:
	void handle_accept(connection::pointer old_connection, const boost::system::error_code& e);

	ba::io_service& io_service_acceptors_;  /**< reference to io_service */
	ba::io_service& io_service_executors_;  /**< reference to io_service */
	ba::io_service::work work_acceptors_;   /**< object to inform the io_service_acceptors_ when it has work to do */
	ba::io_service::work work_executors_;   /**< object to inform the io_service_executors_ when it has work to do */
	std::vector<boost::thread> thr_grp_acceptors_;  /**< thread pool object for acceptors */
	std::vector<boost::thread> thr_grp_executors_;  /**< thread pool object for executors */
	const ba::ip::tcp::endpoint endpoint_;   /**< object, that points to the connection endpoint */
	ba::ip::tcp::acceptor acceptor_;	
};

server::server(
	ba::io_service& io_service_acceptors,
	ba::io_service& io_service_executors,
	unsigned int thread_num_acceptors,
	unsigned int thread_num_executors,
	unsigned int port,
	std::string interface_address
)
: io_service_acceptors_(io_service_acceptors),
  io_service_executors_(io_service_executors),
  work_acceptors_(io_service_acceptors_),
  work_executors_(io_service_executors_),
  endpoint_(interface_address.empty()?
  	(ba::ip::tcp::endpoint(ba::ip::tcp::v4(), port)):
  	ba::ip::tcp::endpoint(ba::ip::address().from_string(interface_address), port)
  ),
  acceptor_(io_service_acceptors_, endpoint_)
{
	std::cout << endpoint_.address().to_string() << ":" << endoint_.port() << std::endl;

	// Create executor thread pool
	//
	for (size_t i = 0; i < thread_num_acceptors; i ++) {
		thr_grp_executors_.emplace_back(	
}

class printer
{
public:
	printer(boost::asio::io_service &io)
	: timer_(io, boost::posix_time::seconds(1)),
	  count_(0)
	{
		timer_.async_wait(boost::bind(&printer::print, this));
	}

	~printer()
	{
		std::cout << "Final count is " << count_ << std::endl;
	}

	void
	print()
	{
		if (count_ < 5) {
			std::cout << count_ << std::endl;
			++count_;
			timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(1));
			timer_.async_wait(boost::bind(&printer::print, this));
		}
	}

private:
	boost::asio::deadline_timer timer_;
	int count_;
};
 
int
main(int argc, char *argv[])
{
	try {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("compression", po::value<double>(), "set compression level")
		;
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 0;
		}

		if (vm.count("compression")) {
			std::cout << "Compression level was set to " << vm["compression"].as<double>() << ".\n";
		} else {
			std::cout << "Compression level was not set.\n";
		}
	} catch (const po::error &e) {
		std::cerr << "error: " << e.what() << "\n";
		return 1;
	} catch (...) {
		std::cerr << "Unknown error\n";
		return 1;
	}

	std::cout << "hardware threads: " << std::thread::hardware_concurrency() << std::endl;

	boost::asio::io_service io;
	printer p(io);
	io.run();

	return 0;
}

