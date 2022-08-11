#include "esphome.h"

class LD2410 : public Component, public UARTDevice
{
public:
	LD2410(UARTComponent *parent) : UARTDevice(parent)
	{
	}

	Sensor *max_move_distance_sensor = new Sensor();
	Sensor *max_static_distance_sensor = new Sensor();
	Sensor *unattended_duration_sensor = new Sensor();
	Sensor *move_sensitivity_sensor = new Sensor();
	Sensor *static_sensitivity_sensor = new Sensor();

	Sensor *status_sensor = new Sensor();
	Sensor *move_distance_sensor = new Sensor();
	Sensor *move_energy_sensor = new Sensor();
	Sensor *static_distance_sensor = new Sensor();
	Sensor *static_energy_sensor = new Sensor();
	Sensor *detection_distance_sensor = new Sensor();

	uint8_t status;
	uint16_t move_distance;
	uint8_t move_energy;
	uint16_t static_distance;
	uint8_t static_energy;
	uint16_t detection_distance;

	uint8_t conf8[2];
	uint16_t conf16;

	uint8_t data8[2];
	uint16_t data16;

	long millis = 0L;
	int refresh_millisecond = 500;

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

				if (bytes.size() >= 8 && bytes.size() < 38)
				{
					if (bytes[6] != 0x61 || bytes[7] != 0x01)
					{
						std::vector<uint8_t>().swap(bytes);
					}
					continue;
				}

				if (bytes.size() == 38)
				{
					max_move_distance_sensor->publish_state(bytes[12]);

					max_static_distance_sensor->publish_state(bytes[13]);

					move_sensitivity_sensor->publish_state(bytes[14]);

					static_sensitivity_sensor->publish_state(bytes[23]);

					conf8[0] = bytes[32];
					conf8[1] = bytes[33];
					conf16 = *(uint16_t *)(conf8);
					unattended_duration_sensor->publish_state(conf16);
				}
				std::vector<uint8_t>().swap(bytes);
				break;
			case 0xF4:
				if (bytes.size() < 8)
				{
					continue;
				}

				if (bytes.size() >= 8 && bytes.size() < 23)
				{
					if (bytes[6] != 0x02 || bytes[7] != 0xAA)
					{
						std::vector<uint8_t>().swap(bytes);
					}
					continue;
				}

				if (bytes.size() == 23)
				{

					if (millis() - millis < refresh_millisecond)
					{
						std::vector<uint8_t>().swap(bytes);
						break;
					}
					else
					{
						millis = millis();
					}

					if (status != bytes[8])
					{
						status = bytes[8];
						status_sensor->publish_state(status);
					}

					data8[0] = bytes[9];
					data8[1] = bytes[10];
					data16 = *(uint16_t *)(data8);
					if (move_distance != data16)
					{
						move_distance = data16;
						move_distance_sensor->publish_state(move_distance);
					}

					if (move_energy != bytes[11])
					{
						move_energy = bytes[11];
						move_energy_sensor->publish_state(move_energy);
					}

					data8[0] = bytes[12];
					data8[1] = bytes[13];
					data16 = *(uint16_t *)(data8);
					if (static_distance != data16)
					{
						static_distance = data16;
						static_distance_sensor->publish_state(static_distance);
					}

					if (static_energy != bytes[14])
					{
						static_energy = bytes[14];
						static_energy_sensor->publish_state(static_energy);
					}

					data8[0] = bytes[15];
					data8[1] = bytes[16];
					data16 = *(uint16_t *)(data8);
					if (detection_distance != data16)
					{
						detection_distance = data16;
						detection_distance_sensor->publish_state(detection_distance);
					}
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