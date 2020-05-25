#pragma once

#include <QObject>


class QNetworkAccessManager;


class Redirector : public QObject
{
  Q_OBJECT
public:
  explicit Redirector(QObject *parent = nullptr);

private:
  void sendData(const QString &subUrl, const QByteArray &data);

  QNetworkAccessManager *mNam;
  QString mUrl;
  QString mUuid;
};
