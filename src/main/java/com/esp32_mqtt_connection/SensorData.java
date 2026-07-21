package com.esp32_mqtt_connection;

import lombok.Data;

@Data
public class SensorData {
    private double actualTemp;
    private double feelsLikeTemp;
    private long timestamp;
}
