#include "electra_rc3.h"
#include "electra_rc3_protocol.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome
{
    namespace electra_rc3
    {
        // using climate::ClimateMode;
        // using climate::ClimateFanMode;
        using remote_base::ElectraRC3Data;
        using remote_base::ElectraRC3Fan;
        using remote_base::ElectraRC3Mode;

        static const char *const TAG = "electra_rc3.climate";

        // void ElectraRC3IR::control(const climate::ClimateCall &call) {
        //   // swing and preset resets after unit powered off
        //   if (call.get_mode().has_value() && (climate::CLIMATE_MODE_OFF == call.get_mode())) {
        //     this->current_mode = call.get_mode();
        //   }

        //   climate_ir::ClimateIR::control(call);
        // }

        void ElectraRC3::transmit_state()
        {
            ElectraRC3Data data;

            switch (this->fan_mode.value())
            {
            case climate::CLIMATE_FAN_LOW:
                data.fan = ElectraRC3Fan::FAN_LOW;
                break;
            case climate::CLIMATE_FAN_MEDIUM:
                data.fan = ElectraRC3Fan::FAN_MEDIUM;
                break;
            case climate::CLIMATE_FAN_HIGH:
                data.fan = ElectraRC3Fan::FAN_HIGH;
                break;
            default:
                data.fan = ElectraRC3Fan::FAN_AUTO;
                break;
            }

            switch (this->mode)
            {
            case climate::CLIMATE_MODE_COOL:
                data.mode = ElectraRC3Mode::MODE_COOL;
                break;
            case climate::CLIMATE_MODE_HEAT:
                data.mode = ElectraRC3Mode::MODE_HEAT;
                break;
            case climate::CLIMATE_MODE_AUTO:
                data.mode = ElectraRC3Mode::MODE_AUTO;
                break;
            case climate::CLIMATE_MODE_DRY:
                data.mode = ElectraRC3Mode::MODE_DRY;
                break;
            case climate::CLIMATE_MODE_FAN_ONLY:
                data.mode = ElectraRC3Mode::MODE_FAN;
                break;
            case climate::CLIMATE_MODE_OFF:
            default:
                data.mode = ElectraRC3Mode::MODE_OFF;
                break;
            }

            if (climate::CLIMATE_MODE_OFF != this->mode)
            {
                data.power = (climate::CLIMATE_MODE_OFF == this->current_mode_) ? 1 : 0;
            }
            this->current_mode_ = this->mode;

            auto temp = (uint8_t)roundf(this->target_temperature);

            if (temp < TEMP_MIN)
            {
                temp = TEMP_MIN;
            }
            else if (temp > TEMP_MAX)
            {
                temp = TEMP_MAX;
            }
            data.temperature = temp - 15;

            data.sleep = (this->preset == climate::CLIMATE_PRESET_SLEEP) ? 1 : 0;
            ;
            data.swing = (this->swing_mode == climate::CLIMATE_SWING_VERTICAL) ? 1 : 0;

            ESP_LOGD(TAG, "Electra RC3:");
            ESP_LOGD(TAG, "  sleep:       0x%X", data.sleep);
            ESP_LOGD(TAG, "  temperature: 0x%X", data.temperature);
            ESP_LOGD(TAG, "  ifeel:       0x%X", data.ifeel);
            ESP_LOGD(TAG, "  swing:       0x%X", data.swing);
            ESP_LOGD(TAG, "  fan:         0x%X", data.fan);
            ESP_LOGD(TAG, "  mode:        0x%X", data.mode);
            ESP_LOGD(TAG, "  power:       0x%X", data.power);


            auto transmit = this->transmitter_->transmit();
            remote_base::ElectraRC3Protocol().encode(transmit.get_data(), data);
            transmit.perform();
        }

        bool ElectraRC3::on_receive(remote_base::RemoteReceiveData data) { return false; }

    } // namespace electra_rc3
} // namespace esphome