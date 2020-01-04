#include "messagecontainer.h"
#include "interfaces/messageconteinereventhandler.h"
#include "usermessage.h"

#include <libintl.h>
#include "fmt/core.h"

void MessageContainer::setActiveDialog(const std::string& dialogId) {
  std::lock_guard<std::recursive_mutex> lock(mMutex);
  if (dialogId.empty()) {
    mActiveDialog.clear();
  } else if (dialogId != mActiveDialog) {
    mActiveDialog = dialogId;
    if (0 == mMessages.count(dialogId)) {
      mMessages.emplace(dialogId, DialogMessages());
    }
    notifymAboutDialogIdChanged(dialogId);
    notifyPeekAllMessagesFromActive();
  }
}

void MessageContainer::addMessage(const std::string& dialogId,
                                  const std::string& message,
                                  bool isIncome) {
  auto type = isIncome ? UserMessage::Type::In : UserMessage::Type::Out;
  auto status = isIncome ? UserMessage::Status::Deliveried
                         : UserMessage::Status::WaitDelivery;
  std::lock_guard<std::recursive_mutex> lock(mMutex);
  auto msg = std::make_shared<UserMessage>(status, type, message);
  mMessages[dialogId].push_back(msg);
  notifyAboutNewMessage(dialogId, msg);
}

void MessageContainer::addMessage(
    const std::string& dialogId,
    const std::shared_ptr<UserMessage>& messsage) {
  std::lock_guard<std::recursive_mutex> lock(mMutex);
  mMessages[dialogId].push_back(messsage);
  notifyAboutNewMessage(dialogId, messsage);
}

std::unique_ptr<MessageContainer::Wrapper>
MessageContainer::addToActiveDialogWithWrapper(const std::string& message,
                                               bool isIncome) {
  std::lock_guard<std::recursive_mutex> lock(mMutex);
  if (mMessages.count(mActiveDialog) == 0) {
    throw std::runtime_error(fmt::format(
        dgettext("messages",
                 "MessageConteiner not contains active dialog id: [{}]"),
        mActiveDialog));
  }
  auto type = isIncome ? UserMessage::Type::In : UserMessage::Type::Out;
  auto status = isIncome ? UserMessage::Status::Deliveried
                         : UserMessage::Status::WaitDelivery;
  auto msg = std::make_shared<UserMessage>(status, type, message);
  mMessages[mActiveDialog].push_back(msg);
  notifyAboutNewMessage(mActiveDialog, msg);

  return std::make_unique<MessageContainer::Wrapper>(this->shared_from_this(),
                                                     msg, mActiveDialog);
}

std::unique_ptr<MessageContainer::Wrapper>
MessageContainer::addToActiveDialogWithWrapper(std::string&& message,
                                               bool isIncome) {
  std::lock_guard<std::recursive_mutex> lock(mMutex);
  if (mMessages.count(mActiveDialog) == 0) {
    throw std::runtime_error(fmt::format(
        dgettext("messages",
                 "MessageConteiner not contains active dialog id: [{}]"),
        mActiveDialog));
  }
  auto type = isIncome ? UserMessage::Type::In : UserMessage::Type::Out;
  auto status = isIncome ? UserMessage::Status::Deliveried
                         : UserMessage::Status::WaitDelivery;
  auto msg = std::make_shared<UserMessage>(status, type, std::move(message));
  mMessages[mActiveDialog].push_back(msg);
  notifyAboutNewMessage(mActiveDialog, msg);

  return std::make_unique<MessageContainer::Wrapper>(this->shared_from_this(),
                                                     msg, mActiveDialog);
}

void MessageContainer::addMessageToActivedialog(const std::string& message,
                                                bool isIncome) {
  std::lock_guard<std::recursive_mutex> lock(mMutex);
  if (mMessages.count(mActiveDialog) == 0) {
    throw std::runtime_error(fmt::format(
        dgettext("messages",
                 "MessageConteiner not contains active dialog id: [{}]"),
        mActiveDialog));
  }

  addMessage(mActiveDialog, message, isIncome);
}

void MessageContainer::registerHandler(
    const std::shared_ptr<MessageContainerEventHandler>& handler) {
  std::lock_guard<std::recursive_mutex> lock(mMutex);
  mEventHandlers.push_back(handler);
}

void MessageContainer::cleareHandlers() {
  std::lock_guard<std::recursive_mutex> lock(mMutex);
  mEventHandlers.clear();
}

void MessageContainer::messageChanged(
    const std::string& dialogId,
    const std::shared_ptr<UserMessage>& /*message*/) {
  if (mMessages.count(dialogId) > 0) {
    for (const auto& handler : mEventHandlers) {
      handler->invalidateData(dialogId);
    }
  }
}

std::string MessageContainer::getActiveDialog() const {
  std::lock_guard<std::recursive_mutex> lock(mMutex);
  return mActiveDialog;
}

void MessageContainer::list(
    const std::string& dialogId,
    const std::shared_ptr<MessageContainerEventHandler>& handler) const {
  std::lock_guard<std::recursive_mutex> lock(mMutex);
  if (mMessages.count(mActiveDialog) == 0) {
    throw std::runtime_error(fmt::format(
        dgettext("messages", "MessageConteiner not contains dialog id: {}"),
        dialogId));
  }
  const auto& messages = mMessages.at(dialogId);
  for (const auto& message : messages) {
    handler->peekMessage(message);
  }
}

void MessageContainer::removeDialog(const std::string& dialogId) {
  std::lock_guard<std::recursive_mutex> lock(mMutex);
  if (mMessages.count(dialogId) > 0) {
    mMessages.erase(dialogId);
  }

  if (dialogId == mActiveDialog) {
    mActiveDialog.clear();

    if (!mMessages.empty()) {
      mActiveDialog = std::cbegin(mMessages)->first;
      notifymAboutDialogIdChanged(mActiveDialog);
      notifyPeekAllMessagesFromActive();
    }
  }
}

void MessageContainer::notifymAboutDialogIdChanged(
    const std::string& newDialogId) const {
  for (const auto& handler : mEventHandlers) {
    handler->activeDialogChanged(newDialogId);
  }
}

void MessageContainer::notifyAboutNewMessage(
    const std::string& dialogId,
    const std::shared_ptr<const UserMessage>& message) const {
  for (const auto& handler : mEventHandlers) {
    handler->messageAdded(dialogId, message);
  }
}

void MessageContainer::notifyPeekAllMessagesFromActive() const {
  if (mMessages.count(mActiveDialog) > 0) {
    const auto& messages = mMessages.at(mActiveDialog);
    for (const auto& handler : mEventHandlers) {
      for (const auto& message : messages) {
        handler->peekMessage(message);
      }
    }
  }
}

MessageContainer::Wrapper::Wrapper(std::shared_ptr<MessageContainer> container,
                                   std::shared_ptr<UserMessage> message,
                                   std::string dialogid) :
    mMessage(std::move(message)),
    mContainer(std::move(container)), mDialogId(std::move(dialogid))

{}

bool MessageContainer::Wrapper::save() {
  if (auto lock = mContainer.lock()) {
    lock->messageChanged(mDialogId, mMessage);
  }
  return true;
}

std::shared_ptr<UserMessage> MessageContainer::Wrapper::get() {
  return mMessage;
}
