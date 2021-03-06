#include "recieveddiscoveredcontactsdialog.h"

#include <QMessageBox>
#include "models/discovered-contacts/discoveredcontactmodel.h"
#include "wrappers/recievedcontactsstoragewrapper.h"

Q_DECLARE_METATYPE(std::string)

RecievedDiscoveredContactsDialog::RecievedDiscoveredContactsDialog(
    DiscoveredContactModel* model,
    std::shared_ptr<RecievedContactsStorageWrapper> storageWrapper,
    QWidget* parent) :
    TranslateChangeEventHandler<QDialog, Ui::RecievedDiscoveredContactsDialog>(
        parent),
    ui(new Ui::RecievedDiscoveredContactsDialog),
    mStorageWrapper(std::move(storageWrapper)) {
  ui->setupUi(this);

  ui->recievedContacts->setModel(model);
  ui->recievedContacts->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->recievedContacts, &QListView::customContextMenuRequested, this,
          &RecievedDiscoveredContactsDialog::requestToShowMenu);
  this->setUI(ui);
}

RecievedDiscoveredContactsDialog::~RecievedDiscoveredContactsDialog() {
  delete ui;
}

void RecievedDiscoveredContactsDialog::requestToShowMenu(QPoint pos) {
  const auto index = ui->recievedContacts->indexAt(pos);
  if (!index.isValid()) {
    return;
  }
  const auto data = ui->recievedContacts->model()->data(
      index, DiscoveredContactModel::ContactRole);
  if (!data.isValid() || !data.canConvert<std::string>()) {
    return;
  }
  const auto dialogId = qvariant_cast<std::string>(data);
  emit showDialogMenu(ui->recievedContacts->viewport()->mapToGlobal(pos),
                      dialogId);
}

void RecievedDiscoveredContactsDialog::removeRecievedContact(
    std::string dialogId) {
  const auto res = QMessageBox::question(
      this, tr("Removing recieved contact information"),
      tr("Are sure to remove recieved contact information?"),
      QMessageBox::Apply | QMessageBox::Cancel);
  if (QMessageBox::Apply == res) {
    if (mStorageWrapper->removeDiscoveredContact(dialogId)) {
      QMessageBox::information(this, tr("Successful!"),
                               tr("Information was removed successful!"));
    } else {
      QMessageBox::warning(
          this, tr("Error!"),
          tr("Error occured during removing. Please, try later!"));
    }
  }
}
