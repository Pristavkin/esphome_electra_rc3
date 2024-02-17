#include "electra_rc3_protocol.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace remote_base
    {
        static const char *const ELECTRA_RC3_TAG = "remote.electra_rc3";

        static const char *TAG = "DEBPROTO";

        static const uint16_t CARRIER_FREQ = 33000;
        static const uint16_t UNIT_MSEC = 1000;
        static const uint16_t HEADER_MARK = (3 * UNIT_MSEC);
        static const uint16_t HEADER_SPACE = (3 * UNIT_MSEC);
        static const uint16_t FOOTER_MARK = (4 * UNIT_MSEC);
        static const uint16_t PACKET_BITS = 34;
        static const uint16_t PACKET_REPEATS = 3;

        void ElectraRC3Protocol::encode(RemoteTransmitData *dst, const ElectraRC3Data &data)
        {
            dst->reserve(44);
            dst->set_carrier_frequency(CARRIER_FREQ);

            for (uint8_t repeat = 0; repeat < PACKET_REPEATS; ++repeat)
            {
                // Encode header
                dst->mark(HEADER_MARK);
                uint16_t next_value = HEADER_SPACE;
                bool is_next_space = true;

                // Encode Data
                for (int bit_index = PACKET_BITS - 1; bit_index >= 0; --bit_index)
                {
                    uint8_t bit = (data.q_word >> bit_index) & 1;

                    if (is_next_space)
                    // one is one unit low, then one unit up
                    // since we're pointing at SPACE, we should increase it by a unit
                    // then add another MARK unit
                    {
                        if (bit == 1)
                        {
                            dst->space(next_value + UNIT_MSEC);
                            next_value = UNIT_MSEC;
                            is_next_space = false;
                        }
                        else
                        {
                            dst->space(next_value);
                            dst->mark(UNIT_MSEC);
                            next_value = UNIT_MSEC;
                            is_next_space = true;
                        }
                    }
                    else
                    {
                        if (bit == 1)
                        {
                            dst->mark(next_value);
                            dst->space(UNIT_MSEC);
                            next_value = UNIT_MSEC;
                            is_next_space = false;
                        }
                        else
                        {
                            dst->mark(next_value + UNIT_MSEC);
                            next_value = UNIT_MSEC;
                            is_next_space = true;
                        }
                    }
                }
                dst->space(next_value);
            }

            // Encode footer
            dst->mark(FOOTER_MARK);
        }

        optional<ElectraRC3Data> ElectraRC3Protocol::decode(RemoteReceiveData src)
        {
            ElectraRC3Data data;

            return data;
        }

        void ElectraRC3Protocol::dump(const ElectraRC3Data &data)
        {
            ESP_LOGD(ELECTRA_RC3_TAG,
                     "Received Electra RC3: power = 0x%X, mode = 0x%X, fan = 0x%X, swing = 0x%X, ifeel = 0x%X, temperature = "
                     "0x%X, sleep = 0x%X",
                     data.power, data.mode, data.fan, data.swing, data.ifeel, data.temperature, data.sleep);
        }

    } // namespace remote_base
} // namespace esphome