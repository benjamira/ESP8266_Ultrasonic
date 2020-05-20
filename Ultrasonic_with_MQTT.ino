#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>

Ultrasonic ultrasonic(12, 13);

#define wifi_ssid         "<wifi ssid>"
#define wifi_password     "<wifi pass>"

#define mqtt_server       "<mqtt server>"
#define mqtt_clientid     "<unique clieent id>"

#define abstand_topic     "Haus/Garten/Zisterne/Abstand"     //Abstand zwischen Sensor und Oberfläche (cm)
#define wasserstand_topic "Haus/Garten/Zisterne/Wasserstand" //Wasserstand (cm)
#define fuellstand_topic  "Haus/Garten/Zisterne/Fuellstand"  //Füllstand der Zisterne (%)
#define inhalt_topic      "Haus/Garten/Zisterne/Inhalt"      //Füllstand der Zisterne (Liter)

int distanz_leer = 142;  // Distanz zwischen Sensor und Zisternenboden
float wasser_max = 132;  // Max. Wasserstand (Distanz Boden bis Oberkante Siffon)
int gesamtinhalt = 5000; // Maximaler Gesamtinhalt der Zisterne in Litern

WiFiClient espClient;
PubSubClient client(espClient);

void setup() 
{
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
}

void setup_wifi() 
{
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(wifi_ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_password);

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
 }

void reconnect()  
{
    // Loop until we're reconnected
    while (!client.connected()) 
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        // If you do not want to use a username and password, change next line to
        if (client.connect(mqtt_clientid)) 
        //if (client.connect(mqtt_clientid, mqtt_user, mqtt_password)) {
        {
            Serial.println("connected");
        } 
        else 
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void loop() 
{
    if (!client.connected()) 
    {
        reconnect();
    }
    client.loop();
    int Abstand = ultrasonic.distanceRead();
    int Wasserstand = distanz_leer - Abstand;
    int fuellstand = (Wasserstand/wasser_max) * 100;
    int inhalt = (gesamtinhalt/100)*fuellstand;
    /*
      Serial.println("Abstand cm: ");
      Serial.println(Abstand);
      Serial.println("Wasserstand cm: ");
      Serial.println(Wasserstand);
      Serial.println("Füllstand in Prozent: ");
      Serial.println(fuellstand);
      Serial.println("Inhalt in Liter: ");
      Serial.println(inhalt);
    */
    client.publish(abstand_topic, String(Abstand).c_str(), true);
    client.publish(wasserstand_topic, String(Wasserstand).c_str(), true);
    client.publish(fuellstand_topic, String(fuellstand).c_str(), true);
    client.publish(inhalt_topic, String(inhalt).c_str(), true);
    delay(60000); // Check every 60.000 ms = 60 s = 1 min
    //delay(10000); // Check every 10.000 ms = 10 s
}
