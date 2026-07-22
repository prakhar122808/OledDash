package com.esp32_mqtt_connection;

import org.eclipse.paho.client.mqttv3.*;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.integration.annotation.ServiceActivator;
import org.springframework.integration.channel.DirectChannel;
import org.springframework.integration.core.MessageProducer;
import org.springframework.integration.mqtt.core.DefaultMqttPahoClientFactory;
import org.springframework.integration.mqtt.core.MqttPahoClientFactory;
import org.springframework.integration.mqtt.inbound.MqttPahoMessageDrivenChannelAdapter;
import org.springframework.messaging.Message;
import org.springframework.messaging.MessageChannel;
import org.springframework.messaging.MessageHandler;
import org.springframework.messaging.MessagingException;
import tools.jackson.databind.ObjectMapper;

@Configuration
public class MqttConfig {

    String clientId = "testID";
    @Value("${topic}")
    String topic;
    @Value("${url}")
    String url;
    double lastActualTemp = 0;
    double lastFeelsLikeTemp = 0;

    @Bean
    public MqttPahoClientFactory mqttClientFactory() {
        DefaultMqttPahoClientFactory factory = new DefaultMqttPahoClientFactory();
        MqttConnectOptions options = new MqttConnectOptions();
        options.setServerURIs(new String[] { url });
        options.setAutomaticReconnect(true);
        options.setCleanSession(true);
        options.setConnectionTimeout(10);
        factory.setConnectionOptions(options);
        return factory;
    }

    @Bean
    public MessageChannel mqttInputChannel() {
        return new DirectChannel();
    }

    @Bean
    public MessageProducer inbound() {
        MqttPahoMessageDrivenChannelAdapter adapter = new MqttPahoMessageDrivenChannelAdapter(clientId,
                mqttClientFactory(), topic);
        adapter.setCompletionTimeout(5000);
        adapter.setQos(1);
        adapter.setOutputChannel(mqttInputChannel());
        return adapter;
    }

    @Bean
    @ServiceActivator(inputChannel = "mqttInputChannel")
    public MessageHandler mqttOutputChannel() throws MqttException {
        return new MessageHandler() {
            @Override
            public void handleMessage(Message<?> message) throws MessagingException {
                String payload = (String) message.getPayload();
                SensorData data = new ObjectMapper().readValue(payload, SensorData.class);
                if (lastActualTemp != data.getActualTemp() || lastFeelsLikeTemp != data.getFeelsLikeTemp()) {
                    System.out.println("Actual temperature: " + data.getActualTemp() + "°C");
                    System.out.println("Feels like: " + data.getFeelsLikeTemp() + "°C");
                    lastActualTemp = data.getActualTemp();
                    lastFeelsLikeTemp = data.getFeelsLikeTemp();
                }
            }
        };
    }
}
