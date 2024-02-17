#pragma once

#include "esphome/components/remote_base/remote_base.h"

namespace esphome
{
    namespace remote_base
    {
        // Electra RC3 configuration has a total of 34 bits, repeated 3 times
        //     0: Zero
        //     1: One
        //  2-17: Zeros
        //    18: Sleep mode On/Off
        // 19-22: Temperature, where 15 is 0000, 30 is 1111
        //    23: Zero
        //    24: iFeel On/Off
        //    25: Swing On/Off
        // 26-27: Zeros
        // 28-29: Fan - Low, medium etc.
        // 30-32: Mode - Cool, heat etc.

        //    33: Power bit, if this bit is ON, the A/C will toggle it's power.

        enum ElectraRC3Mode {
            MODE_COOL = 0b001,
            MODE_HEAT = 0b010,
            MODE_AUTO = 0b011,
            MODE_DRY = 0b100,
            MODE_FAN = 0b101,
            MODE_OFF = 0b111
        };

        enum ElectraRC3Fan {
            FAN_LOW = 0b00,
            FAN_MEDIUM = 0b01,
            FAN_HIGH = 0b10,
            FAN_AUTO = 0b11
        };

        struct ElectraRC3Data
        {
            union
            {
                uint64_t q_word;
                struct
                {
                    uint64_t zeros1 : 1;
                    uint64_t ones1 : 1;
                    uint64_t zeros2 : 16;
                    uint64_t sleep : 1;
                    uint64_t temperature : 4;
                    uint64_t zeros3 : 1;
                    uint64_t ifeel : 1;
                    uint64_t swing : 1;
                    uint64_t zeros4 : 2;
                    uint64_t fan : 2;
                    uint64_t mode : 3;
                    uint64_t power : 1;
                };
            };

            ElectraRC3Data() : q_word(0) { ones1 = 1; }

            bool operator==(const ElectraRC3Data &rhs) const { return (q_word == rhs.q_word); }
        };

        class ElectraRC3Protocol : public RemoteProtocol<ElectraRC3Data>
        {
        public:
            void encode(RemoteTransmitData *dst, const ElectraRC3Data &data) override;
            optional<ElectraRC3Data> decode(RemoteReceiveData src) override;
            void dump(const ElectraRC3Data &data) override;
        };

        DECLARE_REMOTE_PROTOCOL(ElectraRC3)

        template <typename... Ts>
        class ElectraRC3Action : public RemoteTransmitterActionBase<Ts...>
        {
        public:
            TEMPLATABLE_VALUE(uint64_t, power)
            TEMPLATABLE_VALUE(uint64_t, mode)
            TEMPLATABLE_VALUE(uint64_t, fan)
            TEMPLATABLE_VALUE(uint64_t, swing)
            TEMPLATABLE_VALUE(uint64_t, ifeel)
            TEMPLATABLE_VALUE(uint64_t, temperature)
            TEMPLATABLE_VALUE(uint64_t, sleep)

            void encode(RemoteTransmitData *dst, Ts... x)
            {
                ElectraRC3Data data{};
                data.mode = 0;
                data.power = this->power_.value(x...);
                data.mode = this->mode_.value(x...);
                data.fan = this->fan_.value(x...);
                data.swing = this->swing_.value(x...);
                data.ifeel = this->ifeel_.value(x...);
                data.temperature = this->temperature_.value(x...);
                data.sleep = this->sleep_.value(x...);
                ElectraRC3Protocol().encode(dst, data);
            }
        };

    } // namespace remote_base
} // namespace esphome