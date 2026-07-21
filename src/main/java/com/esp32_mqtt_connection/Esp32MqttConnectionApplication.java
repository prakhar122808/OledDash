package com.esp32_mqtt_connection;

import org.springframework.boot.WebApplicationType;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.builder.SpringApplicationBuilder;

@SpringBootApplication
public class Esp32MqttConnectionApplication {

    public static void main(String[] args) {
        new SpringApplicationBuilder(Esp32MqttConnectionApplication.class)
                .web(WebApplicationType.NONE)
                .run(args);
    }
}
