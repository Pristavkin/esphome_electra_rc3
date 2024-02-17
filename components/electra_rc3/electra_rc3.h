#pragma once

#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome
{
    namespace electra_rc3
    {

        // Temperature
        const uint8_t TEMP_MIN = 16;
        const uint8_t TEMP_MAX = 30;
        const bool SUPPORTS_DRY = true;
        const bool SUPPORTS_FAN_ONLY = true;

        class ElectraRC3 : public climate_ir::ClimateIR
        {
        public:
            ElectraRC3()
                : climate_ir::ClimateIR(TEMP_MIN, TEMP_MAX, 1.0f, SUPPORTS_DRY, SUPPORTS_FAN_ONLY,
                                        {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH},
                                        {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL},
                                        {climate::CLIMATE_PRESET_NONE, climate::CLIMATE_PRESET_SLEEP}) {}

            // void control(const climate::ClimateCall &call) override;

        protected:
            /// Transmit via IR the state of this climate controller.
            void transmit_state() override;
            /// Handle received IR Buffer
            bool on_receive(remote_base::RemoteReceiveData data) override;

        private:
            climate::ClimateMode current_mode_{climate::ClimateMode::CLIMATE_MODE_OFF};
        };

    } // namespace electra_rc3
} // namespace esphome