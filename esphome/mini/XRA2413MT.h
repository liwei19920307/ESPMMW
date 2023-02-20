#include "esphome.h"

class XRA2413MT : public Component, public UARTDevice
{
public:
	XRA2413MT(UARTComponent *parent) : UARTDevice(parent)
	{
	}

	Sensor *max_move_distance_sensor = new Sensor();
	Sensor *max_static_distance_sensor = new Sensor();
	Sensor *unattended_duration_sensor = new Sensor();
	Sensor *move_sensitivity_sensor = new Sensor();
	Sensor *static_sensitivity_sensor = new Sensor();

	uint8_t conf8[2];
	uint16_t conf16;

	uint8_t data8[2];
	uint16_t data16;

	std::vector<uint8_t> bytes;

	void loop() override
	{
		while (available() > 0)
		{
			bytes.push_back(read());

			switch (bytes[0])
			{
			case 0xFD:
				if (bytes.size() < 8)
				{
					continue;
				}

				if (bytes.size() >= 8 && bytes.size() < 34)
				{
					if (bytes[6] != 0x61 || bytes[7] != 0x01)
					{
						std::vector<uint8_t>().swap(bytes);
					}
					continue;
				}

				if (bytes.size() == 34)
				{
					max_move_distance_sensor->publish_state(bytes[12]);

					max_static_distance_sensor->publish_state(bytes[13]);

					move_sensitivity_sensor->publish_state(bytes[14]);

					static_sensitivity_sensor->publish_state(bytes[23]);

					conf8[0] = bytes[28];
					conf8[1] = bytes[29];
					conf16 = *(uint16_t *)(conf8);
					unattended_duration_sensor->publish_state(conf16);
				}
				std::vector<uint8_t>().swap(bytes);
				break;
			default:
				std::vector<uint8_t>().swap(bytes);
				continue;
			}
		}
	}
};
