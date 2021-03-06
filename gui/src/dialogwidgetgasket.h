#ifndef DIALOGWIDGETGASKET_H
#define DIALOGWIDGETGASKET_H

#include <memory>
#include "interfaces/abstractuserask.h"
#include "interfaces/abstractusernotifier.h"

#include <QApplication>

/**
 * Прокладка между контейнером и виджетом, который отображает конкретный элемент
 * в диалоге управления.
 */
template <typename Container, typename Widget>
class DialogWidgetGasket {
 public:
  DialogWidgetGasket(std::shared_ptr<Container> container,
                     Widget* widget,
                     std::shared_ptr<AbstractUserAsk> userAsk,
                     std::shared_ptr<AbstractUserNotifier> notifier);

 public:
  /**
   * @brief Получает элемент из контейнера по указанной позиции и передает его в
   * виджет для отображения
   * @param pos позиция элемента в контейнере
   */
  void viewAt(std::string id);

  /**
   * @brief Получает из виджета элемент и обновляет его в контейнере
   */
  void update();

  /**
   * @brief Удаляет элемент из контейнера по указанной позиции
   * @param pos позиция элемента в контейнере для удаления
   */
  void removeAt(std::string id);

  /**
   * @brief Получает из виджета элемент и сохраняет его в контейнере как новый
   */
  void add();

 private:
  std::shared_ptr<Container> mContainer;
  Widget* mWidget;
  std::shared_ptr<AbstractUserAsk> mUserAsk;
  std::shared_ptr<AbstractUserNotifier> mUserNotifier;
};

template <typename Container, typename Widget>
DialogWidgetGasket<Container, Widget>::DialogWidgetGasket(
    std::shared_ptr<Container> container,
    Widget* widget,
    std::shared_ptr<AbstractUserAsk> userAsk,
    std::shared_ptr<AbstractUserNotifier> notifier) :
    mContainer(std::move(container)),
    mWidget(widget), mUserAsk(std::move(userAsk)),
    mUserNotifier(std::move(notifier)) {}

template <typename Container, typename Widget>
void DialogWidgetGasket<Container, Widget>::viewAt(std::string id) {
  try {
    auto element = mContainer->get(id);
    mWidget->setElement(element);
  } catch (std::exception& ex) {
    mUserNotifier->notify(AbstractUserNotifier::Severity::Error, ex.what());
  }
}

template <typename Container, typename Widget>
void DialogWidgetGasket<Container, Widget>::update() {
  try {
    auto element = mWidget->getElement();
    mContainer->update(element);
  } catch (const std::exception& ex) {
    mUserNotifier->notify(AbstractUserNotifier::Severity::Error, ex.what());
  }
}

template <typename Container, typename Widget>
void DialogWidgetGasket<Container, Widget>::removeAt(std::string id) {
  if (mUserAsk->ask(QApplication::tr("Are you sure to remove selected element?")
                        .toStdString())) {
    mContainer->remove(id);
  }
}

template <typename Container, typename Widget>
void DialogWidgetGasket<Container, Widget>::add() {
  try {
    auto element = mWidget->getElement();
    mContainer->add(element);
    mWidget->actionCleare();
  } catch (const std::exception& ex) {
    std::string err = ex.what();
    mUserNotifier->notify(AbstractUserNotifier::Severity::Error, ex.what());
  }
}

#endif  // DIALOGWIDGETGASKET_H
