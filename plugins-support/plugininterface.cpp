#include "plugininterface.h"

#include "export/pluginconnectioninfo.h"
#include "export/pluginconnectionserializer.h"
#include "export/pluginfacade.h"
#include "export/pluginmessagecommunicator.h"
#include "export/pluginwidget.h"
#include "pluginmessagecommunicatorwrapper.h"
#include "pluginwidgetwrapper.h"

#include <dynalo/dynalo.hpp>
plugin_support::PluginInterface::PluginInterface(dynalo::library&& lib,
                                                 PluginFacade* facade,
                                                 PlugingMetaInfo&& info) :
    mLib(std::make_unique<dynalo::library>(std::move(lib))),
    mFacade(facade), mMetaInfo(std::move(info)) {}

std::unique_ptr<plugin_support::PluginWidgetWrapper>
plugin_support::PluginInterface::getWidget() noexcept {
  return std::make_unique<plugin_support::PluginWidgetWrapper>(
      mFacade->getWidget(), shared_from_this());
}

const PluginConnectionSerializer*
plugin_support::PluginInterface::getSerializer() const noexcept {
  return mFacade->getSerializer();
}

std::shared_ptr<const PluginAddressValidator>
plugin_support::PluginInterface::getAddressValidator() const noexcept {
  return std::shared_ptr<const PluginAddressValidator>(
      mFacade->getAddressValidator(),
      [_interface = this->shared_from_this()](const auto*) {

      });
}

std::unique_ptr<plugin_support::PluginMessageCommunicatorWrapper>
plugin_support::PluginInterface::getCommunicator() noexcept {
  auto comm = std::unique_ptr<PluginMessageCommunicator, owned_deletor>(
      mFacade->makeCommunicator(), owned_deletor(mFacade));
  return std::make_unique<plugin_support::PluginMessageCommunicatorWrapper>(
      std::move(comm), shared_from_this(), mFacade);
}

std::shared_ptr<PluginConnectionInfo>
plugin_support::PluginInterface::makeConnInfo() const noexcept {
  return std::shared_ptr<PluginConnectionInfo>(
      mFacade->makeEmptyConn(), plugin_support::owned_deletor(mFacade));
}

std::string plugin_support::PluginInterface::getName() const noexcept {
  return mMetaInfo.name;
}

std::string plugin_support::PluginInterface::getId() const noexcept {
  return mFacade->getId();
}

std::string plugin_support::PluginInterface::getTranslationFileName(
    Language lang) const noexcept {
  auto iter = mMetaInfo.languages.find(lang);
  return iter == mMetaInfo.languages.cend() ? "" : iter->second;
}

std::string plugin_support::PluginInterface::getGettextDomain() const noexcept {
  return mMetaInfo.gettextDomain;
}

std::string plugin_support::PluginInterface::getLocaleFolder() const noexcept {
  return mMetaInfo.localeFolder;
}
