#include <QCoreApplication>

#include "Redirector.h"


int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  Redirector r;

  return a.exec();
}
