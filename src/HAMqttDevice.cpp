#include "HAMqttDevice.h"


HAMqttDevice::HAMqttDevice(
    const String& name, 
    const DeviceType type, 
    const String& haMQTTPrefix) :
    _name(name), 
    _type(type)
{
    // Id = name to lower case replacing spaces by underscore (ex: name="Kitchen Light" -> id="kitchen_light")
    _identifier = name;
    _identifier.replace(' ', '_');
    _identifier.toLowerCase();

    // Define the MQTT topic of the device
    _topic = haMQTTPrefix + '/' + deviceTypeToStr(_type) + '/' + _identifier;

    // Preconfigure mandatory config vars that we already know
    addConfigVar("~", _topic);
    addConfigVar("name", _name);

    // When the command topic is mandatory, enable it.
    switch(type)
    {
        case DeviceType::ALARM_CONTROL_PANEL:
        case DeviceType::FAN:
        case DeviceType::LIGHT:
        case DeviceType::LOCK:
        case DeviceType::SWITCH:
            enableCommandTopic();
    }

    // When the state topic is mandatory, enable it.
    switch(type)
    {
        case DeviceType::ALARM_CONTROL_PANEL:
        case DeviceType::BINARY_SENSOR:
        case DeviceType::FAN:
        case DeviceType::LIGHT:
        case DeviceType::LOCK:
        case DeviceType::SENSOR:
        case DeviceType::SWITCH:
            enableStateTopic();
    }
}

HAMqttDevice::~HAMqttDevice(){}

HAMqttDevice& HAMqttDevice::enableCommandTopic()
{
    static bool firstCall = true;

    if(firstCall)
    {
        addConfigVar("cmd_t", "~/cmd");
        firstCall = false;
    }
    return *this;
}

HAMqttDevice& HAMqttDevice::enableStateTopic(const String& topic="~/state")
{
    bool firstCall = true;
    
    String name = "stat_t";
    for (uint8_t i = 0 ; i < _configVars.size() ; i++)
    {
        if(_configVars[i].key==name) {
            firstCall = false;
        }
    }    
    return *this;
}

HAMqttDevice& HAMqttDevice::enableAttributesTopic(const String& topic="~/attr")
{
    bool firstCall = true;

    String name = "json_attr_t";
    
    for (uint8_t i = 0 ; i < _configVars.size() ; i++)
    {
        if(_configVars[i].key==name) {
            firstCall = false;
        }
    }
    
    if(firstCall) {
        addConfigVar(name, topic);
    }
    
    return *this;
}


HAMqttDevice& HAMqttDevice::enableAvailabilityTopic(const String& name="~/availability")
{
    bool firstCall = true;
    for (uint8_t i = 0 ; i < _configVars.size() ; i++)
    {
        if(_configVars[i].key==name) {
            firstCall = false;
        }
    }
    if(firstCall) {
        addConfigVar("avty_t", name);
    }
    return *this;
}

HAMqttDevice& HAMqttDevice::addConfigVar(const String& name, const String& value)
{
    _configVars.push_back({ name, value });
    return *this;
}

HAMqttDevice& HAMqttDevice::addAttribute(const String& name, const String& value)
{
    _attributes.push_back({ name, value });
    return *this;
}

HAMqttDevice& HAMqttDevice::clearAttributes()
{
    _attributes.clear();
    return *this;
}

const String HAMqttDevice::getConfigPayload() const
{
    String configPayload = "{";

    for (uint8_t i = 0 ; i < _configVars.size() ; i++)
    {
        configPayload.concat('"');
        configPayload.concat(_configVars[i].key);
        configPayload.concat("\":");

        bool valueIsDictionnary = _configVars[i].value[0] == '{';

        if (!valueIsDictionnary)
            configPayload.concat('"');

        configPayload.concat(_configVars[i].value);

        if (!valueIsDictionnary)
            configPayload.concat('"');

        configPayload.concat(',');
    }
    configPayload.setCharAt(configPayload.length()-1, '}');

    return configPayload;
}

const String HAMqttDevice::getAttributesPayload() const
{
    String attrPayload = "{";

    for (uint8_t i = 0 ; i < _attributes.size() ; i++)
    {
        attrPayload.concat('"');
        attrPayload.concat(_attributes[i].key);
        attrPayload.concat("\":\"");
        attrPayload.concat(_attributes[i].value);
        attrPayload.concat("\",");
    }
    attrPayload.setCharAt(attrPayload.length()-1, '}');

    return attrPayload;
}

String HAMqttDevice::deviceTypeToStr(DeviceType type)
{
    switch (type)
    {
        case DeviceType::ALARM_CONTROL_PANEL: return "alarm_control_panel";
        case DeviceType::BINARY_SENSOR: return "binary_sensor";
        case DeviceType::CAMERA: return "camera";
        case DeviceType::COVER: return "cover";
        case DeviceType::FAN: return "fan";
        case DeviceType::LIGHT: return "light";
        case DeviceType::LOCK: return "lock";
        case DeviceType::SENSOR: return "sensor";
        case DeviceType::SWITCH: return "switch";
        case DeviceType::CLIMATE: return "climate";
        case DeviceType::VACUUM: return "vacuum";
        default: return "[Unknown DeviceType]";
    }
}

