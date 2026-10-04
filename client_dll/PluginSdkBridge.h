#pragma once
class GtaAdapter;
namespace PluginSdkBridge {
bool Install(GtaAdapter* adapter);
void Uninstall();
bool Available();
}
