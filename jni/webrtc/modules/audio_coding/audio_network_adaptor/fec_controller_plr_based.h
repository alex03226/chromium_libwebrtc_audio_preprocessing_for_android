/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_FEC_CONTROLLER_PLR_BASED_H_
#define WEBRTC_MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_FEC_CONTROLLER_PLR_BASED_H_

#include <memory>

#include "webrtc/base/constructormagic.h"
#include "webrtc/common_audio/smoothing_filter.h"
#include "webrtc/modules/audio_coding/audio_network_adaptor/controller.h"
#include "webrtc/modules/audio_coding/audio_network_adaptor/util/threshold_curve.h"

namespace webrtc {

class FecControllerPlrBased final : public Controller {
 public:
  struct Config {
    // |fec_enabling_threshold| defines a curve, above which FEC should be
    // enabled. |fec_disabling_threshold| defines a curve, under which FEC
    // should be disabled. See below
    //
    // packet-loss ^   |  |
    //             |   |  |   FEC
    //             |    \  \   ON
    //             | FEC \  \_______ fec_enabling_threshold
    //             | OFF  \_________ fec_disabling_threshold
    //             |-----------------> bandwidth
    Config(bool initial_fec_enabled,
           const ThresholdCurve& fec_enabling_threshold,
           const ThresholdCurve& fec_disabling_threshold,
           int time_constant_ms);
    bool initial_fec_enabled;
    ThresholdCurve fec_enabling_threshold;
    ThresholdCurve fec_disabling_threshold;
    int time_constant_ms;
  };

  // Dependency injection for testing.
  FecControllerPlrBased(const Config& config,
                        std::unique_ptr<SmoothingFilter> smoothing_filter);

  explicit FecControllerPlrBased(const Config& config);

  ~FecControllerPlrBased() override;

  void UpdateNetworkMetrics(const NetworkMetrics& network_metrics) override;

  void MakeDecision(AudioEncoderRuntimeConfig* config) override;

 private:
  bool FecEnablingDecision(const rtc::Optional<float>& packet_loss) const;
  bool FecDisablingDecision(const rtc::Optional<float>& packet_loss) const;

  const Config config_;
  bool fec_enabled_;
  rtc::Optional<int> uplink_bandwidth_bps_;
  const std::unique_ptr<SmoothingFilter> packet_loss_smoother_;

  RTC_DISALLOW_COPY_AND_ASSIGN(FecControllerPlrBased);
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_FEC_CONTROLLER_PLR_BASED_H_
