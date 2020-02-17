load("//engine/build:isaac.bzl", "isaac_app", "isaac_cc_module")

isaac_app(
     name = "irrigator",
	modules = ["//packages/irrigator:irrigator_components"]
)

isaac_cc_module(
  name = "irrigator_components",
  srcs = ["Driver.cpp", "Detector.cpp"],
  hdrs = ["Driver.hpp", "Detector.hpp"],
)
