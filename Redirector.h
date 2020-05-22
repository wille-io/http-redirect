#pragma once

#include <QObject>


class QNetworkAccessManager;


class Redirector : public QObject
{
  Q_OBJECT
public:
  explicit Redirector(QObject *parent = nullptr);

private:
  QNetworkAccessManager *mNam;

};
