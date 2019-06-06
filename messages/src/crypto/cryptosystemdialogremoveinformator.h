#ifndef CRYPTOSYSTEMDIALOGREMOVEINFORMATOR_H
#define CRYPTOSYSTEMDIALOGREMOVEINFORMATOR_H

#include "containers/dialogmanager.h"
#include "interfaces/changewatcher.h"

class CryptoSystemImpl;

/**
 * @brief  Отслеживает изменения в контейнере диалогов, используется для
 * актуализации данных в CryptoSystemImpl
 */
class CryptoSystemDialogRemoveInformator
    : public ChangeWatcher<DialogManager::const_element> {
 public:
  explicit CryptoSystemDialogRemoveInformator(
      std::shared_ptr<CryptoSystemImpl> system);

 public:
  void added(const element&) override;
  void changed(const element&) override;
  void removed(const element& obj) override;

 private:
  std::shared_ptr<CryptoSystemImpl> mSystem;
};

#endif  // CRYPTOSYSTEMDIALOGREMOVEINFORMATOR_H
