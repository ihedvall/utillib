

#pragma once

#include <array>
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "listenmessage.h"
#include "util/ilistenclient.h"

namespace util::log::detail {

class ListenClient : public IListenClient {
 public:
  ListenClient(const std::string &host_name, uint16_t port);

  ~ListenClient() override;

  ListenClient() = delete;

  ListenClient(const ListenClient &) = delete;

  ListenClient &operator=(const ListenClient &) = delete;

  [[nodiscard]] bool GetMsg(std::unique_ptr<ListenMessage> &message) override;

  void SendLogLevel(uint64_t level) override;

 private:
  boost::asio::io_context context_;
  boost::asio::ip::tcp::resolver lookup_;
  boost::asio::steady_timer retry_timer_;
  std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
  boost::asio::ip::tcp::resolver::results_type endpoints_;
  boost::asio::ip::tcp::resolver::results_type::const_iterator current_endpoint_;

  std::array<uint8_t, 8> header_data_{0};
  std::vector<uint8_t> body_data_;
  ThreadSafeQueue<ListenMessage> msg_queue_;

  std::thread worker_thread_;

  void WorkerTask();

  void Close();

  void DoLookup();

  void DoConnect();

  void DoRetryWait();

  void DoReadHeader();

  void DoReadBody();

  void HandleMessage();
};

}  // namespace util::log::detail
