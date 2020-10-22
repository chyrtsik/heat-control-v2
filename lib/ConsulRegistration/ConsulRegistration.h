#ifndef __CONSUL_REGISTRATION_INCLUDED__
#define __CONSUL_REGISTRATION_INCLUDED__

#include <Arduino.h>
#include <Switch.h>

/**
 * Consul client, which is reponsible for this instance to be discoverable.
 */
class ConsulRegistration{
  private:
    int refreshInterval;
    const char *serviceName;
    unsigned long lastRefresh;

    char requestBuffer[512] = {};
    const char *consulEndpoint = "http://consul.service.consul:8500/v1/catalog/register"; //TODO - extract to config

  public:
    ConsulRegistration(const char *serviceName, int refreshInterval);
    void refresh();

  private:
    boolean registerInConsul();
    boolean submitServerIp(const char *ip);
    const char *createPayload(const char *ip, const char *serviceName);
};

#endif //__CONSUL_REGISTRATION_INCLUDED__