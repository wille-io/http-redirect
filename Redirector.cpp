#include "Redirector.h"

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSocketNotifier>
#include <QFile>
#include <QProcess>
#include <QUuid>
#include <QDebug>
#include <stdio.h>
#include <unistd.h>
#include <iostream>


Redirector::Redirector(QObject *parent)
  : QObject(parent)
  , mNam(new QNetworkAccessManager(this))
  , mUuid(QUuid::createUuid().toString())
{
  const QStringList args(QCoreApplication::arguments());

  if (args.count() < 3)
  {
    qCritical() << "usage:" << args[0] << "<post url> <program> [args ...]";
    abort();
  }

  QProcess *p = new QProcess(this);

  p->setProgram(args[2]);
  p->setArguments(args.mid(3));

  mUrl = args[1];


  connect(p, &QProcess::readyReadStandardOutput,
          this, [this, p]()
  {
    QByteArray data(p->readAllStandardOutput());
    //qWarning() << "stdout" << data;
    sendData("stdout", data);
  });


  connect(p, &QProcess::readyReadStandardError,
          this, [this, p]()
  {
    QByteArray data(p->readAllStandardError());
    //qWarning() << "stderr" << data;
    sendData("stderr", data);
  });


  connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          this, [this](int x)
  {
    qWarning() << "process finished" << x;
    sendData("finished", QString::number(x).toUtf8());
  });


  connect(p, &QProcess::errorOccurred,
          this, [this, p]()
  {
    qWarning() << "process error" << p->errorString();
    sendData("error", p->errorString().toUtf8());
  });


  connect(p, &QProcess::started,
          this, [this, p]()
  {
    qWarning() << "process started" << p->processId();
    sendData("started", QString::number(p->processId()).toUtf8());
  });


  p->start();


  sendData("connected", {});
}


void Redirector::sendData(const QString &subUrl, const QByteArray &data)
{
  QUrl u(QString("%1/%2").arg(mUrl, subUrl));
  QNetworkRequest request(u);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
  request.setRawHeader("User-Agent", "http-redirect");
  request.setRawHeader("Uuid", mUuid.toUtf8());

  QNetworkReply *reply = mNam->post(request, data);


  connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
          this, [reply](QNetworkReply::NetworkError code)
  {
    qWarning() << "network error while sending data" << code << reply->errorString();
    reply->deleteLater();
  });


  connect(reply, &QNetworkReply::finished,
          this, [reply]()
  {
    reply->deleteLater();
  });
}
