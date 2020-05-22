#include "Redirector.h"

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSocketNotifier>
#include <QFile>
#include <QProcess>
#include <QDebug>
#include <stdio.h>
#include <unistd.h>
#include <iostream>


Redirector::Redirector(QObject *parent)
  : QObject(parent)
  , mNam(new QNetworkAccessManager(this))
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

  const QString url(args[1]);


  connect(p, &QProcess::readyReadStandardOutput,
          this, [this, p, url]()
  {
    QByteArray data(p->readAllStandardOutput());
    //qWarning() << "stdin" << data;

    QUrl u(url);
    QNetworkRequest request(u);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    request.setRawHeader("User-Agent", "Mozilla/5.0 (X11; Fedora; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.138 Safari/537.36");


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
  });


  connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          this, [](int x)
  {
    qWarning() << "finished" << x;
  });


  connect(p, &QProcess::errorOccurred,
          this, [p]()
  {
    qWarning() << "error" << p->errorString();
  });

  p->start();
}
