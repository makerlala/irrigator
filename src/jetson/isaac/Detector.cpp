#include "iostream"
#include "Detector.hpp"

namespace isaac {

namespace irrigator {

Detector::Detector() {}

Detector::~Detector() {}

void Detector::start() {}

void Detector::stop() {}

void Detector::tick() {
	auto proto = rx_command().getProto();
  	const std::string message = proto.getMessage();

	if (message == "detect") {
		std::cout << "Start detection!" << std::endl;
		int ret = system("/home/dumi/git/irrigator/src/jetson/run-all-models.sh");
		auto proto = tx_result().initProto();  		  			
		if ((ret >> 8) == 1) {
			proto.setMessage("yes");
		}
		else {
			proto.setMessage("no");
		}
		tx_result().publish();
	}
}

}

}
