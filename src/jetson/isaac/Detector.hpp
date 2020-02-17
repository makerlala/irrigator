#pragma once

#include <memory>
#include <string>

#include "engine/alice/alice_codelet.hpp"
#include "messages/messages.hpp"

namespace isaac {

namespace irrigator {

class Detector : public isaac::alice::Codelet {
 public:
  Detector();
  ~Detector();
  void start() override;
  void tick() override;
  void stop() override;
  
  ISAAC_PROTO_RX(PingProto, command);

  ISAAC_PROTO_TX(PingProto, result); 
};

} // namespace irrigator

} // namespace isaac

ISAAC_ALICE_REGISTER_CODELET(isaac::irrigator::Detector);
