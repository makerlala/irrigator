#pragma once

#include <memory>
#include <string>

#include "engine/alice/alice_codelet.hpp"
#include "messages/messages.hpp"

namespace isaac {

namespace irrigator {

class Driver : public isaac::alice::Codelet {
 public:
  Driver();
  ~Driver();
  void start() override;
  void tick() override;
  void stop() override;
  
  ISAAC_PROTO_RX(PingProto, result);

  ISAAC_PROTO_TX(PingProto, command);

 private:
  int _state = 0;
};

} // namespace irrigator

} // namespace isaac

ISAAC_ALICE_REGISTER_CODELET(isaac::irrigator::Driver);
